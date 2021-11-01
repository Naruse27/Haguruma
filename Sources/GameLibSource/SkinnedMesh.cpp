#include "misc.h"
#include "SkinnedMesh.h"
#include <sstream>
#include <functional>
#include <filesystem>
#include "texture.h"
#include <filesystem>
#include <fstream>
#include "Shader.h"
#include "ResourceManager.h"
#include "Light.h"


struct BoneInfluence
{
	uint32_t boneIndex;
	float boneWeight;
};
//エイリアス宣言
using BoneInfluencesPerControlPoint = std::vector<BoneInfluence>;

void FetchBoneInfluences(const FbxMesh* fbxMesh, std::vector<BoneInfluencesPerControlPoint>& boneInfluences) {
	const int controlPointsCount = fbxMesh->GetControlPointsCount();
	boneInfluences.resize(controlPointsCount);

	const int skinCount = fbxMesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int skinIndex = 0; skinIndex < skinCount; ++skinIndex) {
		const FbxSkin* fbxSkin = static_cast<FbxSkin*>(fbxMesh->GetDeformer(skinIndex, FbxDeformer::eSkin));

		const int clusterCount = fbxSkin->GetClusterCount();
		for (int clusterIndex = 0; clusterIndex < clusterCount; ++clusterIndex) {
			const FbxCluster* fbxCluster = fbxSkin->GetCluster(clusterIndex);

			const int controlPointIndicesCount = fbxCluster->GetControlPointIndicesCount();
			for (int controlPointIndicesIndex = 0; controlPointIndicesIndex < controlPointIndicesCount; ++controlPointIndicesIndex) {
				int controlPointIndex = fbxCluster->GetControlPointIndices()[controlPointIndicesIndex];
				double controlPointWeight = fbxCluster->GetControlPointWeights()[controlPointIndicesIndex];
				BoneInfluence& boneInfluence = boneInfluences.at(controlPointIndex).emplace_back();
				boneInfluence.boneIndex = static_cast<uint32_t>(clusterIndex);
				boneInfluence.boneWeight = static_cast<float>(controlPointWeight);
			}
		}
	}
}

SkinnedMesh::SkinnedMesh(ID3D11Device* device, const char* fbxFilename, bool triangulate, float samplingRate) {

	std::filesystem::path cerealFilename(fbxFilename);
	cerealFilename.replace_extension("Numbers");
	if (std::filesystem::exists(cerealFilename.c_str())) {
		std::ifstream ifs(cerealFilename.c_str(), std::ios::binary);
		cereal::BinaryInputArchive deserialization(ifs);
		deserialization(sceneView, meshes, materials, animationClips);
	}
	else {
		FbxManager* fbxManager = FbxManager::Create();
		FbxScene* fbxScene = FbxScene::Create(fbxManager, "");
		FbxImporter* fbxImporter = FbxImporter::Create(fbxManager, "");

		bool importStatus = fbxImporter->Initialize(fbxFilename);
		_ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());

		importStatus = fbxImporter->Import(fbxScene);
		_ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());

		FbxGeometryConverter fbxConverter(fbxManager);
		if (triangulate) {
			fbxConverter.Triangulate(fbxScene, true/*replace*/, false/*legacy*/);
			fbxConverter.RemoveBadPolygonsFromMeshes(fbxScene);
		}

		std::function<void(FbxNode*)> traverse = [&](FbxNode* fbxNode) {
			Scene::Node& node = sceneView.nodes.emplace_back();
			node.attribute = fbxNode->GetNodeAttribute() ? fbxNode->GetNodeAttribute()->GetAttributeType() : FbxNodeAttribute::EType::eUnknown;
			node.name = fbxNode->GetName();
			node.uniqueId = fbxNode->GetUniqueID();
			node.parentIndex = sceneView.indexof(fbxNode->GetParent() ? fbxNode->GetParent()->GetUniqueID() : 0);
			for (int childIndex = 0; childIndex < fbxNode->GetChildCount(); ++childIndex) {
				traverse(fbxNode->GetChild(childIndex));
			}
		};
		traverse(fbxScene->GetRootNode());


#if 0
		for (const Scene::Node& node : sceneView.nodes) {
			FbxNode* fbxNode = fbxScene->FindNodeByName(node.name.c_str());
			// Display node data in the output window as debug
			std::string nodeName = fbxNode->GetName();
			uint64_t uid = fbxNode->GetUniqueID();
			uint64_t parentUid = fbxNode->GetParent() ? fbxNode->GetParent()->GetUniqueID() : 0;
			int32_t type = fbxNode->GetNodeAttribute() ? fbxNode->GetNodeAttribute()->GetAttributeType() : 0;
			std::stringstream debugString;
			debugString << nodeName << ":" << uid << ":" << parentUid << ":" << type << "\n";
			OutputDebugStringA(debugString.str().c_str());
		}
#endif

		FetchMeshes(fbxScene, meshes);
		FetchMaterials(fbxScene, materials);
		FetchAnimations(fbxScene, animationClips, samplingRate);
		fbxManager->Destroy();

		std::ofstream ofs(cerealFilename.c_str(), std::ios::binary);
		cereal::BinaryOutputArchive serialization(ofs);
		serialization(sceneView, meshes, materials, animationClips);
	}
	CreateComObjects(device, fbxFilename);
}

void SkinnedMesh::FetchMeshes(FbxScene* fbxScene, std::vector<Mesh>& meshes) {
	for (const Scene::Node& node : sceneView.nodes) {
		if (node.attribute != FbxNodeAttribute::EType::eMesh) {
			continue;
		}
		//                                                              ↓nodeの名前が絶対違うときのみ　やるなら自分でuniqueIdを使って分ける
		FbxNode* fbxNode = fbxScene->FindNodeByName(node.name.c_str());
		FbxMesh* fbxMesh = fbxNode->GetMesh();

		Mesh& mesh = meshes.emplace_back();
		{
			mesh.uniqueId = fbxMesh->GetNode()->GetUniqueID();
			mesh.name = fbxMesh->GetNode()->GetName();
			mesh.nodeIndex = sceneView.indexof(mesh.uniqueId);
			mesh.defaultGlobalTransform = to_xmfloat4x4(fbxMesh->GetNode()->EvaluateGlobalTransform());
		}

		std::vector<BoneInfluencesPerControlPoint> boneInfluences;
		FetchBoneInfluences(fbxMesh, boneInfluences);
		FetchSkeleton(fbxMesh, mesh.bindPose);

		std::vector<Mesh::Subset>& subsets = mesh.subsets;
		const int materialCount = fbxMesh->GetNode()->GetMaterialCount();
		subsets.resize(materialCount > 0 ? materialCount : 1);
		for (int materialIndex = 0; materialIndex < materialCount; ++materialIndex) {
			const FbxSurfaceMaterial* fbxMaterial = fbxMesh->GetNode()->GetMaterial(materialIndex);
			subsets.at(materialIndex).materialName = fbxMaterial->GetName();
			subsets.at(materialIndex).materialUniqueId = fbxMaterial->GetUniqueID();
		}
		if (materialCount > 0) {
			const int polygonCount = fbxMesh->GetPolygonCount();
			for (int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex) {
				const int materialIndex = fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(polygonIndex);
				subsets.at(materialIndex).indexCount += 3;
			}
			uint32_t offset = 0;
			for (Mesh::Subset& subset : subsets) {
				subset.startIndexLocation = offset;
				offset += subset.indexCount;
				// This will be used as counter in the following procedures, reset to zero
				subset.indexCount = 0;
			}
		}

		const int polygonCount = fbxMesh->GetPolygonCount();
		mesh.vertices.resize(polygonCount * 3LL);
		mesh.indices.resize(polygonCount * 3LL);


		FbxStringList uvNames;
		fbxMesh->GetUVSetNames(uvNames);
		const FbxVector4* controlPoints = fbxMesh->GetControlPoints();
		for (int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex) {
			const int materialIndex = materialCount > 0 ? fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(polygonIndex) : 0;
			Mesh::Subset& subset = subsets.at(materialIndex);
			const uint32_t offset = subset.startIndexLocation + subset.indexCount;

			for (int positionInPolygon = 0; positionInPolygon < 3; ++positionInPolygon) {
				const int vertexIndex = polygonIndex * 3 + positionInPolygon;

				Vertex vertex;
				const int polygonVertex = fbxMesh->GetPolygonVertex(polygonIndex, positionInPolygon);
				vertex.position.x = static_cast<float>(controlPoints[polygonVertex][0]);
				vertex.position.y = static_cast<float>(controlPoints[polygonVertex][1]);
				vertex.position.z = static_cast<float>(controlPoints[polygonVertex][2]);


#if 1
				//ボーンの影響を４つ以上に対応してないUNIT22
				const BoneInfluencesPerControlPoint& influencesPerControlPoint = boneInfluences.at(polygonVertex);
				for (int influenceIndex = 0; influenceIndex < influencesPerControlPoint.size(); ++influenceIndex) {
					if (influenceIndex < MAX_BONE_INFLUENCES) {
						vertex.boneWeights[influenceIndex] = influencesPerControlPoint.at(influenceIndex).boneWeight;
						vertex.boneIndices[influenceIndex] = influencesPerControlPoint.at(influenceIndex).boneIndex;
					}
				}
#else
				//ボーンの影響を４つ以上に対応済み（未作成）
				const BoneInfluencesPerControlPoint& influencesPerControlPoint = boneInfluences.at(polygonVertex);
				for (int influenceIndex = 0; influenceIndex < influencesPerControlPoint.size(); ++influenceIndex) {
					if (influenceIndex < MAX_BONE_INFLUENCES) {
						vertex.boneWeights[influenceIndex] = influencesPerControlPoint.at(influenceIndex).boneWeight;
						vertex.boneIndices[influenceIndex] = influencesPerControlPoint.at(influenceIndex).boneIndex;
					}
				}

#endif

				if (fbxMesh->GetElementNormalCount() > 0) {
					FbxVector4 normal;
					fbxMesh->GetPolygonVertexNormal(polygonIndex, positionInPolygon, normal);
					vertex.normal.x = static_cast<float>(normal[0]);
					vertex.normal.y = static_cast<float>(normal[1]);
					vertex.normal.z = static_cast<float>(normal[2]);
				}
				if (fbxMesh->GenerateTangentsData(0, false)) {
					const FbxGeometryElementTangent* tangent = fbxMesh->GetElementTangent(0);
					vertex.tangent.x = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[0]);
					vertex.tangent.y = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[1]);
					vertex.tangent.z = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[2]);
					vertex.tangent.w = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[3]);
				}
				if (fbxMesh->GetElementUVCount() > 0) {
					FbxVector2 uv;
					bool unmappedUv;
					fbxMesh->GetPolygonVertexUV(polygonIndex, positionInPolygon, uvNames[0], uv, unmappedUv);
					vertex.texcoord.x = static_cast<float>(uv[0]);
					vertex.texcoord.y = 1.0f - static_cast<float>(uv[1]);
				}
				mesh.vertices.at(vertexIndex) = std::move(vertex);
				//mesh.indices.at(vertexIndex) = vertexIndex;
				mesh.indices.at(static_cast<size_t>(offset) + positionInPolygon) = vertexIndex;
				subset.indexCount++;
			}
		}
		for (const Vertex& v : mesh.vertices)
		{
			mesh.boundingBox[0].x = std::min<float>(mesh.boundingBox[0].x, v.position.x);
			mesh.boundingBox[0].y = std::min<float>(mesh.boundingBox[0].y, v.position.y);
			mesh.boundingBox[0].z = std::min<float>(mesh.boundingBox[0].z, v.position.z);
			mesh.boundingBox[1].x = std::max<float>(mesh.boundingBox[1].x, v.position.x);
			mesh.boundingBox[1].y = std::max<float>(mesh.boundingBox[1].y, v.position.y);
			mesh.boundingBox[1].z = std::max<float>(mesh.boundingBox[1].z, v.position.z);
		}
	}
}

