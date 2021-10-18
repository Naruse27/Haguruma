#ifndef SKINNEDMESH
#define SKINNEDMESH

#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>
#include <string>
#include "Shader.h"
#include <d3d11.h>
#include <wrl.h>

#include <directxmath.h>

#include <vector>
#include <string>

#include <fbxsdk.h>

#include <set>
#include <unordered_map>

#include <cereal/archives/binary.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/set.hpp>
#include <cereal/types/unordered_map.hpp>

#include <functional>
#include "Mesh.h"


namespace DirectX
{
	template<class T>
	void serialize(T& archive, DirectX::XMFLOAT2& v)
	{
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y)
		);
	}

	template<class T>
	void serialize(T& archive, DirectX::XMFLOAT3& v)
	{
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y),
			cereal::make_nvp("z", v.z)
		);
	}

	template<class T>
	void serialize(T& archive, DirectX::XMFLOAT4& v)
	{
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y),
			cereal::make_nvp("z", v.z),
			cereal::make_nvp("w", v.w)
		);
	}

	template<class T>
	void serialize(T& archive, DirectX::XMFLOAT4X4& m)
	{
		archive(
			cereal::make_nvp("_11", m._11), cereal::make_nvp("_12", m._12),
			cereal::make_nvp("_13", m._13), cereal::make_nvp("_14", m._14),
			cereal::make_nvp("_21", m._21), cereal::make_nvp("_22", m._22),
			cereal::make_nvp("_23", m._23), cereal::make_nvp("_24", m._24),
			cereal::make_nvp("_31", m._31), cereal::make_nvp("_32", m._32),
			cereal::make_nvp("_33", m._33), cereal::make_nvp("_34", m._34),
			cereal::make_nvp("_41", m._41), cereal::make_nvp("_42", m._42),
			cereal::make_nvp("_43", m._43), cereal::make_nvp("_44", m._44)
		);
	}
}

class SkinnedMesh : public GeometricMesh
{
public:
	struct Skeleton
	{
		struct Bone
		{
			uint64_t uniqueId = 0;
			std::string name;
			// 'parent_index' is index that refers to the parent bone's position in the array that contains itself.
			int64_t parentIndex = -1; // -1 : the bone is orphan
			// 'node_index' is an index that refers to the node array of the scene.
			int64_t nodeIndex = 0;

			// 'offset_transform' is used to convert from model(mesh) space to bone(node) scene.
			DirectX::XMFLOAT4X4 offsetTransform = { 1, 0, 0, 0,   0, 1, 0, 0,   0, 0, 1, 0,   0, 0, 0, 1 };

			bool isOrphan() const { return parentIndex < 0; };

			template <class T>
			void serialize(T& archive)
			{
				archive(uniqueId, name, parentIndex, nodeIndex, offsetTransform);
			}
		};
		std::vector<Bone> bones;
		int64_t indexof(uint64_t uniqueId) const {
			int64_t index = 0;
			for (const Bone& bone : bones) {
				if (bone.uniqueId == uniqueId) {
					return index;
				}
				++index;
			}
			return -1;
		}

		template < class T>
		void serialize(T& archive)
		{
			archive(bones);
		}
	};

	struct Scene
	{
		struct Node
		{
			uint64_t uniqueId = 0;
			std::string name;
			FbxNodeAttribute::EType attribute = FbxNodeAttribute::EType::eUnknown;
			int64_t parentIndex = -1;

			template < class T>
			void serialize(T& archive)
			{
				archive(uniqueId, name, attribute, parentIndex);
			}
		};
		std::vector<Node> nodes;
		int64_t indexof(uint64_t uniqueId) const {
			int64_t index = 0;
			for (const Node& node : nodes) {
				if (node.uniqueId == uniqueId) {
					return index;
				}
				++index;
			}
			return -1;
		}
		template <class T>
		void serialize(T& archive)
		{
			archive(nodes);
		}
	};

	struct Animation
	{
		std::string name;
		float samplingRate = 0;

