#include "Model.h"
#include "FrameWork.h"
//#include "StaticMesh.h"
#include "SkinnedMesh.h"
//#include "SkinnedMeshBatch.h"

Model::Model(ID3D11Device* device, const char* fbxFilename, bool triangulate, float samplingRate, bool avoidCreateComObjects, BOOL handedCoordinateSystem)
{
	SkinnedMesh mesh(device, fbxFilename, triangulate, samplingRate);
	mesh.Init(device, handedCoordinateSystem);
	model.emplace(this->index, std::move(mesh));
}

//Model::Model(ID3D11Device* device, const char* fbxFilename, std::vector<std::string>& animationFilenames, bool triangulate, float samplingRate, int index)
//{
//	SkinnedMesh mesh(device, fbxFilename, animationFilenames, triangulate, samplingRate);
//	model.emplace(index, mesh);
//}

Model::Model(ID3D11Device* device, std::vector<const char*> fbxFilenames, bool triangulate, float samplingRate, BOOL handedCoordinateSystem)
{
	for (auto& filename : fbxFilenames) {
		SkinnedMesh mesh(device, filename, triangulate, samplingRate);
		mesh.Init(device, handedCoordinateSystem);
		model.emplace(this->index, mesh);
		this->index++;
	}
	this->index = 0;
}

Model::~Model()
{
	model.clear();
}

void Model::PlayAnimation(int index)
{
	//auto it = model.find(this->index);
	//auto it2 = model.find(index);
	//SkinnedMesh::Animation::Keyframe keyframe;
	//const SkinnedMesh::Animation::Keyframe* keyframes[2] = {
	//	&it->second.animationClips.at(80).sequence.at(80),
	//	&it2->second.animationClips.at(0).sequence.at(0)
	//};
	this->index = index;
}

void Model::Preparation(ID3D11DeviceContext* immediateContext, Shader shader, bool wireframe)
{
	auto it = model.find(this->index);
	it->second.Preparation(immediateContext, shader, this->index);
}

void Model::Render(ID3D11DeviceContext* immediateContext, const Vector4 materialColor)  // , const Animation::Keyframe* keyframe = nullptr);
{
	Shader shader{};
	auto it = model.find(this->index);
	it->second.Render(immediateContext, this->transform, materialColor, shader, keyframe);
}

void Model::AnimationInput(const char* fbxFilename, int index)
{
}

void Model::UpdateAnimation(float elapsedTime, bool loop)
{
#if 0
	float factor = 0.5f;
	auto it = model.find(this->index);
	SkinnedMesh::Animation::Keyframe keyframe;
	const SkinnedMesh::Animation::Keyframe* keyframes[2] = {
		&it->second.animationClips.at(0).sequence.at(40),
		&it->second.animationClips.at(0).sequence.at(80)
	};
	it->second.BlendAnimations(keyframes, factor, keyframe);
	it->second.UpdateAnimation(keyframe);
#else 
	auto it = model.find(this->index);
	int clipIndex = 0;
	int frameIndex = 0;
	static float animationTick = 0;
	bool check = false;

	SkinnedMesh::Animation& animation = it->second.animationClips.at(clipIndex);//skinnedMeshes[0]->animationClips.at(clipIndex);
	frameIndex = static_cast<int>(animationTick * animation.samplingRate);
	if (frameIndex > animation.sequence.size() - 1) {
		check = true;
		if (loop) {
			frameIndex = 0;
			animationTick = 0;
			check = false;
		}
	}
	else {
		animationTick += elapsedTime;
	}
	
	if (!check) keyframe = &animation.sequence.at(frameIndex);
#endif
}

void Model::UpdateTransform(const DirectX::XMFLOAT4X4& transform)
{
	this->transform = transform;
	//DirectX::XMMATRIX Transform = DirectX::XMLoadFloat4x4(&transform);
	//
	//for (SkinnedMesh::Node& node : nodes)
	//{
	//	// ローカル行列算出
	//	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(node.scale.x, node.scale.y, node.scale.z);
	//	DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node.rotate));
	//	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(node.translate.x, node.translate.y, node.translate.z);
	//	DirectX::XMMATRIX LocalTransform = S * R * T;
	//
	//	// ワールド行列算出
	//	DirectX::XMMATRIX ParentTransform;
	//	if (node.parent != nullptr)
	//	{
	//		ParentTransform = DirectX::XMLoadFloat4x4(&node.parent->worldTransform);
	//	}
	//	else
	//	{
	//		ParentTransform = Transform;
	//	}
	//	DirectX::XMMATRIX WorldTransform = LocalTransform * ParentTransform;
	//
	//	// 計算結果を格納
	//	DirectX::XMStoreFloat4x4(&node.localTransform, LocalTransform);
	//	DirectX::XMStoreFloat4x4(&node.worldTransform, WorldTransform);
	//}
}