void SkinnedMesh::FetchMaterials(FbxScene* fbxScene, std::unordered_map<uint64_t, Material>& materials) {
	const size_t nodeCount = sceneView.nodes.size();
	for (size_t nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex) {
		const Scene::Node& node = sceneView.nodes.at(nodeIndex);
		const FbxNode* fbxNode = fbxScene->FindNodeByName(node.name.c_str());

		const int materialCount = fbxNode->GetMaterialCount();
		//マテリアルがある場合
		if (materialCount > 0) {
			for (int materialIndex = 0; materialIndex < materialCount; ++materialIndex) {
				const FbxSurfaceMaterial* fbxMaterial = fbxNode->GetMaterial(materialIndex);
				Material material;
				material.name = fbxMaterial->GetName();
				material.uniqueId = fbxMaterial->GetUniqueID();
				FbxProperty fbxProperty;
				fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
				if (fbxProperty.IsValid()) {
					const FbxDouble3 color = fbxProperty.Get<FbxDouble3>();
					material.diffuse.x = static_cast<float>(color[0]);
					material.diffuse.y = static_cast<float>(color[1]);
					material.diffuse.z = static_cast<float>(color[2]);
					material.diffuse.w = 1.0f;

					const FbxFileTexture* fbxTexture = fbxProperty.GetSrcObject<FbxFileTexture>();
					material.textureFilenames[0] = fbxTexture ? fbxTexture->GetRelativeFileName() : "";
				}
				fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sAmbient);
				if (fbxProperty.IsValid()) {
					const FbxDouble3 color = fbxProperty.Get<FbxDouble3>();
					material.ambient.x = static_cast<float>(color[0]);
					material.ambient.y = static_cast<float>(color[1]);
					material.ambient.z = static_cast<float>(color[2]);
					material.ambient.w = 1.0f;

					const FbxFileTexture* fbxTexture = fbxProperty.GetSrcObject<FbxFileTexture>();
					material.textureFilenames[1] = fbxTexture ? fbxTexture->GetRelativeFileName() : "";
				}
				fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sSpecular);
				if (fbxProperty.IsValid()) {
					const FbxDouble3 color = fbxProperty.Get<FbxDouble3>();
					material.speculer.x = static_cast<float>(color[0]);
					material.speculer.y = static_cast<float>(color[1]);
					material.speculer.z = static_cast<float>(color[2]);
					material.speculer.w = 1.0f;
				}
				//fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::)

				fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sNormalMap);
				if (fbxProperty.IsValid()) {
					const FbxFileTexture* file_texture{ fbxProperty.GetSrcObject<FbxFileTexture>() };
					material.textureFilenames[1] = file_texture ? file_texture->GetRelativeFileName() : "";
				}
				materials.emplace(material.uniqueId, std::move(material));
			}
		}
	}
	materials.emplace();
}

void SkinnedMesh::FetchSkeleton(FbxMesh* fbxMesh, Skeleton& bindPose) {

	const int deformerCount = fbxMesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int deformerIndex = 0; deformerIndex < deformerCount; ++deformerIndex) {
		FbxSkin* skin = static_cast<FbxSkin*>(fbxMesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));
		const int clusterCount = skin->GetClusterCount();
		bindPose.bones.resize(clusterCount);
		for (int clusterIndex = 0; clusterIndex < clusterCount; ++clusterIndex) {
			FbxCluster* cluster = skin->GetCluster(clusterIndex);

			Skeleton::Bone& bone = bindPose.bones.at(clusterIndex);
			{
				bone.name = cluster->GetLink()->GetName();
				bone.uniqueId = cluster->GetLink()->GetUniqueID();
				bone.parentIndex = bindPose.indexof(cluster->GetLink()->GetParent()->GetUniqueID());
				bone.nodeIndex = sceneView.indexof(bone.uniqueId);
			}

			//'reference_global_init_position' is used to convert from local space of model(mesh) to
			// global space of scene.
			FbxAMatrix referenceGlobalInitPosition;
			cluster->GetTransformMatrix(referenceGlobalInitPosition);

			// 'cluster_global_init_position' is used to convert from local space of bone to
			// global space of scene.
			FbxAMatrix clusterGlobalInitPosition;
			cluster->GetTransformLinkMatrix(clusterGlobalInitPosition);

			// Matrices are defined using the Column Major scheme. When a FbxAMatrix represents a transformation
			// (translation, rotation and scale), the last row of the matrix represents the translation part of
			// the transformation.
			// Compose 'bone.offset_transform' matrix that trnasforms position from mesh space to bone space.
			// This matrix is called the offset matrix.
			bone.offsetTransform = to_xmfloat4x4(clusterGlobalInitPosition.Inverse() * referenceGlobalInitPosition);
		}
	}
}

void SkinnedMesh::FetchAnimations(FbxScene* fbxScene, std::vector<Animation>& animationClips, float samplingRate) {

	FbxArray<FbxString*> animationStackNames;
	fbxScene->FillAnimStackNameArray(animationStackNames);
	const int animationStackCount = animationStackNames.GetCount();
	for (int animationStackIndex = 0; animationStackIndex < animationStackCount; ++animationStackIndex) {
		Animation& animationClip = animationClips.emplace_back();
		animationClip.name = animationStackNames[animationStackIndex]->Buffer();

		FbxAnimStack* animationStack = fbxScene->FindMember<FbxAnimStack>(animationClip.name.c_str());
		fbxScene->SetCurrentAnimationStack(animationStack);

		const FbxTime::EMode timeMode = fbxScene->GetGlobalSettings().GetTimeMode();
		FbxTime oneSecond;
		oneSecond.SetTime(0, 0, 1, 0, 0, timeMode);
		animationClip.samplingRate = samplingRate > 0 ? samplingRate : static_cast<float>(oneSecond.GetFrameRate(timeMode));
		const FbxTime samplingInterval = static_cast<FbxLongLong>(oneSecond.Get() / animationClip.samplingRate);
		const FbxTakeInfo* takeInfo = fbxScene->GetTakeInfo(animationClip.name.c_str());
		const FbxTime startTime = takeInfo->mLocalTimeSpan.GetStart();
		const FbxTime stopTime = takeInfo->mLocalTimeSpan.GetStop();
		for (FbxTime time = startTime; time < stopTime; time += samplingInterval) {
			Animation::Keyframe& keyframe = animationClip.sequence.emplace_back();

			const size_t nodeCount = sceneView.nodes.size();
			keyframe.nodes.resize(nodeCount);
			for (size_t nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex) {
				FbxNode* fbxNode = fbxScene->FindNodeByName(sceneView.nodes.at(nodeIndex).name.c_str());
				if (fbxNode) {
					Animation::Keyframe::Node& node = keyframe.nodes.at(nodeIndex);
					// 'global_transform' is a transformation matrix of a node with respect to
					// the scene's global coordinate system.
					node.globalTransform = to_xmfloat4x4(fbxNode->EvaluateGlobalTransform(time));

					// 'local_transform' is a transformation matrix of a node with respect to
					// its parent's local coordinate system.
					const FbxAMatrix& localTransform = fbxNode->EvaluateLocalTransform(time);
					node.scaling = to_xmfloat3(localTransform.GetS());
					node.rotation = to_xmfloat4(localTransform.GetQ());
					node.translation = to_xmfloat3(localTransform.GetT());

				}
			}
		}
	}
	for (int animationStackIndex = 0; animationStackIndex < animationStackCount; ++animationStackIndex) {
		delete animationStackNames[animationStackIndex];
	}
}

void SkinnedMesh::CreateComObjects(ID3D11Device* device, const char* fbxFilename) {
	HRESULT hr = S_OK;

	for (Mesh& mesh : meshes) {
		D3D11_BUFFER_DESC bufferDesc{};
		D3D11_SUBRESOURCE_DATA subresourceData{};
		{
			bufferDesc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * mesh.vertices.size());
			bufferDesc.Usage = D3D11_USAGE_DEFAULT;
			bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bufferDesc.CPUAccessFlags = 0;
			bufferDesc.MiscFlags = 0;
			bufferDesc.StructureByteStride = 0;

			subresourceData.pSysMem = mesh.vertices.data();
			subresourceData.SysMemPitch = 0;
			subresourceData.SysMemSlicePitch = 0;
		}
		hr = device->CreateBuffer(&bufferDesc, &subresourceData, mesh.vertexBuffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		{
			bufferDesc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * mesh.indices.size());
			bufferDesc.Usage = D3D11_USAGE_DEFAULT;
			bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

			subresourceData.pSysMem = mesh.indices.data();
		}
		hr = device->CreateBuffer(&bufferDesc, &subresourceData, mesh.indexBuffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
#if 0
		mesh.vertices.clear();
		mesh.indices.clear();
#endif
	}

#if 0
	for (std::unordered_map<uint64_t, Material>::iterator it = materials.begin(); it != materials.end(); ++it) {
		if (it->second.textureFilenames[0].size() > 0) {
			std::filesystem::path path(fbxFilename);
			path.replace_filename(it->second.textureFilenames[0]);
			D3D11_TEXTURE2D_DESC texture2dDesc;
			load_texture_from_file(device, path.c_str(), it->second.shaderResourceViews[0].GetAddressOf(), &texture2dDesc);
		}
		else {
			make_dumy_texture(device, it->second.shaderResourceViews[0].GetAddressOf(), 0xFFFFFFFF, 16);
		}
	}
#endif
	for (std::unordered_map<uint64_t, Material>::iterator iterator = materials.begin();
		iterator != materials.end(); ++iterator)
	{
		for (size_t texture_index = 0; texture_index < 2; ++texture_index) {
			if (iterator->second.textureFilenames[texture_index].size() > 0) {
				std::filesystem::path path(fbxFilename);
				path.replace_filename(iterator->second.textureFilenames[texture_index]);
				D3D11_TEXTURE2D_DESC texture2d_desc;
				Texture::LoadTextureFromFile(device, path.c_str(),
					iterator->second.shaderResourceViews[texture_index].GetAddressOf(), &texture2d_desc);
			}
			else {
				Texture::MakeDumyTexture(device,
					iterator->second.shaderResourceViews[texture_index].GetAddressOf(), texture_index == 1 ? 0xFFFF7F7F : 0xFFFFFFFF, 16);
			}
		}
	}


	//シーン定数バッファオブジェクト生成
	D3D11_BUFFER_DESC bufferDesc{};
	{
		bufferDesc.ByteWidth = sizeof(Constants);
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	}
	hr = device->CreateBuffer(&bufferDesc, nullptr, constantBuffer.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

bool SkinnedMesh::AppendAnimation(const char* animationFilename, float samplingRate) {
	FbxManager* fbxManager = FbxManager::Create();
	FbxScene* fbxScene = FbxScene::Create(fbxManager, "");

	FbxImporter* fbxImporter = FbxImporter::Create(fbxManager, "");
	bool importStatus = false;
	importStatus = fbxImporter->Initialize(animationFilename);
	_ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());
	importStatus = fbxImporter->Import(fbxScene);
	_ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());

	FetchAnimations(fbxScene, animationClips, samplingRate);

	fbxManager->Destroy();

	return true;
}

void SkinnedMesh::BlendAnimations(const Animation::Keyframe* keyframes[2], float factor, Animation::Keyframe& keyframe) {
	size_t nodeCount = keyframes[0]->nodes.size();
	keyframe.nodes.resize(nodeCount);
	for (size_t nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex) {
		DirectX::XMVECTOR S[2] = {
			DirectX::XMLoadFloat3(&keyframes[0]->nodes.at(nodeIndex).scaling),
			DirectX::XMLoadFloat3(&keyframes[1]->nodes.at(nodeIndex).scaling)
		};
		DirectX::XMStoreFloat3(&keyframe.nodes.at(nodeIndex).scaling, DirectX::XMVectorLerp(S[0], S[1], factor));

		DirectX::XMVECTOR R[2] = {
			DirectX::XMLoadFloat4(&keyframes[0]->nodes.at(nodeIndex).rotation),
			DirectX::XMLoadFloat4(&keyframes[1]->nodes.at(nodeIndex).rotation)
		};
		DirectX::XMStoreFloat4(&keyframe.nodes.at(nodeIndex).rotation, DirectX::XMQuaternionSlerp(R[0], R[1], factor));

		DirectX::XMVECTOR T[2] = {
			DirectX::XMLoadFloat3(&keyframes[0]->nodes.at(nodeIndex).translation),
			DirectX::XMLoadFloat3(&keyframes[1]->nodes.at(nodeIndex).translation)
		};
		DirectX::XMStoreFloat3(&keyframe.nodes.at(nodeIndex).translation, DirectX::XMVectorLerp(T[0], T[1], factor));
	}
}