		struct Keyframe
		{
			struct Node
			{
				// 'global_transform' is used to convert from local space of node to global space of scene.
				DirectX::XMFLOAT4X4 globalTransform = { 1, 0, 0, 0,   0, 1, 0, 0,   0, 0, 1, 0,   0, 0, 0, 1 };
				// The transformation data of a node includes its translation, rotation and scaling vectors
				// with respect to its parent. 
				DirectX::XMFLOAT3 scaling = { 1, 1, 1 };
				DirectX::XMFLOAT4 rotation = { 0, 0, 0, 1 }; // Rotation quaternion
				DirectX::XMFLOAT3 translation = { 0, 0, 0 };

				template <class T>
				void serialize(T& archive)
				{
					archive(globalTransform, scaling, rotation, translation);
				}
			};
			std::vector<Node> nodes;

			template <class T>
			void serialize(T& archive)
			{
				archive(nodes);
			}
		};
		std::vector<Keyframe> sequence;

		template <class T>
		void serialize(T& archive)
		{
			archive(name, samplingRate, sequence);
		}
	};
	


	static const int MAX_BONE_INFLUENCES = 4;
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT4 tangent;
		DirectX::XMFLOAT2 texcoord;
		float boneWeights[MAX_BONE_INFLUENCES] = { 1, 0, 0, 0 };
		uint32_t boneIndices[MAX_BONE_INFLUENCES]{};

		template <class T>
		void serialize(T& archive)
		{
			archive(position, normal, tangent, texcoord, boneWeights, boneIndices);
		}
	};

	static const int MAX_BONES = 256;
	struct Constants
	{
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4 materialColor;
		DirectX::XMFLOAT4X4 boneTransforms[MAX_BONES] = {
			{ 1, 0, 0, 0,   0, 1, 0, 0,   0, 0, 1, 0,   0, 0, 0, 1 }
		};
		//template <class T>
		//void Serialize(T& archive)
		//{
		//	archive(world, materialColor, boneTransforms);
		//}
	};



	struct Mesh
	{
		uint64_t uniqueId = 0;
		std::string name;
		// 'node_index' is an index that refers to the node array of the scene.
		int64_t nodeIndex = 0;

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		Skeleton bindPose;




		struct Subset
		{
			uint64_t materialUniqueId = 0;
			std::string materialName;

			uint32_t startIndexLocation = 0;
			uint32_t indexCount = 0;

			template<class T>
			void serialize(T& archive)
			{
				archive(materialUniqueId, materialName, startIndexLocation, indexCount);
			}
		};
		std::vector<Subset> subsets;

		DirectX::XMFLOAT4X4 defaultGlobalTransform = {
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		};
		DirectX::XMFLOAT3 boundingBox[2]
		{
		  { +D3D11_FLOAT32_MAX, +D3D11_FLOAT32_MAX, +D3D11_FLOAT32_MAX },
		  { -D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX }
		};

		template < class T>
		void serialize(T& archive)
		{
			archive(uniqueId, name, nodeIndex, subsets, defaultGlobalTransform,
				bindPose, boundingBox, vertices, indices);
		}
	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
		friend class SkinnedMesh;
	};
	std::vector<Mesh> meshes;

	struct Material
	{
		uint64_t uniqueId = 0;
		std::string name;

		DirectX::XMFLOAT4 Ka = { 0.2f, 0.2f, 0.2f, 1.0f };
		DirectX::XMFLOAT4 Kd = { 0.8f, 0.8f, 0.8f, 1.0f };
		DirectX::XMFLOAT4 Ks = { 1.0f, 1.0f, 1.0f, 1.0f };

		std::string textureFilenames[4];
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceViews[4];

		template < class T>
		void serialize(T& archive)
		{
			archive(uniqueId, name, Ka, Kd, Ks, textureFilenames);
		}
	};
	std::unordered_map<uint64_t, Material> materials;

	std::vector<Animation> animationClips;

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffers[8];



	inline DirectX::XMFLOAT4X4 to_xmfloat4x4(const FbxAMatrix& fbxamatrix) {
		DirectX::XMFLOAT4X4 xmfloat4x4;
		for (int row = 0; row < 4; row++) {
			for (int column = 0; column < 4; column++) {
				xmfloat4x4.m[row][column] = static_cast<float>(fbxamatrix[row][column]);
			}
		}
		return xmfloat4x4;
	}
	inline DirectX::XMFLOAT3 to_xmfloat3(const FbxDouble3& fbxdouble3) {
		DirectX::XMFLOAT3 xmfloat3;
		xmfloat3.x = static_cast<float>(fbxdouble3[0]);
		xmfloat3.y = static_cast<float>(fbxdouble3[1]);
		xmfloat3.z = static_cast<float>(fbxdouble3[2]);
		return xmfloat3;
	}
	inline DirectX::XMFLOAT4 to_xmfloat4(const FbxDouble4& fbxdouble4) {
		DirectX::XMFLOAT4 xmfloat4;
		xmfloat4.x = static_cast<float>(fbxdouble4[0]);
		xmfloat4.y = static_cast<float>(fbxdouble4[1]);
		xmfloat4.z = static_cast<float>(fbxdouble4[2]);
		xmfloat4.w = static_cast<float>(fbxdouble4[3]);
		return xmfloat4;
	}