//void Model::Begin(Shader shader, bool wireframe)
//{
//	Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediateContext = FrameWork::GetInstance().GetContext();
//
//	if (pMesh)
//	{
//		pMesh->Begin(immediateContext.Get(), shader, wireframe);
//	}
//}
//
//void Model::Render
//(OBJ3DInstance& obj,
//	//const DirectX::XMMATRIX& world,
//	const DirectX::XMMATRIX& view,
//	const DirectX::XMMATRIX& projection,
//	float elapsedTime)
//{
//	Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediateContext = FrameWork::GetInstance().GetContext();
//
//	//	ワールド変換行列取得
//	DirectX::XMMATRIX worldM = obj.GetWorldMatrix();
//
//	//	Matrix -> Float4x4 変換
//	DirectX::XMFLOAT4X4 wvp;
//	DirectX::XMFLOAT4X4 _world;
//
//	DirectX::XMStoreFloat4x4(&wvp, worldM * view * projection);
//	DirectX::XMStoreFloat4x4(&_world, worldM);
//
//	//	描画
//	if (pMesh)
//	{
//		pMesh->Render(immediateContext.Get(), obj, wvp, _world, elapsedTime);
//	}
//}
//
//void Model::End
//(const DirectX::XMFLOAT4& lightDirection,
//	const DirectX::XMFLOAT4& materialColor)
//{
//	Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediateContext = FrameWork::GetInstance().GetContext();
//
//	if (pMesh)
//	{
//		pMesh->End(immediateContext.Get(), lightDirection, materialColor);
//	}
//}

bool Model::JudgeInCamera(DirectX::XMFLOAT3 position)
{
	//DirectX::XMFLOAT3 _cameraFoward = CameraSystem::GetInstance()->mainView.GetFoward();
	//
	//DirectX::XMVECTOR cameraFowardVec = DirectX::XMLoadFloat2(&DirectX::XMFLOAT2(_cameraFoward.x, _cameraFoward.z));
	//
	//DirectX::XMFLOAT3 _cameraPos = CameraSystem::GetInstance()->mainView.GetPos();
	//DirectX::XMVECTOR modelToCamaraVec = DirectX::XMLoadFloat2(&DirectX::XMFLOAT2(pos.x - _cameraPos.x, pos.z - _cameraPos.z));
	//
	//// ベクトルの長さ
	//float cameraFowardLength = 0.0f;
	//float modelToCamaraLength = 0.0f;
	//DirectX::XMVECTOR cameraFowardVecN = DirectX::XMVector2Normalize(cameraFowardVec);
	//DirectX::XMVECTOR modelToCamaraVecN = DirectX::XMVector2Normalize(modelToCamaraVec);
	//DirectX::XMStoreFloat(&cameraFowardLength, DirectX::XMVector2Length(cameraFowardVecN));
	//DirectX::XMStoreFloat(&modelToCamaraLength, DirectX::XMVector2Length(modelToCamaraVecN));
	//
	//// 内積
	//float dot = 0.0f;
	//DirectX::XMStoreFloat(&dot, DirectX::XMVector2Dot(cameraFowardVecN, modelToCamaraVecN));
	//
	//// cosΘ
	//float cosSita = dot / (cameraFowardLength * modelToCamaraLength);
	//
	//// cosΘからΘ
	//if (1.0f < cosSita)
	//{
	//	cosSita = 1.0f;
	//}
	//float sita = acosf(cosSita);
	//
	////角度(degree)
	//float degreeSita = sita * 180.0f / PI;
	//
	//// カメラに映っているか判定
	//float degreeFov = (CameraSystem::GetInstance()->mainView.GetFov() * 180.0f / PI);
	//if (-degreeFov <= degreeSita && degreeSita <= degreeFov)
	//{
	//	return true;
	//}

	return false;
}