void SkinnedMesh::Preparation(ID3D11DeviceContext* immediateContext, Shader shader, bool wireframe)
{
   // プリミティブモードの設定
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// シェーダーの設定
	//immediateContext->VSSetShader(vertexShader.Get(), nullptr, 0);
	//immediateContext->PSSetShader(pixelShader.Get(), nullptr, 0);
	//immediateContext->VSSetShader(shader.GetVertexShader().Get(), nullptr, 0);
	//immediateContext->PSSetShader(shader.GetPixelShader().Get(), nullptr, 0);

	// 入力レイアウトの設定
	//immediateContext->IASetInputLayout(shader.GetInputLayout().Get());

	// 深度テストの設定
	immediateContext->OMSetDepthStencilState(depthStencilState.Get(), 0);

	// サンプラーの設定
	immediateContext->PSSetSamplers(0, 1, samplerState.GetAddressOf());


	//ビュープロジェクション変換行列の計算　定数バッファへセット
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	immediateContext->RSGetViewports(&numViewports, &viewport);

	// 入力レイアウト
	//immediateContext->IASetInputLayout(inputLayout.Get());
	immediateContext->IASetInputLayout(shader.GetInputLayout().Get());
	// シェーダーの設定
	immediateContext->VSSetShader(shader.GetVertexShader().Get(), nullptr, 0);
	immediateContext->PSSetShader(shader.GetPixelShader().Get(), nullptr, 0);
	immediateContext->GSSetShader(shader.GetGeometryShader().Get(), nullptr, 0);

}

void SkinnedMesh::Render(ID3D11DeviceContext* deviceContext, const DirectX::XMFLOAT4X4& world, const Vector4& materialColor, Shader shader, const Animation::Keyframe* keyframe) {
	using namespace DirectX;

#if 0
	for (const Mesh& mesh : meshes) {
		uint32_t stride = sizeof(Vertex);
		uint32_t offset = 0;
		deviceContext->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
		deviceContext->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		deviceContext->IASetInputLayout(inputLayout.Get());

		deviceContext->VSSetShader(vertexShader.Get(), nullptr, 0);
		deviceContext->PSSetShader(pixelShader.Get(), nullptr, 0);

		Constants data;
		//data.world = world;
		//XMStoreFloat4x4(&data.world, XMLoadFloat4x4(&mesh.defaultGlobalTransform) * XMLoadFloat4x4(&world));
		const Animation::Keyframe::Node& meshNode = keyframe->nodes.at(mesh.nodeIndex);
		XMStoreFloat4x4(&data.world, XMLoadFloat4x4(&meshNode.globalTransform) * XMLoadFloat4x4(&world));


#if 0// UNIT23
		XMStoreFloat4x4(&data.boneTransforms[0], XMMatrixIdentity());
		XMStoreFloat4x4(&data.boneTransforms[1], XMMatrixRotationRollPitchYaw(0, 0, XMConvertToRadians(+45)));
		XMStoreFloat4x4(&data.boneTransforms[2], XMMatrixRotationRollPitchYaw(0, 0, XMConvertToRadians(-45)));
#endif
#if 0//UNIT24
		// Bind pose transform(Offest matrix) : Convert from the model(mesh) space to the bone space
		XMMATRIX B[3];
		B[0] = XMLoadFloat4x4(&mesh.bindPose.bones.at(0).offsetTransform);
		B[1] = XMLoadFloat4x4(&mesh.bindPose.bones.at(1).offsetTransform);
		B[2] = XMLoadFloat4x4(&mesh.bindPose.bones.at(2).offsetTransform);

		// Animation bone transform : Convert from the bone space to the model(mesh) or the parent bone space
		XMMATRIX A[3];
		// from A0 space to model space
		A[0] = XMMatrixRotationRollPitchYaw(XMConvertToRadians(90), 0, 0);

		// from A1 space to parent bone(A0) space
		A[1] = XMMatrixRotationRollPitchYaw(0, 0, XMConvertToRadians(45)) * XMMatrixTranslation(0, 2, 0);

		// from A2 space to parent bone(A1) space
		A[2] = XMMatrixRotationRollPitchYaw(0, 0, XMConvertToRadians(-45)) * XMMatrixTranslation(0, 2, 0);

		XMStoreFloat4x4(&data.boneTransforms[0], B[0] * A[0]);
		XMStoreFloat4x4(&data.boneTransforms[1], B[1] * A[1] * A[0]);
		XMStoreFloat4x4(&data.boneTransforms[2], B[1] * A[2] * A[1] * A[0]);
#endif

		const size_t boneCount = mesh.bindPose.bones.size();
		for (int boneIndex = 0; boneIndex < boneCount; ++boneIndex) {

			const Skeleton::Bone& bone = mesh.bindPose.bones.at(boneIndex);
			const Animation::Keyframe::Node& boneNode = keyframe->nodes.at(bone.nodeIndex);
			XMStoreFloat4x4(
				&data.boneTransforms[boneIndex],
				XMLoadFloat4x4(&bone.offsetTransform) *
				XMLoadFloat4x4(&boneNode.globalTransform) *
				XMMatrixInverse(nullptr, XMLoadFloat4x4(&mesh.defaultGlobalTransform))
			);

		}




		for (const Mesh::Subset& subset : mesh.subsets) {
			const Material& material = materials.at(subset.materialUniqueId);

			XMStoreFloat4(&data.materialColor, XMLoadFloat4(&materialColor) * XMLoadFloat4(&material.Kd));
			deviceContext->UpdateSubresource(constantBuffer.Get(), 0, 0, &data, 0, 0);
			deviceContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

			deviceContext->PSSetShaderResources(0, 1, material.shaderResourceViews[0].GetAddressOf());

			deviceContext->DrawIndexed(subset.indexCount, subset.startIndexLocation, 0);
		}

		////data.materialColor = materialColor;
		//XMStoreFloat4(&data.materialColor, XMLoadFloat4(&materialColor) * XMLoadFloat4(&materials.cbegin()->second.Kd));
		//deviceContext->UpdateSubresource(constantBuffer.Get(), 0, 0, &data, 0, 0);
		//deviceContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
		//deviceContext->PSSetShaderResources(0, 1, materials.cbegin()->second.shaderResourceViews[0].GetAddressOf());
		//D3D11_BUFFER_DESC bufferDesc;
		//mesh.indexBuffer->GetDesc(&bufferDesc);
		//deviceContext->DrawIndexed(bufferDesc.ByteWidth / sizeof(uint32_t), 0, 0);
	}
#endif

	for (Mesh& mesh : meshes)
	{
		uint32_t stride{ sizeof(Vertex) };
		uint32_t offset{ 0 };
		deviceContext->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
		deviceContext->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		Constants data;
		if (keyframe && keyframe->nodes.size() > 0)
		{
			const Animation::Keyframe::Node& mesh_node{ keyframe->nodes.at(mesh.nodeIndex) };
			XMStoreFloat4x4(&data.world, XMLoadFloat4x4(&mesh_node.globalTransform) * XMLoadFloat4x4(&world));

			const size_t bone_count{ mesh.bindPose.bones.size() };
			_ASSERT_EXPR(bone_count < MAX_BONES, L"The value of the 'bone_count' has exceeded MAX_BONES.");

			for (size_t bone_index = 0; bone_index < bone_count; ++bone_index)
			{
				const Skeleton::Bone& bone{ mesh.bindPose.bones.at(bone_index) };
				const Animation::Keyframe::Node& boneNode{ keyframe->nodes.at(bone.nodeIndex) };
				XMStoreFloat4x4(&data.boneTransforms[bone_index],
					XMLoadFloat4x4(&bone.offsetTransform) *
					XMLoadFloat4x4(&boneNode.globalTransform) *
					XMMatrixInverse(nullptr, XMLoadFloat4x4(&mesh.defaultGlobalTransform))
				);
			}
		}
		else
		{
			XMStoreFloat4x4(&data.world,
				XMLoadFloat4x4(&mesh.defaultGlobalTransform) * XMLoadFloat4x4(&world));
			for (size_t bone_index = 0; bone_index < MAX_BONES; ++bone_index)
			{
				data.boneTransforms[bone_index] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
			}
		}
		for (const Mesh::Subset& subset : mesh.subsets)
		{
			const Material& material{ materials.at(subset.materialUniqueId) };
			XMStoreFloat4(&data.materialColor, XMLoadFloat4(&materialColor) * XMLoadFloat4(&material.diffuse));
			deviceContext->UpdateSubresource(constantBuffer.Get(), 0, 0, &data, 0, 0);
			deviceContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

			deviceContext->PSSetShaderResources(0, 1, material.shaderResourceViews[0].GetAddressOf());
			deviceContext->PSSetShaderResources(1, 1, material.shaderResourceViews[1].GetAddressOf());

			deviceContext->DrawIndexed(subset.indexCount, subset.startIndexLocation, 0);
		}
	}

}

void SkinnedMesh::Render(ID3D11DeviceContext* deviceContext, const DirectX::XMFLOAT4X4& world, const Vector4& materialColor, const Animation::Keyframe* keyframe) {
	using namespace DirectX;

#if 0
	for (const Mesh& mesh : meshes) {
		uint32_t stride = sizeof(Vertex);
		uint32_t offset = 0;
		deviceContext->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
		deviceContext->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		deviceContext->IASetInputLayout(inputLayout.Get());

		deviceContext->VSSetShader(vertexShader.Get(), nullptr, 0);
		deviceContext->PSSetShader(pixelShader.Get(), nullptr, 0);

		Constants data;
		//data.world = world;
		//XMStoreFloat4x4(&data.world, XMLoadFloat4x4(&mesh.defaultGlobalTransform) * XMLoadFloat4x4(&world));
		const Animation::Keyframe::Node& meshNode = keyframe->nodes.at(mesh.nodeIndex);
		XMStoreFloat4x4(&data.world, XMLoadFloat4x4(&meshNode.globalTransform) * XMLoadFloat4x4(&world));


#if 0// UNIT23
		XMStoreFloat4x4(&data.boneTransforms[0], XMMatrixIdentity());
		XMStoreFloat4x4(&data.boneTransforms[1], XMMatrixRotationRollPitchYaw(0, 0, XMConvertToRadians(+45)));
		XMStoreFloat4x4(&data.boneTransforms[2], XMMatrixRotationRollPitchYaw(0, 0, XMConvertToRadians(-45)));
#endif
#if 0//UNIT24
		// Bind pose transform(Offest matrix) : Convert from the model(mesh) space to the bone space
		XMMATRIX B[3];
		B[0] = XMLoadFloat4x4(&mesh.bindPose.bones.at(0).offsetTransform);
		B[1] = XMLoadFloat4x4(&mesh.bindPose.bones.at(1).offsetTransform);
		B[2] = XMLoadFloat4x4(&mesh.bindPose.bones.at(2).offsetTransform);

		// Animation bone transform : Convert from the bone space to the model(mesh) or the parent bone space
		XMMATRIX A[3];
		// from A0 space to model space
		A[0] = XMMatrixRotationRollPitchYaw(XMConvertToRadians(90), 0, 0);

		// from A1 space to parent bone(A0) space
		A[1] = XMMatrixRotationRollPitchYaw(0, 0, XMConvertToRadians(45)) * XMMatrixTranslation(0, 2, 0);

		// from A2 space to parent bone(A1) space
		A[2] = XMMatrixRotationRollPitchYaw(0, 0, XMConvertToRadians(-45)) * XMMatrixTranslation(0, 2, 0);

		XMStoreFloat4x4(&data.boneTransforms[0], B[0] * A[0]);
		XMStoreFloat4x4(&data.boneTransforms[1], B[1] * A[1] * A[0]);
		XMStoreFloat4x4(&data.boneTransforms[2], B[1] * A[2] * A[1] * A[0]);
#endif

		const size_t boneCount = mesh.bindPose.bones.size();
		for (int boneIndex = 0; boneIndex < boneCount; ++boneIndex) {

			const Skeleton::Bone& bone = mesh.bindPose.bones.at(boneIndex);
			const Animation::Keyframe::Node& boneNode = keyframe->nodes.at(bone.nodeIndex);
			XMStoreFloat4x4(
				&data.boneTransforms[boneIndex],
				XMLoadFloat4x4(&bone.offsetTransform) *
				XMLoadFloat4x4(&boneNode.globalTransform) *
				XMMatrixInverse(nullptr, XMLoadFloat4x4(&mesh.defaultGlobalTransform))
			);

		}




		for (const Mesh::Subset& subset : mesh.subsets) {
			const Material& material = materials.at(subset.materialUniqueId);

			XMStoreFloat4(&data.materialColor, XMLoadFloat4(&materialColor) * XMLoadFloat4(&material.Kd));
			deviceContext->UpdateSubresource(constantBuffer.Get(), 0, 0, &data, 0, 0);
			deviceContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

			deviceContext->PSSetShaderResources(0, 1, material.shaderResourceViews[0].GetAddressOf());

			deviceContext->DrawIndexed(subset.indexCount, subset.startIndexLocation, 0);
		}

		////data.materialColor = materialColor;
		//XMStoreFloat4(&data.materialColor, XMLoadFloat4(&materialColor) * XMLoadFloat4(&materials.cbegin()->second.Kd));
		//deviceContext->UpdateSubresource(constantBuffer.Get(), 0, 0, &data, 0, 0);
		//deviceContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
		//deviceContext->PSSetShaderResources(0, 1, materials.cbegin()->second.shaderResourceViews[0].GetAddressOf());
		//D3D11_BUFFER_DESC bufferDesc;
		//mesh.indexBuffer->GetDesc(&bufferDesc);
		//deviceContext->DrawIndexed(bufferDesc.ByteWidth / sizeof(uint32_t), 0, 0);
	}
#endif

	for (Mesh& mesh : meshes)
	{
		uint32_t stride{ sizeof(Vertex) };
		uint32_t offset{ 0 };
		deviceContext->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
		deviceContext->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		deviceContext->IASetInputLayout(inputLayout.Get());
		deviceContext->VSSetShader(vertexShader.Get(), nullptr, 0);
		deviceContext->PSSetShader(pixelShader.Get(), nullptr, 0);

		Constants data;
		if (keyframe && keyframe->nodes.size() > 0)
		{
			const Animation::Keyframe::Node& mesh_node{ keyframe->nodes.at(mesh.nodeIndex) };
			XMStoreFloat4x4(&data.world, XMLoadFloat4x4(&mesh_node.globalTransform) * XMLoadFloat4x4(&world));

			const size_t bone_count{ mesh.bindPose.bones.size() };
			_ASSERT_EXPR(bone_count < MAX_BONES, L"The value of the 'bone_count' has exceeded MAX_BONES.");

			for (size_t bone_index = 0; bone_index < bone_count; ++bone_index)
			{
				const Skeleton::Bone& bone{ mesh.bindPose.bones.at(bone_index) };
				const Animation::Keyframe::Node& bone_node{ keyframe->nodes.at(bone.nodeIndex) };
				XMStoreFloat4x4(&data.boneTransforms[bone_index],
					XMLoadFloat4x4(&bone.offsetTransform) *
					XMLoadFloat4x4(&bone_node.globalTransform) *
					XMMatrixInverse(nullptr, XMLoadFloat4x4(&mesh.defaultGlobalTransform))
				);
			}
		}
		else
		{
			XMStoreFloat4x4(&data.world,
				XMLoadFloat4x4(&mesh.defaultGlobalTransform) * XMLoadFloat4x4(&world));
			for (size_t bone_index = 0; bone_index < MAX_BONES; ++bone_index)
			{
				data.boneTransforms[bone_index] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
			}
		}
		int a = 0;
		for (const Mesh::Subset& subset : mesh.subsets)
		{
			const Material& material{ materials.at(subset.materialUniqueId) };
			XMStoreFloat4(&data.materialColor, XMLoadFloat4(&materialColor) * XMLoadFloat4(&material.diffuse));
			data.ambient = material.ambient;
			data.diffuse = material.diffuse;
			data.speculer = material.speculer;
			deviceContext->UpdateSubresource(constantBuffer.Get(), 0, 0, &data, 0, 0);
			deviceContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

			deviceContext->PSSetShaderResources(0, 1, material.shaderResourceViews[0].GetAddressOf());
			deviceContext->PSSetShaderResources(1, 1, material.shaderResourceViews[1].GetAddressOf());

			deviceContext->DrawIndexed(subset.indexCount, subset.startIndexLocation, 0);
		}
	}

}