public:
	SkinnedMesh(ID3D11Device* device, const char* fbxFilename, bool triangulate = false, float samplingRate = 0);
	virtual ~SkinnedMesh() {};

	void FetchMeshes(FbxScene* fbxScene, std::vector<Mesh>& meshes);
	void FetchMaterials(FbxScene* fbxScene, std::unordered_map<uint64_t, Material>& materials);
	void FetchSkeleton(FbxMesh* fbxMesh, Skeleton& bindPose);
	void FetchAnimations(FbxScene* fbxScene, std::vector<Animation>& animationClips, float samplingRate);
	void CreateComObjects(ID3D11Device* device, const char* fbxFilename);

	void Preparation(ID3D11DeviceContext* immediateContext, Shader shader, bool wireframe);
	void Render(ID3D11DeviceContext* deviceContext, const DirectX::XMFLOAT4X4& world, const Vector4& materialColor, Shader shader, const Animation::Keyframe* keyframe = nullptr);
	void Render(ID3D11DeviceContext* deviceContext, const DirectX::XMFLOAT4X4& world, const Vector4& materialColor = { 1.0f, 1.0f, 1.0f, 1.0f }, const Animation::Keyframe* keyframe = nullptr);

	void UpdateAnimation(Animation::Keyframe& keyframe);
	bool AppendAnimation(const char* animationFilename, float samplingRate);
	void BlendAnimations(const Animation::Keyframe* keyframes[2], float factor, Animation::Keyframe& keyframe);

	const std::vector<Mesh>& GetMeshs() const { return meshes; }
	const std::vector<Animation>& GetAnimation() { return animationClips; }

protected:
	Scene sceneView;
};

//class SkinnedMesh : public GeometricMesh
//{
//public:
//	struct Skeleton
//	{
//		struct Bone
//		{
//			uint64_t uniqueId{ 0 };
//			std::string name;
//			// 'parent_index' is index that refers to the parent bone's position in the array that contains itself.
//			int64_t parentIndex{ -1 }; // -1 : the bone is orphan
//			// 'node_index' is an index that refers to the node array of the scene.
//			int64_t nodeIndex{ 0 };
//
//			// 'offset_transform' is used to convert from model(mesh) space to bone(node) scene.
//			DirectX::XMFLOAT4X4 offsetTransform{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
//
//			bool IsOrphan() const { return parentIndex < 0; };
//
//			// UNIT.30
//			template<class T>
//			void serialize(T& archive)
//			{
//				archive(uniqueId, name, parentIndex, nodeIndex, offsetTransform);
//			}
//		};
//		std::vector<Bone> bones;
//		int64_t Indexof(uint64_t uniqueId) const
//		{
//			int64_t index{ 0 };
//			for (const Bone& bone : bones)
//			{
//				if (bone.uniqueId == uniqueId)
//				{
//					return index;
//				}
//				++index;
//			}
//			return -1;
//		}
//#if 0
//		int64_t indexof(string name) const
//		{
//			int64_t index{ 0 };
//			for (const bone& bone : bones)
//			{
//				if (bone.name == name)
//				{
//					return index;
//				}
//				++index;
//			}
//			return -1;
//		}
//#endif
//
//		template<class T>
//		void serialize(T& archive)
//		{
//			archive(bones);
//		}
//	};
//
//	struct Animation
//	{
//		std::string name;
//		float samplingRate{ 0 };
//
//		struct Keyframe
//		{
//			struct Node
//			{
//				// 'global_transform' is used to convert from local space of node to global space of scene.
//				DirectX::XMFLOAT4X4 globalTransform{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
//				// The transformation data of a node includes its translation, rotation and scaling vectors with respect to its parent. 
//				DirectX::XMFLOAT3 scaling{ 1, 1, 1 };
//				DirectX::XMFLOAT4 rotation{ 0, 0, 0, 1 }; // Rotation quaternion
//				DirectX::XMFLOAT3 translation{ 0, 0, 0 };
//
//				// UNIT.30
//				template<class T>
//				void serialize(T& archive)
//				{
//					archive(globalTransform, scaling, rotation, translation);
//				}
//			};
//			std::vector<Node> nodes;
//
//			template<class T>
//			void serialize(T& archive)
//			{
//				archive(nodes);
//			}
//		};
//		std::vector<Keyframe> sequence;
//
//		template<class T>
//		void serialize(T& archive)
//		{
//			archive(name, samplingRate, sequence);
//		}
//	};
//
//	struct Scene // FBXデータの保存
//	{
//		struct Node
//		{
//			uint64_t uniqueId{ 0 }; // ノードデータ
//			std::string name; // ノードの名前
//			FbxNodeAttribute::EType attribute{ FbxNodeAttribute::EType::eUnknown }; // アトリビュートの種類　Attributeで種類を調べればNodeにどのような情報が保存されているかがわかる
//			int64_t parentIndex{ -1 }; // 親子関係の数
//
//			template<class T>
//			void serialize(T& archive)
//			{
//				archive(uniqueId, name, attribute, parentIndex);
//			}
//		};
//		std::vector<Node> nodes;
//		int64_t indexof(uint64_t uniqueId) const
//		{
//			int64_t index{ 0 };
//			for (const Node& node : nodes)
//			{
//				if (node.uniqueId == uniqueId)
//				{
//					return index;
//				}
//				++index;
//			}
//			return -1;
//		}
//#if 0
//		int64_t indexof(string name) const
//		{
//			int64_t index{ 0 };
//			for (const node& node : nodes)
//			{
//				if (node.name == name)
//				{
//					return index;
//				}
//				++index;
//			}
//			return -1;
//		}
//#endif
//		// UNIT.30
//		template<class T>
//		void serialize(T& archive)
//		{
//			archive(nodes);
//		}
//	};
//
//	struct VertexPosition
//	{
//		DirectX::XMFLOAT3 position;
//
//		template<class T>
//		void serialize(T& archive)
//		{
//			archive(position);
//		}
//	};
//
//	struct VertexExtraAttribute
//	{
//		DirectX::XMFLOAT3 normal;
//		DirectX::XMFLOAT4 tangent;
//		DirectX::XMFLOAT2 texcoord;
//
//		template<class T>
//		void serialize(T& archive)
//		{
//			archive(normal, tangent, texcoord);
//		}
//	};
//
//	static const int MAX_BONE_INFLUENCES{ 4 };
//	struct VertexBoneInfluence // ボーンの影響
//	{
//		float boneWeights[MAX_BONE_INFLUENCES]{ 1, 0, 0, 0 };
//		uint32_t boneIndices[MAX_BONE_INFLUENCES]{};
//
//		template<class T>
//		void serialize(T& archive)
//		{
//			archive(boneWeights, boneIndices);
//		}
//	};
//	static const int MAX_BONES{ 256 }; // UNIT.23
//	struct Constants
//	{
//		DirectX::XMFLOAT4X4 world;
//	};
//	struct BoneConstants
//	{
//		DirectX::XMFLOAT4X4 boneTransforms[MAX_BONES]{ { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 } };
//	};
//	struct MaterialConstants
//	{
//		/*
//		// Phong
//			  // Lambert
//		DirectX::XMFLOAT4 ambient{ 0.2f, 0.2f, 0.2f, 1.0f }; // 環境光ライトに対する反射色
//		DirectX::XMFLOAT4 diffuse{ 0.8f, 0.8f, 0.8f, 1.0f }; // The w component is 'opacity'.　ある点に当たった光が四方八方に反射する強度。強いほど良く反射
//		DirectX::XMFLOAT4 emissive{ 0.0f, 0.0f, 0.0f, 1.0f }; // ライトに関係なく自身が放射する光。真っ暗な世界でも光れます
//		DirectX::XMFLOAT4 bump{ 0.0f, 0.0f, 0.0f, 1.0f }; // 表面の凸凹の強度。テクスチャとワンセットが普通
//		DirectX::XMFLOAT4 transparency{ 0.0f, 0.0f, 0.0f, 1.0f }; // 光の透過具合。値が大きいほど不透明。
//			   //
//		DirectX::XMFLOAT4 specular{ 0.0f, 0.0f, 0.0f, 1.0f }; // The w component is 'shininess'.　ある点に当たった光が特定の方向に反射する強度。鏡面のような効果
//		DirectX::XMFLOAT4 shininess{ 0.0f, 0.0f, 0.0f, 1.0f }; // 光沢
//		DirectX::XMFLOAT4 reflection{ 0.0f, 0.0f, 0.0f, 1.0f }; // 反射
//
//		DirectX::XMFLOAT2 texcoord_offset{ 0.0f, 0.0f };
//		DirectX::XMFLOAT2 texcoord_scale{ 1.0f, 1.0f };*/
//		DirectX::XMFLOAT4 ambient{ 0.2f, 0.2f, 0.2f, 1.0f };
//		DirectX::XMFLOAT4 diffuse{ 0.8f, 0.8f, 0.8f, 1.0f }; // The w component is 'opacity'. コンポーネントは「不透明度」
//		DirectX::XMFLOAT4 specular{ 0.0f, 0.0f, 0.0f, 1.0f }; // The w component is 'shininess'. // 成分は '光沢です
//		DirectX::XMFLOAT4 reflection{ 0.0f, 0.0f, 0.0f, 1.0f };
//		DirectX::XMFLOAT4 emissive{ 0.0f, 0.0f, 0.0f, 1.0f };
//		DirectX::XMFLOAT2 texcoord_offset{ 0.0f, 0.0f };
//		DirectX::XMFLOAT2 texcoord_scale{ 1.0f, 1.0f };
//	};
//	struct Mesh
//	{
//		uint64_t uniqueId{ 0 }; // メッシュID
//		std::string name;
//		// 'node_index' is an index that refers to the node array of the scene.
//		int64_t nodeIndex{ 0 }; // シーンのノード配列を参照するインデックス
//
//		std::vector<VertexPosition> vertexPositions;
//		std::vector<VertexExtraAttribute> vertexExtraAttributes;
//		std::vector<VertexBoneInfluence> vertexBoneInfluences; // UNIT.99
//		std::vector<uint32_t> indices;
//
//		geometric_attribute attribute{ geometric_attribute::skinnned_mesh }; // UNIT.99
//
//		struct Subset // サブセット
//		{
//			uint64_t materialUniqueId{ 0 };
//			std::string materialName;
//
//			uint32_t startIndexLocation{ 0 }; // GPUによってインデックスバッファから読み取られた最初のインデックスの場所
//			uint32_t indexCount{ 0 }; // 描画するインデックスの数
//			uint8_t draworder{ 0x7F };
//			bool operator<(const Subset& rhs) { return draworder < rhs.draworder; }
//
//			template<class T>
//			void serialize(T& archive)
//			{
//				archive(materialUniqueId, materialName, startIndexLocation, indexCount);
//			}
//		};
//		std::vector<Subset> subsets;
//
//		DirectX::XMFLOAT4X4 defaultGlobalTransform{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
//		DirectX::XMFLOAT4X4 geometricTransform{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
//
//		Skeleton bindPose;
//
//		uint8_t draworder{ 0x7F };
//		bool operator<(const Mesh& rhs)
//		{
//			return draworder < rhs.draworder;
//		}
//
//		// bounding_box [0] is the min vertex and [1] is the max
//		DirectX::XMFLOAT3 boundingBox[2];
//		// この関数は、ワールド空間のバウンディングボックスの座標を計算して出力します
//		void TransformBoundingBox(const DirectX::XMFLOAT4X4& worldTransform, DirectX::XMFLOAT3 worldBoundingBox[2]) const
//		{
//			DirectX::XMMATRIX concatenatedMatrix{
//				DirectX::XMLoadFloat4x4(&geometricTransform) *
//				DirectX::XMLoadFloat4x4(&defaultGlobalTransform) *
//				DirectX::XMLoadFloat4x4(&worldTransform) };
//			DirectX::XMStoreFloat3(&worldBoundingBox[0], DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&boundingBox[0]), concatenatedMatrix));
//			DirectX::XMStoreFloat3(&worldBoundingBox[1], DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&boundingBox[1]), concatenatedMatrix));
//
//			if (worldBoundingBox[0].x > worldBoundingBox[1].x) std::swap<float>(worldBoundingBox[0].x, worldBoundingBox[1].x);
//			if (worldBoundingBox[0].y > worldBoundingBox[1].y) std::swap<float>(worldBoundingBox[0].y, worldBoundingBox[1].y);
//			if (worldBoundingBox[0].z > worldBoundingBox[1].z) std::swap<float>(worldBoundingBox[0].z, worldBoundingBox[1].z);
//		}
//
//		template<class T>
//		void serialize(T& archive)
//		{
//			archive(uniqueId, name, nodeIndex, subsets, defaultGlobalTransform, geometricTransform/*UNIT.99*/, bindPose, boundingBox, vertexPositions/*UNIT.99*/, vertexExtraAttributes/*UNIT.99*/, vertexBoneInfluences/*UNIT.99*/, indices);
//		}
//	private:
//		Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffers[3];
//		Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
//		friend class SkinnedMesh; // UNIT.99
//	};
//	struct Material
//	{
//		uint64_t uniqueId{ 0 };
//		std::string name;
//		/*
//		// Phong
//			  // Lambert
//		DirectX::XMFLOAT4 ambient{ 0.2f, 0.2f, 0.2f, 1.0f }; // 環境光ライトに対する反射色
//		DirectX::XMFLOAT4 diffuse{ 0.8f, 0.8f, 0.8f, 1.0f }; // The w component is 'opacity'.　ある点に当たった光が四方八方に反射する強度。強いほど良く反射
//		DirectX::XMFLOAT4 emissive{ 0.0f, 0.0f, 0.0f, 1.0f }; // ライトに関係なく自身が放射する光。真っ暗な世界でも光れます
//		DirectX::XMFLOAT4 bump{ 0.0f, 0.0f, 0.0f, 1.0f }; // 表面の凸凹の強度。テクスチャとワンセットが普通
//		DirectX::XMFLOAT4 transparency{ 0.0f, 0.0f, 0.0f, 1.0f }; // 光の透過具合。値が大きいほど不透明。
//			   //
//		DirectX::XMFLOAT4 specular{ 0.0f, 0.0f, 0.0f, 1.0f }; // The w component is 'shininess'.　ある点に当たった光が特定の方向に反射する強度。鏡面のような効果
//		DirectX::XMFLOAT4 shininess{ 0.0f, 0.0f, 0.0f, 1.0f }; // 光沢
//		DirectX::XMFLOAT4 reflection{ 0.0f, 0.0f, 0.0f, 1.0f }; // 反射
//
//		DirectX::XMFLOAT2 texcoord_offset{ 0.0f, 0.0f };
//		DirectX::XMFLOAT2 texcoord_scale{ 1.0f, 1.0f };*/
//		DirectX::XMFLOAT4 ambient{ 0.2f, 0.2f, 0.2f, 1.0f };
//		DirectX::XMFLOAT4 diffuse{ 0.8f, 0.8f, 0.8f, 1.0f }; // The w component is 'opacity'.
//		DirectX::XMFLOAT4 specular{ 0.0f, 0.0f, 0.0f, 1.0f }; // The w component is 'shininess'.
//		DirectX::XMFLOAT4 reflection{ 0.0f, 0.0f, 0.0f, 1.0f };
//		DirectX::XMFLOAT4 emissive{ 0.0f, 0.0f, 0.0f, 1.0f };
//
//		std::string textureFilenames[4];
//		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceViews[4];
//
//		template<class T>
//		void serialize(T& archive)
//		{
//			archive(uniqueId, name, ambient, diffuse, specular, reflection, emissive, textureFilenames);
//		}
//	};
//public:
//	std::vector<Mesh> meshes;
//	
//	std::unordered_map<uint64_t, Material> materials;
//	std::vector<Animation> animationClips;
//
//	struct PipelineState
//	{
//		D3D11_PRIMITIVE_TOPOLOGY topology;
//		ID3D11VertexShader* vertexShader;
//		ID3D11HullShader* hullShader;
//		ID3D11DomainShader* domainShader;
//		ID3D11GeometryShader* geometryShader;
//		ID3D11PixelShader* pixelShader;
//		ID3D11RasterizerState* rasterizerState;
//		ID3D11BlendState* blendState;
//		ID3D11DepthStencilState* depthStencilState;
//		UINT stencilRef;
//	};
//	struct ShaderResources
//	{
//		MaterialConstants materialData;
//		ID3D11ShaderResourceView* shaderResourceViews[4];
//	};
//
//private:
//	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShaders[4];
//	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShaders[2];
//	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayouts[2];
//	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffers[3];
//
//	void CreateComObjects(ID3D11Device* device, const char* fbx_filename);
//
//public:
//	// 'samplinr_rate'が負の数の場合、アニメーションデータはロードされません
//	SkinnedMesh(ID3D11Device* device, const char* fbxFilename, bool triangulate = false, float samplingRate = 0,
//		std::function<void(Mesh&, Mesh::Subset& subset)> draworderCallback = [](Mesh&, Mesh::Subset&) {}, bool avoidCreateComObjects = false/*UNIT.99*/);
//
//	SkinnedMesh(ID3D11Device* device, const char* fbxFilename, std::vector<std::string>& animationFilenames, bool triangulate = false, float sampling_rate = 0);
//	~SkinnedMesh() {};
//	// Turning on 'shadow_casting' flag variable binds only the vertex position (and bone influence to the pipeline). It also does not bind any textures.
//	// 'shadow_casting'フラグ変数をオンにすると、頂点位置（およびパイプラインへのボーンの影響）のみがバインドされます。また、テクスチャをバインドしません。
//	// The shadow mapping method assumes double speed z.
//	// シャドウマッピングメソッドは2倍の速度zを想定しています
//	void Render(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT4X4& world, const Animation::Keyframe* keyframe/*UNIT.25*/,
//		std::function<int(const Mesh&, const Material&, ShaderResources&, PipelineState&)> callback = [](const Mesh&, const Material&, ShaderResources&, PipelineState&) { return 0; }, bool shadowCasting = false);
//	void Preparation(ID3D11DeviceContext* immediateContext, Shader shader, bool wireframe);
//	void Render(ID3D11DeviceContext* deviceContext, const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& materialColor = { 1.0f, 1.0f, 1.0f, 1.0f }, const Animation::Keyframe* keyframe = nullptr);
//
//
//	// ここからはモデルに移動させる
//	void UpdateAnimation(Animation::Keyframe& keyframe);
//	bool AppendAnimations(const char* animationFilename, float samplingRate /*0:use default value*/);
//	void BlendAnimations(const Animation::Keyframe* keyframes[2], float factor, Animation::Keyframe& keyframe);
//
//protected:
//	Scene sceneView;
//	void FetchMeshes(FbxScene* fbxScene, std::vector<Mesh>& meshes);
//	void FetchMaterials(FbxScene* fbxScene, std::unordered_map<uint64_t, Material>& materials);
//	void FetchSkeleton(FbxMesh* fbxMesh, Skeleton& bindPose);
//	//  'samplinr_rate'が負の数の場合、アニメーションデータはロードされません
//	void FetchAnimations(FbxScene* fbxScene, std::vector<Animation>& animationClips, float samplingRate/*If this value is 0, the animation data will be sampled at the default frame rate.*/);
//	void FetchScene(const char* fbxFilename, bool triangulate, float samplingRate/*If this value is 0, the animation data will be sampled at the default frame rate.*/);
//};


#endif // !SKINNEDMESH