void SkinnedMesh::UpdateAnimation(Animation::Keyframe& keyframe)
{
	size_t nodeCount = keyframe.nodes.size();
	for (size_t nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex) {
		Animation::Keyframe::Node& node = keyframe.nodes.at(nodeIndex);
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(node.scaling.x, node.scaling.y, node.scaling.z);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(XMLoadFloat4(&node.rotation));
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(node.translation.x, node.translation.y, node.translation.z);

		int64_t parentIndex = sceneView.nodes.at(nodeIndex).parentIndex;
		DirectX::XMMATRIX P = parentIndex < 0 ? DirectX::XMMatrixIdentity() :
			DirectX::XMLoadFloat4x4(&keyframe.nodes.at(parentIndex).globalTransform);

		DirectX::XMStoreFloat4x4(&node.globalTransform, S * R * T * P);
	}
}

//#include "misc.h"
//#include "SkinnedMesh.h"
//#include <sstream>
//using namespace DirectX;
//#include "shader.h"
//#include "ResourceManager.h"
//#include <filesystem>
//#include "texture.h"
//#include "Light.h"
//#include "../GameSources/CameraSystem.h"
//#include <fstream>
//
//inline XMFLOAT4X4 to_xmfloat4x4(const FbxAMatrix& fbxamatrix)
//{
//	XMFLOAT4X4 xmfloat4x4;
//	for (int row = 0; row < 4; row++)
//	{
//		for (int column = 0; column < 4; column++)
//		{
//			xmfloat4x4.m[row][column] = static_cast<float>(fbxamatrix[row][column]);
//		}
//	}
//	return xmfloat4x4;
//}
//inline XMFLOAT3 to_xmfloat3(const FbxDouble3& fbxdouble3)
//{
//	XMFLOAT3 xmfloat3;
//	xmfloat3.x = static_cast<float>(fbxdouble3[0]);
//	xmfloat3.y = static_cast<float>(fbxdouble3[1]);
//	xmfloat3.z = static_cast<float>(fbxdouble3[2]);
//	return xmfloat3;
//}
//inline XMFLOAT4 to_xmfloat4(const FbxDouble4& fbxdouble4)
//{
//	XMFLOAT4 xmfloat4;
//	xmfloat4.x = static_cast<float>(fbxdouble4[0]);
//	xmfloat4.y = static_cast<float>(fbxdouble4[1]);
//	xmfloat4.z = static_cast<float>(fbxdouble4[2]);
//	xmfloat4.w = static_cast<float>(fbxdouble4[3]);
//	return xmfloat4;
//}
//
//struct BoneInfluence
//{
//	uint32_t boneIndex;
//	float boneWeight;
//};
//using BoneInfluencesPerControlPoint = std::vector<BoneInfluence>;
//void FetchBoneInfluences(const FbxMesh* fbxMesh, std::vector<BoneInfluencesPerControlPoint>& boneInfluences)
//{
//	const int controlPointsCount{ fbxMesh->GetControlPointsCount() };
//	boneInfluences.resize(controlPointsCount);
//
//	const int skinCount{ fbxMesh->GetDeformerCount(FbxDeformer::eSkin) };
//	for (int skinIndex = 0; skinIndex < skinCount; ++skinIndex)
//	{
//		const FbxSkin* fbxSkin{ static_cast<FbxSkin*>(fbxMesh->GetDeformer(skinIndex, FbxDeformer::eSkin)) };
//
//		const int clusterCount{ fbxSkin->GetClusterCount() };
//		for (int clusterIndex = 0; clusterIndex < clusterCount; ++clusterIndex)
//		{
//			const FbxCluster* fbx_cluster{ fbxSkin->GetCluster(clusterIndex) };
//
//			const int controlPointIndicesCount{ fbx_cluster->GetControlPointIndicesCount() };
//			for (int controlPointIndicesIndex = 0; controlPointIndicesIndex < controlPointIndicesCount; ++controlPointIndicesIndex)
//			{
//#if 1
//				int controlPointIndex{ fbx_cluster->GetControlPointIndices()[controlPointIndicesIndex] };
//				double controlPointWeight{ fbx_cluster->GetControlPointWeights()[controlPointIndicesIndex] };
//				BoneInfluence& boneInfluence{ boneInfluences.at(controlPointIndex).emplace_back() };
//				boneInfluence.boneIndex = static_cast<uint32_t>(clusterIndex);
//				boneInfluence.boneWeight = static_cast<float>(controlPointWeight);
//#else
//				bone_influences.at((fbx_cluster->GetControlPointIndices())[control_point_indices_index]).emplace_back()
//					= { static_cast<uint32_t>(cluster_index),  static_cast<float>((fbx_cluster->GetControlPointWeights())[control_point_indices_index]) };
//#endif
//			}
//		}
//	}
//}
//
//void SkinnedMesh::FetchScene(const char* fbxFilename, bool triangulate, float samplingRate)
//{
//	FbxManager* fbxManager{ FbxManager::Create() };
//	FbxScene* fbxScene{ FbxScene::Create(fbxManager, "") };
//	FbxImporter* fbxImporter{ FbxImporter::Create(fbxManager, "") };
//	bool importStatus{ false };
//	importStatus = fbxImporter->Initialize(fbxFilename);
//	_ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());
//	importStatus = fbxImporter->Import(fbxScene);
//	_ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());
//
//	FbxGeometryConverter fbx_converter(fbxManager);
//	if (triangulate)
//	{
//		fbx_converter.Triangulate(fbxScene, true/*replace*/, false/*legacy*/);
//		fbx_converter.RemoveBadPolygonsFromMeshes(fbxScene);
//	}
//
//	// Serialize an entire scene graph into sequence container
//	std::function<void(FbxNode*)> traverse{ [&](FbxNode* fbxNode) {
//#if 0
//		if (fbx_node->GetNodeAttribute())
//		{
//			switch (fbx_node->GetNodeAttribute()->GetAttributeType())
//			{
//			case FbxNodeAttribute::EType::eNull:
//			case FbxNodeAttribute::EType::eMesh:
//			case FbxNodeAttribute::EType::eSkeleton:
//			case FbxNodeAttribute::EType::eUnknown:
//			case FbxNodeAttribute::EType::eMarker:
//			case FbxNodeAttribute::EType::eCamera:
//			case FbxNodeAttribute::EType::eLight:
//				scene::node& node{ scene_view.nodes.emplace_back() };
//				node.attribute = fbx_node->GetNodeAttribute()->GetAttributeType();
//				node.name = fbx_node->GetName();
//				node.unique_id = fbx_node->GetUniqueID();
//				node.parent_index = scene_view.indexof(fbx_node->GetParent() ? fbx_node->GetParent()->GetUniqueID() : 0);
//				break;
//			}
//		}
//#else
//		Scene::Node& node{ sceneView.nodes.emplace_back() };
//		node.attribute = fbxNode->GetNodeAttribute() ? fbxNode->GetNodeAttribute()->GetAttributeType() : FbxNodeAttribute::EType::eUnknown;
//		node.name = fbxNode->GetName();
//		node.uniqueId = fbxNode->GetUniqueID();
//		node.parentIndex = sceneView.indexof(fbxNode->GetParent() ? fbxNode->GetParent()->GetUniqueID() : 0);
//#endif
//		for (int child_index = 0; child_index < fbxNode->GetChildCount(); ++child_index)
//		{
//			traverse(fbxNode->GetChild(child_index));
//		}
//	} };
//	traverse(fbxScene->GetRootNode());
//
//	FetchMeshes(fbxScene, meshes);
//	FetchMaterials(fbxScene, materials);
//	FetchAnimations(fbxScene, animationClips, samplingRate);
//
//	fbxManager->Destroy();
//}
//
//SkinnedMesh::SkinnedMesh(ID3D11Device* device, const char* fbxFilename, bool triangulate, float samplingRate,
//	std::function<void(Mesh&, Mesh::Subset& subset)> draworderCallback, bool avoidCreateComObjects)
//	//: GeometricMesh(device, fbxFilename, triangulate, samplingRate, draworderCallback, avoidCreateComObjects)
//{
//	std::filesystem::path cerealFilename(fbxFilename);
//	cerealFilename.replace_extension("cereal");
//	if (std::filesystem::exists(cerealFilename.c_str())) {
//		std::ifstream ifs(cerealFilename.c_str(), std::ios::binary);
//		cereal::BinaryInputArchive deserialization(ifs);
//		deserialization(sceneView, meshes, materials, animationClips);
//	}
//	else {
//		FetchScene(fbxFilename, triangulate, samplingRate);
//		std::ofstream ofs(cerealFilename.c_str(), std::ios::binary);
//		cereal::BinaryOutputArchive serialization(ofs);
//		serialization(sceneView, meshes, materials, animationClips);
//	}
//
//	if (!avoidCreateComObjects) CreateComObjects(device, fbxFilename);
//
//	for (Mesh& mesh : meshes)
//	{
//		for (Mesh::Subset& subset : mesh.subsets)
//		{
//			draworderCallback(mesh, subset);
//		}
//		std::sort(mesh.subsets.begin(), mesh.subsets.end());
//	}
//	std::sort(meshes.begin(), meshes.end());
//}
//
//SkinnedMesh::SkinnedMesh(ID3D11Device* device, const char* fbxFilename, std::vector<std::string>& animationFilenames, bool triangulate, float samplingRate)
//	//: GeometricMesh(device, fbxFilename, animationFilenames, triangulate, samplingRate)
//{
//	std::filesystem::path cerealFilename(fbxFilename);
//	cerealFilename.replace_extension("cereal");
//	if (std::filesystem::exists(cerealFilename.c_str())) {
//		std::ifstream ifs(cerealFilename.c_str(), std::ios::binary);
//		cereal::BinaryInputArchive deserialization(ifs);
//		deserialization(sceneView, meshes, materials, animationClips);
//	}
//	else {
//		FetchScene(fbxFilename, triangulate, samplingRate);
//
//		for (const std::string animationFilename : animationFilenames) AppendAnimations(animationFilename.c_str(), samplingRate);
//		
//		std::ofstream ofs(cerealFilename.c_str(), std::ios::binary);
//		cereal::BinaryOutputArchive serialization(ofs);
//		serialization(sceneView, meshes, materials, animationClips);
//	}
//	CreateComObjects(device, fbxFilename);
//}
//
//void SkinnedMesh::FetchMeshes(FbxScene* fbxScene, std::vector<Mesh>& meshes)
//{
//	// シーンからすべてのメッシュをフェッチします
//	for (const Scene::Node& node : sceneView.nodes)
//	{
//		// ノード属性がeMeshでない場合はスキップします
//		if (node.attribute != FbxNodeAttribute::EType::eMesh) continue;
//
//		FbxNode* fbxNode{ fbxScene->FindNodeByName(node.name.c_str()) };
//		FbxMesh* fbxMesh{ fbxNode->GetMesh() };
//
//		Mesh& mesh{ meshes.emplace_back() };
//		mesh.uniqueId = fbxMesh->GetNode()->GetUniqueID();
//		mesh.name = fbxMesh->GetNode()->GetName();
//		mesh.nodeIndex = sceneView.indexof(mesh.uniqueId);
//		mesh.defaultGlobalTransform = to_xmfloat4x4(fbxMesh->GetNode()->EvaluateGlobalTransform());
//		// UNIT.99
//		mesh.geometricTransform = to_xmfloat4x4(FbxAMatrix(
//			fbxMesh->GetNode()->GetGeometricTranslation(FbxNode::eSourcePivot),
//			fbxMesh->GetNode()->GetGeometricRotation(FbxNode::eSourcePivot),
//			fbxMesh->GetNode()->GetGeometricScaling(FbxNode::eSourcePivot)));
//
//		std::vector<BoneInfluencesPerControlPoint> boneInfluences;
//		FetchBoneInfluences(fbxMesh, boneInfluences);
//		FetchSkeleton(fbxMesh, mesh.bindPose);
//
//		// 各材料のサブセットを作成する
//		std::vector<Mesh::Subset>& subsets{ mesh.subsets };
//		const int materialCount{ fbxMesh->GetNode()->GetMaterialCount() };
//		subsets.resize(materialCount > 0 ? materialCount : 1);
//		for (int materialIndex = 0; materialIndex < materialCount; ++materialIndex) {
//			const FbxSurfaceMaterial* fbxMaterial{ fbxMesh->GetNode()->GetMaterial(materialIndex) };
//			subsets.at(materialIndex).materialName = fbxMaterial->GetName();
//			subsets.at(materialIndex).materialUniqueId = fbxMaterial->GetUniqueID();
//		}
//		if (materialCount > 0) {
//			// 各素材のマテリアルを数える
//			const int polygonCount{ fbxMesh->GetPolygonCount() };
//			for (int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex) {
//				const int materialIndex{ fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(polygonIndex) };
//				subsets.at(materialIndex).indexCount += 3;
//			}
//
//			// Record the offset (How many vertex) オフセットを記録する
//			uint32_t offset{ 0 };
//			for (Mesh::Subset& subset : subsets) {
//				subset.startIndexLocation = offset;
//				offset += subset.indexCount;
//				// This will be used as counter in the following procedures, reset to zero これは、次の手順でカウンターとして使用され、ゼロにリセットされる
//				subset.indexCount = 0;
//			}
//		}
//
//		const int polygonCount{ fbxMesh->GetPolygonCount() };
//		// UNIT.99
//		mesh.vertexPositions.resize(polygonCount * 3LL);
//		mesh.vertexExtraAttributes.resize(polygonCount * 3LL);
//		mesh.vertexBoneInfluences.resize(polygonCount * 3LL);
//		mesh.indices.resize(polygonCount * 3LL);
//
//		FbxStringList uvNames;
//		fbxMesh->GetUVSetNames(uvNames);
//		const FbxVector4* controlPoints{ fbxMesh->GetControlPoints() };
//		for (int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex)
//		{
//			const int materialIndex{ materialCount > 0 ? fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(polygonIndex) : 0 };
//			Mesh::Subset& subset{ subsets.at(materialIndex) };
//			const uint32_t offset{ subset.startIndexLocation + subset.indexCount };
//
//			for (int positionInPolygon = 0; positionInPolygon < 3; ++positionInPolygon)
//			{
//				const int vertexIndex{ polygonIndex * 3 + positionInPolygon };
//
//				VertexPosition vertexPosition;
//				VertexExtraAttribute vertexExtraAttribute;
//				VertexBoneInfluence vertexBoneInfluence;
//				const int polygonVertex{ fbxMesh->GetPolygonVertex(polygonIndex, positionInPolygon) };
//				vertexPosition.position.x = static_cast<float>(controlPoints[polygonVertex][0]);
//				vertexPosition.position.y = static_cast<float>(controlPoints[polygonVertex][1]);
//				vertexPosition.position.z = static_cast<float>(controlPoints[polygonVertex][2]);
//
//				const BoneInfluencesPerControlPoint& influencesPerControlPoint{ boneInfluences.at(polygonVertex) };
//				for (size_t influenceIndex = 0; influenceIndex < influencesPerControlPoint.size(); ++influenceIndex)
//				{
//					if (influenceIndex < MAX_BONE_INFLUENCES) {
//						vertexBoneInfluence.boneWeights[influenceIndex] = influencesPerControlPoint.at(influenceIndex).boneWeight;
//						vertexBoneInfluence.boneIndices[influenceIndex] = influencesPerControlPoint.at(influenceIndex).boneIndex;
//					}
//#if 1
//					else
//					{
//						size_t minimum_value_index = 0;
//						float minimum_value = FLT_MAX;
//						for (size_t i = 0; i < MAX_BONE_INFLUENCES; ++i) {
//							if (minimum_value > vertexBoneInfluence.boneWeights[i]) {
//								minimum_value = vertexBoneInfluence.boneWeights[i];
//								minimum_value_index = i;
//							}
//						}
//						vertexBoneInfluence.boneWeights[minimum_value_index] += influencesPerControlPoint.at(influenceIndex).boneWeight;
//						vertexBoneInfluence.boneIndices[minimum_value_index] = influencesPerControlPoint.at(influenceIndex).boneIndex;
//					}
//#endif
//				}
//				// UNIT.29
//				//if (fbx_mesh->GetElementNormalCount() > 0)
//				if (fbxMesh->GenerateNormals(false))
//				{
//					FbxVector4 normal;
//					fbxMesh->GetPolygonVertexNormal(polygonIndex, positionInPolygon, normal);
//					vertexExtraAttribute.normal.x = static_cast<float>(normal[0]);
//					vertexExtraAttribute.normal.y = static_cast<float>(normal[1]);
//					vertexExtraAttribute.normal.z = static_cast<float>(normal[2]);
//				}
//				if (fbxMesh->GetElementUVCount() > 0)
//				{
//					FbxVector2 uv;
//					bool unmapped_uv;
//					fbxMesh->GetPolygonVertexUV(polygonIndex, positionInPolygon, uvNames[0], uv, unmapped_uv);
//					vertexExtraAttribute.texcoord.x = static_cast<float>(uv[0]);
//					vertexExtraAttribute.texcoord.y = 1.0f - static_cast<float>(uv[1]);
//				}
//				
//				if (fbxMesh->GenerateTangentsData(0, false))
//				{
//					const FbxGeometryElementTangent* tangent = fbxMesh->GetElementTangent(0);
//					_ASSERT_EXPR(tangent->GetMappingMode() == FbxGeometryElement::EMappingMode::eByPolygonVertex &&
//						tangent->GetReferenceMode() == FbxGeometryElement::EReferenceMode::eDirect,
//						L"Only supports a combination of these modes.");
//
//					vertexExtraAttribute.tangent.x = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[0]);
//					vertexExtraAttribute.tangent.y = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[1]);
//					vertexExtraAttribute.tangent.z = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[2]);
//					vertexExtraAttribute.tangent.w = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[3]);
//				}
//				mesh.vertexExtraAttributes.at(vertexIndex) = std::move(vertexExtraAttribute);
//				mesh.vertexBoneInfluences.at(vertexIndex) = std::move(vertexBoneInfluence);
//				mesh.vertexPositions.at(vertexIndex) = std::move(vertexPosition);
//
//				// UNIT.20
//#if 0
//				mesh.indices.at(vertex_index) = vertex_index;
//#else
//				mesh.indices.at(static_cast<size_t>(offset) + positionInPolygon) = vertexIndex;
//				subset.indexCount++;
//#endif
//			}
//		}
//		// UNIT.29
//		// bounding_box [0] is the min vertex and [1] is the max
//		mesh.boundingBox[0] = { +D3D11_FLOAT32_MAX, +D3D11_FLOAT32_MAX, +D3D11_FLOAT32_MAX };
//		mesh.boundingBox[1] = { -D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX };
//		for (const VertexPosition& v : mesh.vertexPositions)
//		{
//			mesh.boundingBox[0].x = std::min<float>(mesh.boundingBox[0].x, v.position.x);
//			mesh.boundingBox[0].y = std::min<float>(mesh.boundingBox[0].y, v.position.y);
//			mesh.boundingBox[0].z = std::min<float>(mesh.boundingBox[0].z, v.position.z);
//			mesh.boundingBox[1].x = std::max<float>(mesh.boundingBox[1].x, v.position.x);
//			mesh.boundingBox[1].y = std::max<float>(mesh.boundingBox[1].y, v.position.y);
//			mesh.boundingBox[1].z = std::max<float>(mesh.boundingBox[1].z, v.position.z);
//		}
//	}
//}
//
//void SkinnedMesh::CreateComObjects(ID3D11Device* device, const char* fbxFilename)
//{
//	for (Mesh& mesh : meshes) {
//
//		mesh.attribute = mesh.bindPose.bones.size() > 0 ? geometric_attribute::skinnned_mesh : geometric_attribute::static_mesh;
//
//		HRESULT hr{ S_OK };
//		D3D11_BUFFER_DESC bufferDesc{};
//		D3D11_SUBRESOURCE_DATA subresourceData{};
//		bufferDesc.ByteWidth = static_cast<UINT>(sizeof(VertexPosition) * mesh.vertexPositions.size());
//		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
//		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//		bufferDesc.CPUAccessFlags = 0;
//		bufferDesc.MiscFlags = 0;
//		bufferDesc.StructureByteStride = 0;
//		subresourceData.pSysMem = mesh.vertexPositions.data();
//		subresourceData.SysMemPitch = 0;
//		subresourceData.SysMemSlicePitch = 0;
//		hr = device->CreateBuffer(&bufferDesc, &subresourceData, mesh.vertexBuffers[0].ReleaseAndGetAddressOf());
//		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//
//		bufferDesc.ByteWidth = static_cast<UINT>(sizeof(VertexExtraAttribute) * mesh.vertexExtraAttributes.size());
//		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
//		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//		subresourceData.pSysMem = mesh.vertexExtraAttributes.data();
//		hr = device->CreateBuffer(&bufferDesc, &subresourceData, mesh.vertexBuffers[1].ReleaseAndGetAddressOf());
//		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//
//		bufferDesc.ByteWidth = static_cast<UINT>(sizeof(VertexBoneInfluence) * mesh.vertexBoneInfluences.size());
//		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
//		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//		subresourceData.pSysMem = mesh.vertexBoneInfluences.data();
//		hr = device->CreateBuffer(&bufferDesc, &subresourceData, mesh.vertexBuffers[2].ReleaseAndGetAddressOf());
//		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//
//		bufferDesc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * mesh.indices.size());
//		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
//		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
//		subresourceData.pSysMem = mesh.indices.data();
//		hr = device->CreateBuffer(&bufferDesc, &subresourceData, mesh.indexBuffer.ReleaseAndGetAddressOf());
//		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//
//		mesh.vertexPositions.clear();
//		mesh.vertexExtraAttributes.clear();
//		mesh.vertexBoneInfluences.clear();
//		mesh.indices.clear();
//	}
//
//	for (std::unordered_map<uint64_t, Material>::iterator iterator = materials.begin(); iterator != materials.end(); ++iterator) {
//		for (size_t textureIndex = 0; textureIndex < 2; ++textureIndex) {
//			if (iterator->second.textureFilenames[textureIndex].size() > 0) {
//				std::filesystem::path path(fbxFilename);
//				path.replace_filename(iterator->second.textureFilenames[textureIndex]);
//				D3D11_TEXTURE2D_DESC texture2dDesc;
//				iterator->second.shaderResourceViews[textureIndex].Attach(Texture::QueryTexture(device, path.c_str(), &texture2dDesc));
//			}
//			else iterator->second.shaderResourceViews[textureIndex].Attach(Texture::QueryDummyTexture(device, textureIndex == 1 ? 0xFFFF7F7F : 0xFFFFFFFF, 16));
//		}
//	}
//
//	HRESULT hr = S_OK;
//	D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
//	{
//		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1/*UNIT.99*/, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1/*UNIT.99*/, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1/*UNIT.99*/, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 2/*UNIT.99*/, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "BONES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 2/*UNIT.99*/, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//	};
//	// UNIT.99
//	//ResourceManager::CreateVsFromCso(device, "static_mesh_vs.cso", vertexShaders[0].ReleaseAndGetAddressOf(), inputLayouts[0].ReleaseAndGetAddressOf(), inputElementDesc, 4);
//	ResourceManager::CreateVsFromCso(device, "./Data/Shader/SkinnedMeshVS.cso", vertexShaders[1].ReleaseAndGetAddressOf(), inputLayouts[1].ReleaseAndGetAddressOf(), inputElementDesc, 6);
//	//ResourceManager::CreateVsFromCso(device, "static_shadow_cast_vs.cso", vertexShaders[2].ReleaseAndGetAddressOf(), nullptr, nullptr, 0);
//	//ResourceManager::CreateVsFromCso(device, "skinned_shadow_cast_vs.cso", vertexShaders[3].ReleaseAndGetAddressOf(), nullptr, nullptr, 0);
//
//	//ResourceManager::CreatePsFromCso(device, "static_mesh_ps.cso", pixelShaders[0].ReleaseAndGetAddressOf());
//	ResourceManager::CreatePsFromCso(device, "SkinnedMeshPS.cso", pixelShaders[1].ReleaseAndGetAddressOf());
//
//	D3D11_BUFFER_DESC bufferDesc{};
//	bufferDesc.ByteWidth = sizeof(Constants);
//	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
//	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//	hr = device->CreateBuffer(&bufferDesc, nullptr, constantBuffers[0].ReleaseAndGetAddressOf());
//	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//
//	bufferDesc.ByteWidth = sizeof(BoneConstants);
//	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
//	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//	hr = device->CreateBuffer(&bufferDesc, nullptr, constantBuffers[1].ReleaseAndGetAddressOf());
//	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//
//	bufferDesc.ByteWidth = sizeof(MaterialConstants);
//	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
//	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//	hr = device->CreateBuffer(&bufferDesc, nullptr, constantBuffers[2].ReleaseAndGetAddressOf());
//	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//}
//
//
//void SkinnedMesh::Preparation(ID3D11DeviceContext* immediateContext, Shader shader, bool wireframe)
//{
//	// プリミティブモードの設定
//	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//
//	// シェーダーの設定
//	immediateContext->VSSetShader(shader.GetVertexShader().Get(), nullptr, 0);
//	immediateContext->PSSetShader(shader.GetPixelShader().Get(), nullptr, 0);
//
//	// 入力レイアウトの設定
//	immediateContext->IASetInputLayout(shader.GetInputLayout().Get());
//
//	// 深度テストの設定
//	immediateContext->OMSetDepthStencilState(depthStencilState.Get(), 0);
//
//	// サンプラーの設定
//	immediateContext->PSSetSamplers(0, 1, samplerState.GetAddressOf());
//
//	// コンスタントバッファ設定
//	Light::CBufferLight01 cb1;
//	cb1.ambientColor = Light::GetInstance()->ambient;
//	cb1.lightDir = Light::GetInstance()->lightDir;
//	cb1.lightColor = Light::GetInstance()->lightColor;
//	DirectX::XMFLOAT3 _viewPos = CameraSystem::GetInstance()->mainView.GetPos();
//	cb1.eyePos.x = _viewPos.x;
//	cb1.eyePos.y = _viewPos.y;
//	cb1.eyePos.z = _viewPos.z;
//	cb1.eyePos.w = wireframe;
//
//	Light::CBufferLight02 cb2;
//	memcpy(cb2.pointLight, Light::GetInstance()->pointLight, sizeof(Light::POINTLIGHT) * Light::POINTMAX);
//	memcpy(cb2.spotLight, Light::GetInstance()->spotLight, sizeof(Light::SPOTLIGHT) * Light::SPOTMAX);
//
//	immediateContext->UpdateSubresource(Light::GetInstance()->constantBufferLight1.Get(), 0, 0, &cb1, 0, 0);
//	immediateContext->UpdateSubresource(Light::GetInstance()->constantBufferLight2.Get(), 0, 0, &cb2, 0, 0);
//
//	immediateContext->VSSetConstantBuffers(2, 1, Light::GetInstance()->constantBufferLight1.GetAddressOf());
//	immediateContext->VSSetConstantBuffers(3, 1, Light::GetInstance()->constantBufferLight2.GetAddressOf());
//	immediateContext->PSSetConstantBuffers(2, 1, Light::GetInstance()->constantBufferLight1.GetAddressOf());
//	immediateContext->PSSetConstantBuffers(3, 1, Light::GetInstance()->constantBufferLight2.GetAddressOf());
//}
//
//void SkinnedMesh::Render(ID3D11DeviceContext* immediateContext, const XMFLOAT4X4& world, const Animation::Keyframe* keyframe,
//	std::function<int(const Mesh&, const Material&, ShaderResources&, PipelineState&)> callback, bool shadowCasting)
//{
//	Microsoft::WRL::ComPtr<ID3D11Device> device;
//	immediateContext->GetDevice(device.GetAddressOf());
//
//	D3D11_PRIMITIVE_TOPOLOGY cachedTopology;
//	Microsoft::WRL::ComPtr<ID3D11VertexShader> cachedVertexShader;
//	Microsoft::WRL::ComPtr<ID3D11HullShader> cachedHullShader;
//	Microsoft::WRL::ComPtr<ID3D11DomainShader> cachedDomainShader;
//	Microsoft::WRL::ComPtr<ID3D11GeometryShader> cachedGeometryShader;
//	Microsoft::WRL::ComPtr<ID3D11PixelShader> cachedPixelShader;
//	Microsoft::WRL::ComPtr<ID3D11RasterizerState> cachedRasterizerState;
//	Microsoft::WRL::ComPtr<ID3D11BlendState> cachedBlendState;
//	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> cachedDepthStencilState;
//	UINT cachedStencilRef;
//
//	immediateContext->IAGetPrimitiveTopology(&cachedTopology);
//	immediateContext->VSGetShader(cachedVertexShader.ReleaseAndGetAddressOf(), nullptr, nullptr);
//	immediateContext->DSGetShader(cachedDomainShader.ReleaseAndGetAddressOf(), nullptr, nullptr);
//	immediateContext->HSGetShader(cachedHullShader.ReleaseAndGetAddressOf(), nullptr, nullptr);
//	immediateContext->GSGetShader(cachedGeometryShader.ReleaseAndGetAddressOf(), nullptr, nullptr);
//	immediateContext->PSGetShader(cachedPixelShader.ReleaseAndGetAddressOf(), nullptr, nullptr);
//	immediateContext->RSGetState(cachedRasterizerState.ReleaseAndGetAddressOf());
//	immediateContext->OMGetBlendState(cachedBlendState.ReleaseAndGetAddressOf(), nullptr, nullptr);
//	immediateContext->OMGetDepthStencilState(cachedDepthStencilState.ReleaseAndGetAddressOf(), &cachedStencilRef);
//
//	for (Mesh& mesh : meshes) {
//		uint32_t strides[3]{ sizeof(VertexPosition), sizeof(VertexExtraAttribute), sizeof(VertexBoneInfluence) };
//		uint32_t offsets[3]{ 0, 0, 0 };
//		ID3D11Buffer* vertexBuffers[3]
//		{
//			mesh.vertexBuffers[0].Get(),
//			shadowCasting ? nullptr : mesh.vertexBuffers[1].Get(),
//			mesh.attribute == geometric_attribute::skinnned_mesh ? mesh.vertexBuffers[2].Get() : nullptr
//		};
//		immediateContext->IASetVertexBuffers(0, static_cast<size_t>(mesh.attribute) + 2, vertexBuffers, strides, offsets);
//		immediateContext->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
//		immediateContext->IASetInputLayout(inputLayouts[static_cast<size_t>(mesh.attribute)].Get());
//
//		Constants data;
//		if (keyframe && keyframe->nodes.size() > 0) {
//			const Animation::Keyframe::Node& meshNode{ keyframe->nodes.at(mesh.nodeIndex) };
//#if 0
//			XMStoreFloat4x4(&data.world, XMLoadFloat4x4(&mesh_node.global_transform) * XMLoadFloat4x4(&world));
//#else
//			XMStoreFloat4x4(&data.world, XMLoadFloat4x4(&mesh.geometricTransform)/*UNIT.99*/ * XMLoadFloat4x4(&meshNode.globalTransform) * XMLoadFloat4x4(&world));
//#endif
//			const size_t boneCount{ mesh.bindPose.bones.size() };
//			_ASSERT_EXPR(boneCount < MAX_BONES, L"The value of the 'bone_count' has exceeded MAX_BONES.");
//
//			BoneConstants boneData;
//			for (size_t boneIndex = 0; boneIndex < boneCount; ++boneIndex) {
//				const Skeleton::Bone& bone{ mesh.bindPose.bones.at(boneIndex) };
//				const Animation::Keyframe::Node& boneNode{ keyframe->nodes.at(bone.nodeIndex) };
//				XMStoreFloat4x4(&boneData.boneTransforms[boneIndex],
//					XMLoadFloat4x4(&bone.offsetTransform) *
//					XMLoadFloat4x4(&boneNode.globalTransform) *
//					XMMatrixInverse(nullptr, XMLoadFloat4x4(&mesh.defaultGlobalTransform))
//				);
//			}
//			if (mesh.attribute == geometric_attribute::skinnned_mesh) {
//				immediateContext->UpdateSubresource(constantBuffers[1].Get(), 0, 0, &boneData, 0, 0);
//				immediateContext->VSSetConstantBuffers(1, 1, constantBuffers[1].GetAddressOf());
//			}
//		}
//		else {
//#if 0
//			XMStoreFloat4x4(&data.world, XMLoadFloat4x4(&mesh.defaultGlobalTransform) * XMLoadFloat4x4(&world));
//#else
//			XMStoreFloat4x4(&data.world, XMLoadFloat4x4(&mesh.geometricTransform) * XMLoadFloat4x4(&mesh.defaultGlobalTransform) * XMLoadFloat4x4(&world));
//#endif
//
//#if 1
//			if (mesh.attribute == geometric_attribute::skinnned_mesh) // UNIT.99
//			{
//				// ダミーボーントランスフォームをバインド
//				BoneConstants boneData;
//				for (size_t boneIndex = 0; boneIndex < MAX_BONES; ++boneIndex)
//				{
//					boneData.boneTransforms[boneIndex] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
//				}
//				immediateContext->UpdateSubresource(constantBuffers[1].Get(), 0, 0, &boneData, 0, 0);
//				immediateContext->VSSetConstantBuffers(1, 1, constantBuffers[1].GetAddressOf());
//			}
//#endif
//		}
//
//		immediateContext->UpdateSubresource(constantBuffers[0].Get(), 0, 0, &data, 0, 0);
//		immediateContext->VSSetConstantBuffers(0, 1, constantBuffers[0].GetAddressOf());
//
//		for (const Mesh::Subset& subset : mesh.subsets)
//		{
//			const Material& material{ materials.at(subset.materialUniqueId) };
//
//			PipelineState defaultPipelineState;
//			defaultPipelineState.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
//			defaultPipelineState.vertexShader = shadowCasting ? vertexShaders[static_cast<size_t>(mesh.attribute) + 2].Get() : vertexShaders[static_cast<size_t>(mesh.attribute)].Get();
//			defaultPipelineState.hullShader = nullptr;
//			defaultPipelineState.domainShader = nullptr;
//			defaultPipelineState.geometryShader = nullptr;
//			defaultPipelineState.pixelShader = shadowCasting ? nullptr : pixelShaders[static_cast<size_t>(mesh.attribute)].Get();
//			defaultPipelineState.rasterizerState = cachedRasterizerState.Get();
//			defaultPipelineState.blendState = cachedBlendState.Get();
//			defaultPipelineState.depthStencilState = cachedDepthStencilState.Get();
//			defaultPipelineState.stencilRef = cachedStencilRef;
//
//			ShaderResources defaultShaderResources;
//			defaultShaderResources.materialData.ambient = material.ambient;
//			defaultShaderResources.materialData.diffuse = material.diffuse;
//			defaultShaderResources.materialData.specular = material.specular;
//			defaultShaderResources.materialData.reflection = material.reflection;
//			defaultShaderResources.materialData.emissive = material.emissive;
//			defaultShaderResources.shaderResourceViews[0] = material.shaderResourceViews[0].Get();
//			defaultShaderResources.shaderResourceViews[1] = material.shaderResourceViews[1].Get();
//			defaultShaderResources.shaderResourceViews[2] = material.shaderResourceViews[2].Get();
//			defaultShaderResources.shaderResourceViews[3] = material.shaderResourceViews[3].Get();
//
//			if (callback(mesh, material, defaultShaderResources, defaultPipelineState) >= 0) {
//				if (!shadowCasting) {
//					immediateContext->UpdateSubresource(constantBuffers[2].Get(), 0, 0, &defaultShaderResources.materialData, 0, 0);
//					immediateContext->VSSetConstantBuffers(2, 1, constantBuffers[2].GetAddressOf());
//					immediateContext->PSSetConstantBuffers(2, 1, constantBuffers[2].GetAddressOf());
//
//					immediateContext->PSSetShaderResources(0, 1, &defaultShaderResources.shaderResourceViews[0]);
//					immediateContext->PSSetShaderResources(1, 1, &defaultShaderResources.shaderResourceViews[1]);
//				}
//
//				immediateContext->IASetPrimitiveTopology(defaultPipelineState.topology);
//				immediateContext->VSSetShader(defaultPipelineState.vertexShader, nullptr, 0);
//				immediateContext->DSSetShader(defaultPipelineState.domainShader, nullptr, 0);
//				immediateContext->HSSetShader(defaultPipelineState.hullShader, nullptr, 0);
//				immediateContext->GSSetShader(defaultPipelineState.geometryShader, nullptr, 0);
//				immediateContext->PSSetShader(defaultPipelineState.pixelShader, nullptr, 0);
//				immediateContext->RSSetState(defaultPipelineState.rasterizerState);
//				immediateContext->OMSetBlendState(defaultPipelineState.blendState, nullptr, 0xFFFFFFFF);
//				immediateContext->OMSetDepthStencilState(defaultPipelineState.depthStencilState, defaultPipelineState.stencilRef);
//
//				immediateContext->DrawIndexed(subset.indexCount, subset.startIndexLocation, 0);
//			}
//		}
//	}
//
//	immediateContext->IASetPrimitiveTopology(cachedTopology);
//	immediateContext->VSSetShader(cachedVertexShader.Get(), nullptr, 0);
//	immediateContext->DSSetShader(cachedDomainShader.Get(), nullptr, 0);
//	immediateContext->HSSetShader(cachedHullShader.Get(), nullptr, 0);
//	immediateContext->GSSetShader(cachedGeometryShader.Get(), nullptr, 0);
//	immediateContext->PSSetShader(cachedPixelShader.Get(), nullptr, 0);
//	immediateContext->RSSetState(cachedRasterizerState.Get());
//	immediateContext->OMSetBlendState(cachedBlendState.Get(), nullptr, 0xFFFFFFFF);
//	immediateContext->OMSetDepthStencilState(cachedDepthStencilState.Get(), cachedStencilRef);
//}
//
//void SkinnedMesh::Render(ID3D11DeviceContext* deviceContext, const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& materialColor, const Animation::Keyframe* keyframe) {
//	using namespace DirectX;
//
//#if 0
//	for (const Mesh& mesh : meshes) {
//		uint32_t stride = sizeof(Vertex);
//		uint32_t offset = 0;
//		deviceContext->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
//		deviceContext->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
//		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//		deviceContext->IASetInputLayout(inputLayout.Get());
//
//		deviceContext->VSSetShader(vertexShader.Get(), nullptr, 0);
//		deviceContext->PSSetShader(pixelShader.Get(), nullptr, 0);
//
//		Constants data;
//		//data.world = world;
//		//XMStoreFloat4x4(&data.world, XMLoadFloat4x4(&mesh.defaultGlobalTransform) * XMLoadFloat4x4(&world));
//		const Animation::Keyframe::Node& meshNode = keyframe->nodes.at(mesh.nodeIndex);
//		XMStoreFloat4x4(&data.world, XMLoadFloat4x4(&meshNode.globalTransform) * XMLoadFloat4x4(&world));
//
//
//#if 0// UNIT23
//		XMStoreFloat4x4(&data.boneTransforms[0], XMMatrixIdentity());
//		XMStoreFloat4x4(&data.boneTransforms[1], XMMatrixRotationRollPitchYaw(0, 0, XMConvertToRadians(+45)));
//		XMStoreFloat4x4(&data.boneTransforms[2], XMMatrixRotationRollPitchYaw(0, 0, XMConvertToRadians(-45)));
//#endif
//#if 0//UNIT24
//		// Bind pose transform(Offest matrix) : Convert from the model(mesh) space to the bone space
//		XMMATRIX B[3];
//		B[0] = XMLoadFloat4x4(&mesh.bindPose.bones.at(0).offsetTransform);
//		B[1] = XMLoadFloat4x4(&mesh.bindPose.bones.at(1).offsetTransform);
//		B[2] = XMLoadFloat4x4(&mesh.bindPose.bones.at(2).offsetTransform);
//
//		// Animation bone transform : Convert from the bone space to the model(mesh) or the parent bone space
//		XMMATRIX A[3];
//		// from A0 space to model space
//		A[0] = XMMatrixRotationRollPitchYaw(XMConvertToRadians(90), 0, 0);
//
//		// from A1 space to parent bone(A0) space
//		A[1] = XMMatrixRotationRollPitchYaw(0, 0, XMConvertToRadians(45)) * XMMatrixTranslation(0, 2, 0);
//
//		// from A2 space to parent bone(A1) space
//		A[2] = XMMatrixRotationRollPitchYaw(0, 0, XMConvertToRadians(-45)) * XMMatrixTranslation(0, 2, 0);
//
//		XMStoreFloat4x4(&data.boneTransforms[0], B[0] * A[0]);
//		XMStoreFloat4x4(&data.boneTransforms[1], B[1] * A[1] * A[0]);
//		XMStoreFloat4x4(&data.boneTransforms[2], B[1] * A[2] * A[1] * A[0]);
//#endif
//
//		const size_t boneCount = mesh.bindPose.bones.size();
//		for (int boneIndex = 0; boneIndex < boneCount; ++boneIndex) {
//
//			const Skeleton::Bone& bone = mesh.bindPose.bones.at(boneIndex);
//			const Animation::Keyframe::Node& boneNode = keyframe->nodes.at(bone.nodeIndex);
//			XMStoreFloat4x4(
//				&data.boneTransforms[boneIndex],
//				XMLoadFloat4x4(&bone.offsetTransform) *
//				XMLoadFloat4x4(&boneNode.globalTransform) *
//				XMMatrixInverse(nullptr, XMLoadFloat4x4(&mesh.defaultGlobalTransform))
//			);
//
//		}
//
//
//
//
//		for (const Mesh::Subset& subset : mesh.subsets) {
//			const Material& material = materials.at(subset.materialUniqueId);
//
//			XMStoreFloat4(&data.materialColor, XMLoadFloat4(&materialColor) * XMLoadFloat4(&material.Kd));
//			deviceContext->UpdateSubresource(constantBuffer.Get(), 0, 0, &data, 0, 0);
//			deviceContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
//
//			deviceContext->PSSetShaderResources(0, 1, material.shaderResourceViews[0].GetAddressOf());
//
//			deviceContext->DrawIndexed(subset.indexCount, subset.startIndexLocation, 0);
//		}
//
//		////data.materialColor = materialColor;
//		//XMStoreFloat4(&data.materialColor, XMLoadFloat4(&materialColor) * XMLoadFloat4(&materials.cbegin()->second.Kd));
//		//deviceContext->UpdateSubresource(constantBuffer.Get(), 0, 0, &data, 0, 0);
//		//deviceContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
//		//deviceContext->PSSetShaderResources(0, 1, materials.cbegin()->second.shaderResourceViews[0].GetAddressOf());
//		//D3D11_BUFFER_DESC bufferDesc;
//		//mesh.indexBuffer->GetDesc(&bufferDesc);
//		//deviceContext->DrawIndexed(bufferDesc.ByteWidth / sizeof(uint32_t), 0, 0);
//	}
//#endif
//
//	for (Mesh& mesh : meshes)
//	{
//		uint32_t stride{ sizeof(Vertex) };
//		uint32_t offset{ 0 };
//		deviceContext->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
//		deviceContext->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
//		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//		deviceContext->IASetInputLayout(inputLayout.Get());
//		deviceContext->VSSetShader(vertexShader.Get(), nullptr, 0);
//		deviceContext->PSSetShader(pixelShader.Get(), nullptr, 0);
//
//		Constants data;
//		if (keyframe && keyframe->nodes.size() > 0)
//		{
//			const Animation::Keyframe::Node& mesh_node{ keyframe->nodes.at(mesh.nodeIndex) };
//			XMStoreFloat4x4(&data.world, XMLoadFloat4x4(&mesh_node.globalTransform) * XMLoadFloat4x4(&world));
//
//			const size_t bone_count{ mesh.bindPose.bones.size() };
//			_ASSERT_EXPR(bone_count < MAX_BONES, L"The value of the 'bone_count' has exceeded MAX_BONES.");
//
//			for (size_t bone_index = 0; bone_index < bone_count; ++bone_index)
//			{
//				const Skeleton::Bone& bone{ mesh.bindPose.bones.at(bone_index) };
//				const Animation::Keyframe::Node& bone_node{ keyframe->nodes.at(bone.nodeIndex) };
//				XMStoreFloat4x4(&data.boneTransforms[bone_index],
//					XMLoadFloat4x4(&bone.offsetTransform) *
//					XMLoadFloat4x4(&bone_node.globalTransform) *
//					XMMatrixInverse(nullptr, XMLoadFloat4x4(&mesh.defaultGlobalTransform))
//				);
//			}
//		}
//		else
//		{
//			XMStoreFloat4x4(&data.world,
//				XMLoadFloat4x4(&mesh.defaultGlobalTransform) * XMLoadFloat4x4(&world));
//			for (size_t bone_index = 0; bone_index < MAX_BONES; ++bone_index)
//			{
//				data.boneTransforms[bone_index] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
//			}
//		}
//		for (const Mesh::Subset& subset : mesh.subsets)
//		{
//			const Material& material{ materials.at(subset.materialUniqueId) };
//			XMStoreFloat4(&data.materialColor, XMLoadFloat4(&materialColor) * XMLoadFloat4(&material.Kd));
//			deviceContext->UpdateSubresource(constantBuffer.Get(), 0, 0, &data, 0, 0);
//			deviceContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
//
//			deviceContext->PSSetShaderResources(0, 1, material.shaderResourceViews[0].GetAddressOf());
//			deviceContext->PSSetShaderResources(1, 1, material.shaderResourceViews[1].GetAddressOf());
//
//			deviceContext->DrawIndexed(subset.indexCount, subset.startIndexLocation, 0);
//		}
//	}
//
//}
//
//void SkinnedMesh::FetchMaterials(FbxScene* fbxScene, std::unordered_map<uint64_t, Material>& materials)
//{
//	const size_t node_count{ sceneView.nodes.size() };
//	for (size_t node_index = 0; node_index < node_count; ++node_index)
//	{
//		const Scene::Node& node{ sceneView.nodes.at(node_index) };
//		const FbxNode* fbxNode{ fbxScene->FindNodeByName(node.name.c_str()) };
//
//		const int materialCount{ fbxNode->GetMaterialCount() };
//		for (int materialIndex = 0; materialIndex < materialCount; ++materialIndex)
//		{
//			const FbxSurfaceMaterial* fbxMaterial{ fbxNode->GetMaterial(materialIndex) };
//
//			Material material;
//			material.name = fbxMaterial->GetName();
//			material.uniqueId = fbxMaterial->GetUniqueID();
//			FbxProperty fbxProperty;
//			fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
//
//			if (fbxProperty.IsValid())
//			{
//				const float factor{ fbxMaterial->FindProperty(FbxSurfaceMaterial::sDiffuseFactor).Get<FbxFloat>() };
//				const FbxDouble3 color{ fbxProperty.Get<FbxDouble3>() };
//				material.diffuse.x = static_cast<float>(color[0]) * factor;
//				material.diffuse.y = static_cast<float>(color[1]) * factor;
//				material.diffuse.z = static_cast<float>(color[2]) * factor;
//				material.diffuse.w = 1.0f;
//
//				const FbxFileTexture* fbx_texture{ fbxProperty.GetSrcObject<FbxFileTexture>() };
//				material.textureFilenames[0] = fbx_texture ? fbx_texture->GetRelativeFileName() : "";
//			}
//			fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sAmbient);
//			if (fbxProperty.IsValid())
//			{
//				const float factor{ fbxMaterial->FindProperty(FbxSurfaceMaterial::sAmbientFactor).Get<FbxFloat>() };
//				const FbxDouble3 color{ fbxProperty.Get<FbxDouble3>() };
//				material.ambient.x = static_cast<float>(color[0]) * factor;
//				material.ambient.y = static_cast<float>(color[1]) * factor;
//				material.ambient.z = static_cast<float>(color[2]) * factor;
//				material.ambient.w = 1.0f;
//			}
//			fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sSpecular);
//			if (fbxProperty.IsValid())
//			{
//				const float factor{ fbxMaterial->FindProperty(FbxSurfaceMaterial::sSpecularFactor).Get<FbxFloat>() };
//				const FbxDouble3 color{ fbxProperty.Get<FbxDouble3>() };
//				material.specular.x = static_cast<float>(color[0]) * factor;
//				material.specular.y = static_cast<float>(color[1]) * factor;
//				material.specular.z = static_cast<float>(color[2]) * factor;
//				material.specular.w = fbxMaterial->FindProperty(FbxSurfaceMaterial::sShininess).Get<FbxFloat>();
//			}
//			fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sNormalMap);
//			if (fbxProperty.IsValid())
//			{
//				const FbxFileTexture* fbxTexture{ fbxProperty.GetSrcObject<FbxFileTexture>() };
//				material.textureFilenames[1] = fbxTexture ? fbxTexture->GetRelativeFileName() : "";
//			}
//			fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sReflection);
//			if (fbxProperty.IsValid())
//			{
//				float factor = fbxMaterial->FindProperty(FbxSurfaceMaterial::sReflectionFactor).Get<FbxFloat>();
//				const FbxDouble3 color{ fbxProperty.Get<FbxDouble3>() };
//				material.reflection.x = static_cast<float>(color[0]) * factor;
//				material.reflection.y = static_cast<float>(color[1]) * factor;
//				material.reflection.z = static_cast<float>(color[2]) * factor;
//				material.reflection.w = 1.0f;
//			}
//			fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sEmissive);
//			if (fbxProperty.IsValid())
//			{
//				float factor = fbxMaterial->FindProperty(FbxSurfaceMaterial::sEmissiveFactor).Get<FbxFloat>();
//				const FbxDouble3 color{ fbxProperty.Get<FbxDouble3>() };
//				material.emissive.x = static_cast<float>(color[0]) * factor;
//				material.emissive.y = static_cast<float>(color[1]) * factor;
//				material.emissive.z = static_cast<float>(color[2]) * factor;
//				material.emissive.w = 1.0f;
//			}
//
//			materials.emplace(material.uniqueId, std::move(material));
//		}
//	}
//#if 1
//	// Append default(dummy) material
//	materials.emplace();
//#endif
//}
//
//void SkinnedMesh::FetchSkeleton(FbxMesh* fbxMesh, Skeleton& bindPose)
//{
//	const int deformerCount = fbxMesh->GetDeformerCount(FbxDeformer::eSkin);
//	for (int deformerIndex = 0; deformerIndex < deformerCount; ++deformerIndex)
//	{
//		FbxSkin* skin = static_cast<FbxSkin*>(fbxMesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));
//		const int clusterCount = skin->GetClusterCount();
//		bindPose.bones.resize(clusterCount);
//		for (int clusterIndex = 0; clusterIndex < clusterCount; ++clusterIndex)
//		{
//			FbxCluster* cluster = skin->GetCluster(clusterIndex);
//
//			Skeleton::Bone& bone{ bindPose.bones.at(clusterIndex) };
//			bone.name = cluster->GetLink()->GetName();
//			bone.uniqueId = cluster->GetLink()->GetUniqueID();
//			bone.parentIndex = bindPose.Indexof(cluster->GetLink()->GetParent()->GetUniqueID());
//			bone.nodeIndex = sceneView.indexof(bone.uniqueId);
//
//			//'reference_global_init_position' is used to convert from local space of model(mesh) to global space of scene.
//			FbxAMatrix referenceGlobalInitPosition;
//			cluster->GetTransformMatrix(referenceGlobalInitPosition);
//
//			// 'cluster_global_init_position' is used to convert from local space of bone to global space of scene.
//			FbxAMatrix clusterGlobalInitPosition;
//			cluster->GetTransformLinkMatrix(clusterGlobalInitPosition);
//
//			// Matrices are defined using the Column Major scheme. When a FbxAMatrix represents a transformation (translation, rotation and scale), the last row of the matrix represents the translation part of the transformation.
//			// Compose 'bone.offset_transform' matrix that trnasforms position from mesh space to bone space. This matrix is called the offset matrix.
//			bone.offsetTransform = to_xmfloat4x4(clusterGlobalInitPosition.Inverse() * referenceGlobalInitPosition);
//		}
//	}
//}
//
//// If 'samplinr_rate' is a negative number, the animation data will not be loaded.
//void SkinnedMesh::FetchAnimations(FbxScene* fbxScene, std::vector<Animation>& animationClips, float samplingRate /*If this value is 0, the animation data will be sampled at the default frame rate.*/)
//{
//	if (samplingRate < 0.0f) return;
//
//	FbxArray<FbxString*> animationStackNames;
//	fbxScene->FillAnimStackNameArray(animationStackNames);
//	const int animationStackCount{ animationStackNames.GetCount() };
//	for (int animationStackIndex = 0; animationStackIndex < animationStackCount; ++animationStackIndex)
//	{
//		Animation& animationClip{ animationClips.emplace_back() };
//		animationClip.name = animationStackNames[animationStackIndex]->Buffer();
//
//		FbxAnimStack* animationStack{ fbxScene->FindMember<FbxAnimStack>(animationClip.name.c_str()) };
//		fbxScene->SetCurrentAnimationStack(animationStack);
//
//		const FbxTime::EMode timeMode{ fbxScene->GetGlobalSettings().GetTimeMode() };
//		FbxTime oneSecond;
//		oneSecond.SetTime(0, 0, 1, 0, 0, timeMode);
//		animationClip.samplingRate = samplingRate > 0 ? samplingRate : static_cast<float>(oneSecond.GetFrameRate(timeMode));
//		const FbxTime samplingInterval{ static_cast<FbxLongLong>(oneSecond.Get() / animationClip.samplingRate) };
//
//		const FbxTakeInfo* takeInfo{ fbxScene->GetTakeInfo(animationClip.name.c_str()) };
//		const FbxTime startTime{ takeInfo->mLocalTimeSpan.GetStart() };
//		const FbxTime stopTime{ takeInfo->mLocalTimeSpan.GetStop() };
//		for (FbxTime time = startTime; time < stopTime; time += samplingInterval)
//		{
//			Animation::Keyframe& keyframe{ animationClip.sequence.emplace_back() };
//
//			const size_t nodeCount{ sceneView.nodes.size() };
//			keyframe.nodes.resize(nodeCount);
//			for (size_t nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex) {
//				FbxNode* fbxNode{ fbxScene->FindNodeByName(sceneView.nodes.at(nodeIndex).name.c_str()) };
//				if (fbxNode) {
//					Animation::Keyframe::Node& node{ keyframe.nodes.at(nodeIndex) };
//					// global_transform 'は、シーンのグローバル座標系に関するノードの変換行列
//					node.globalTransform = to_xmfloat4x4(fbxNode->EvaluateGlobalTransform(time));
//					// local_transform'は、親のローカル座標系に関するノードの変換行列です
//					const FbxAMatrix& localTransform{ fbxNode->EvaluateLocalTransform(time) };
//					node.scaling = to_xmfloat3(localTransform.GetS());
//					node.rotation = to_xmfloat4(localTransform.GetQ());
//					node.translation = to_xmfloat3(localTransform.GetT());
//				}
//#if 0
//				else
//				{
//					animation::keyframe::node& keyframe_node{ keyframe.nodes.at(node_index) };
//					keyframe_node.global_transform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
//					keyframe_node.translation = { 0, 0, 0 };
//					keyframe_node.rotation = { 0, 0, 0 };
//					keyframe_node.scaling = { 1, 1, 1 };
//				}
//#endif
//			}
//		}
//	}
//	for (int animationStackIndex = 0; animationStackIndex < animationStackCount; ++animationStackIndex) delete animationStackNames[animationStackIndex];
//}
//
//void SkinnedMesh::UpdateAnimation(Animation::Keyframe& keyframe)
//{
//	const size_t node_count{ keyframe.nodes.size() };
//	for (size_t nodeIndex = 0; nodeIndex < node_count; ++nodeIndex) {
//		Animation::Keyframe::Node& node{ keyframe.nodes.at(nodeIndex) };
//		XMMATRIX S{ XMMatrixScaling(node.scaling.x, node.scaling.y, node.scaling.z) };
//		XMMATRIX R{ XMMatrixRotationQuaternion(XMLoadFloat4(&node.rotation)) };
//		XMMATRIX T{ XMMatrixTranslation(node.translation.x, node.translation.y, node.translation.z) };
//
//		const int64_t parentIndex{ sceneView.nodes.at(nodeIndex).parentIndex };
//		XMMATRIX P{ parentIndex < 0 ? XMMatrixIdentity() : XMLoadFloat4x4(&keyframe.nodes.at(parentIndex).globalTransform) };
//
//		XMStoreFloat4x4(&node.globalTransform, S * R * T * P);
//	}
//}
//
//bool SkinnedMesh::AppendAnimations(const char* animationFilename, float samplingRate)
//{
//	FbxManager* fbxManager{ FbxManager::Create() };
//	FbxScene* fbxScene{ FbxScene::Create(fbxManager, "") };
//
//	FbxImporter* fbxImporter{ FbxImporter::Create(fbxManager, "") };
//	bool importStatus{ false };
//	importStatus = fbxImporter->Initialize(animationFilename);
//	_ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());
//	importStatus = fbxImporter->Import(fbxScene);
//	_ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());
//
//	FetchAnimations(fbxScene, animationClips, samplingRate/*0:use default value, less than 0:do not fetch*/);
//
//	fbxManager->Destroy();
//
//	return true;
//}
//
//void SkinnedMesh::BlendAnimations(const Animation::Keyframe* keyframes[2], float factor, Animation::Keyframe& keyframe)
//{
//	_ASSERT_EXPR(keyframes[0]->nodes.size() == keyframes[1]->nodes.size(), "The size of the two node arrays must be the same.");
//
//	size_t nodeCount{ keyframes[0]->nodes.size() };
//	keyframe.nodes.resize(nodeCount);
//	for (size_t nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
//	{
//		XMVECTOR S[2]{ XMLoadFloat3(&keyframes[0]->nodes.at(nodeIndex).scaling), XMLoadFloat3(&keyframes[1]->nodes.at(nodeIndex).scaling) };
//		XMStoreFloat3(&keyframe.nodes.at(nodeIndex).scaling, XMVectorLerp(S[0], S[1], factor));
//
//		XMVECTOR R[2]{ XMLoadFloat4(&keyframes[0]->nodes.at(nodeIndex).rotation), XMLoadFloat4(&keyframes[1]->nodes.at(nodeIndex).rotation) };
//		XMStoreFloat4(&keyframe.nodes.at(nodeIndex).rotation, XMQuaternionSlerp(R[0], R[1], factor));
//
//		XMVECTOR T[2]{ XMLoadFloat3(&keyframes[0]->nodes.at(nodeIndex).translation), XMLoadFloat3(&keyframes[1]->nodes.at(nodeIndex).translation) };
//		XMStoreFloat3(&keyframe.nodes.at(nodeIndex).translation, XMVectorLerp(T[0], T[1], factor));
//	}
//}
