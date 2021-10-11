#ifndef MESH
#define MESH

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
#include "Vector.h"
//namespace DirectX
//{
//	template<class T>
//	void serialize(T& archive, DirectX::XMFLOAT2& v)
//	{
//		archive(
//			cereal::make_nvp("x", v.x),
//			cereal::make_nvp("y", v.y)
//		);
//	}
//
//	template<class T>
//	void serialize(T& archive, DirectX::XMFLOAT3& v)
//	{
//		archive(
//			cereal::make_nvp("x", v.x),
//			cereal::make_nvp("y", v.y),
//			cereal::make_nvp("z", v.z)
//		);
//	}
//
//	template<class T>
//	void serialize(T& archive, DirectX::XMFLOAT4& v)
//	{
//		archive(
//			cereal::make_nvp("x", v.x),
//			cereal::make_nvp("y", v.y),
//			cereal::make_nvp("z", v.z),
//			cereal::make_nvp("w", v.w)
//		);
//	}
//
//	template<class T>
//	void serialize(T& archive, DirectX::XMFLOAT4X4& m)
//	{
//		archive(
//			cereal::make_nvp("_11", m._11), cereal::make_nvp("_12", m._12),
//			cereal::make_nvp("_13", m._13), cereal::make_nvp("_14", m._14),
//			cereal::make_nvp("_21", m._21), cereal::make_nvp("_22", m._22),
//			cereal::make_nvp("_23", m._23), cereal::make_nvp("_24", m._24),
//			cereal::make_nvp("_31", m._31), cereal::make_nvp("_32", m._32),
//			cereal::make_nvp("_33", m._33), cereal::make_nvp("_34", m._34),
//			cereal::make_nvp("_41", m._41), cereal::make_nvp("_42", m._42),
//			cereal::make_nvp("_43", m._43), cereal::make_nvp("_44", m._44)
//		);
//	}
//}

class GeometricMesh
{
protected:
    struct SceneConstants
    {
        DirectX::XMFLOAT4X4 viewProjection; //�r���[�E�v���W�F�N�V�����ϊ��s��
        DirectX::XMFLOAT4     lightDirection; //���C�g�̌���
        DirectX::XMFLOAT4     cameraPosition;//�J�����ʒu
    };

protected:
    Microsoft::WRL::ComPtr<ID3D11RasterizerState>		wireframeRasterizerState;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState>		solidRasterizerState;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState>		depthStencilState;
    Microsoft::WRL::ComPtr<ID3D11SamplerState>			samplerState;

    // TRUE : right handed , FALSE : left handed
    BOOL handedCoordinateSystem = FALSE;

    //protected:
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
    //	struct Scene // FBX�f�[�^�̕ۑ�
    //	{
    //		struct Node
    //		{
    //			uint64_t uniqueId{ 0 }; // �m�[�h�f�[�^
    //			std::string name; // �m�[�h�̖��O
    //			FbxNodeAttribute::EType attribute{ FbxNodeAttribute::EType::eUnknown }; // �A�g���r���[�g�̎�ށ@Attribute�Ŏ�ނ𒲂ׂ��Node�ɂǂ̂悤�ȏ�񂪕ۑ�����Ă��邩���킩��
    //			int64_t parentIndex{ -1 }; // �e�q�֌W�̐�
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
    //	struct VertexBoneInfluence // �{�[���̉e��
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
    //		DirectX::XMFLOAT4 ambient{ 0.2f, 0.2f, 0.2f, 1.0f }; // �������C�g�ɑ΂��锽�ːF
    //		DirectX::XMFLOAT4 diffuse{ 0.8f, 0.8f, 0.8f, 1.0f }; // The w component is 'opacity'.�@����_�ɓ������������l�������ɔ��˂��鋭�x�B�����قǗǂ�����
    //		DirectX::XMFLOAT4 emissive{ 0.0f, 0.0f, 0.0f, 1.0f }; // ���C�g�Ɋ֌W�Ȃ����g�����˂�����B�^���ÂȐ��E�ł�����܂�
    //		DirectX::XMFLOAT4 bump{ 0.0f, 0.0f, 0.0f, 1.0f }; // �\�ʂ̓ʉ��̋��x�B�e�N�X�`���ƃ����Z�b�g������
    //		DirectX::XMFLOAT4 transparency{ 0.0f, 0.0f, 0.0f, 1.0f }; // ���̓��ߋ�B�l���傫���قǕs�����B
    //			   //
    //		DirectX::XMFLOAT4 specular{ 0.0f, 0.0f, 0.0f, 1.0f }; // The w component is 'shininess'.�@����_�ɓ���������������̕����ɔ��˂��鋭�x�B���ʂ̂悤�Ȍ���
    //		DirectX::XMFLOAT4 shininess{ 0.0f, 0.0f, 0.0f, 1.0f }; // ����
    //		DirectX::XMFLOAT4 reflection{ 0.0f, 0.0f, 0.0f, 1.0f }; // ����
    //
    //		DirectX::XMFLOAT2 texcoord_offset{ 0.0f, 0.0f };
    //		DirectX::XMFLOAT2 texcoord_scale{ 1.0f, 1.0f };*/
    //		DirectX::XMFLOAT4 ambient{ 0.2f, 0.2f, 0.2f, 1.0f };
    //		DirectX::XMFLOAT4 diffuse{ 0.8f, 0.8f, 0.8f, 1.0f }; // The w component is 'opacity'. �R���|�[�l���g�́u�s�����x�v
    //		DirectX::XMFLOAT4 specular{ 0.0f, 0.0f, 0.0f, 1.0f }; // The w component is 'shininess'. // ������ '����ł�
    //		DirectX::XMFLOAT4 reflection{ 0.0f, 0.0f, 0.0f, 1.0f };
    //		DirectX::XMFLOAT4 emissive{ 0.0f, 0.0f, 0.0f, 1.0f };
    //		DirectX::XMFLOAT2 texcoord_offset{ 0.0f, 0.0f };
    //		DirectX::XMFLOAT2 texcoord_scale{ 1.0f, 1.0f };
    //	};
    //	struct Mesh
    //	{
    //		uint64_t uniqueId{ 0 }; // ���b�V��ID
    //		std::string name;
    //		// 'node_index' is an index that refers to the node array of the scene.
    //		int64_t nodeIndex{ 0 }; // �V�[���̃m�[�h�z����Q�Ƃ���C���f�b�N�X
    //
    //		std::vector<VertexPosition> vertexPositions;
    //		std::vector<VertexExtraAttribute> vertexExtraAttributes;
    //		std::vector<VertexBoneInfluence> vertexBoneInfluences; // UNIT.99
    //		std::vector<uint32_t> indices;
    //
    //		geometric_attribute attribute{ geometric_attribute::skinnned_mesh }; // UNIT.99
    //
    //		struct Subset // �T�u�Z�b�g
    //		{
    //			uint64_t materialUniqueId{ 0 };
    //			std::string materialName;
    //
    //			uint32_t startIndexLocation{ 0 }; // GPU�ɂ���ăC���f�b�N�X�o�b�t�@����ǂݎ��ꂽ�ŏ��̃C���f�b�N�X�̏ꏊ
    //			uint32_t indexCount{ 0 }; // �`�悷��C���f�b�N�X�̐�
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
    //		// ���̊֐��́A���[���h��Ԃ̃o�E���f�B���O�{�b�N�X�̍��W���v�Z���ďo�͂��܂�
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
    //
    //		/*
    //		// Phong
    //			  // Lambert
    //		DirectX::XMFLOAT4 ambient{ 0.2f, 0.2f, 0.2f, 1.0f }; // �������C�g�ɑ΂��锽�ːF
    //		DirectX::XMFLOAT4 diffuse{ 0.8f, 0.8f, 0.8f, 1.0f }; // The w component is 'opacity'.�@����_�ɓ������������l�������ɔ��˂��鋭�x�B�����قǗǂ�����
    //		DirectX::XMFLOAT4 emissive{ 0.0f, 0.0f, 0.0f, 1.0f }; // ���C�g�Ɋ֌W�Ȃ����g�����˂�����B�^���ÂȐ��E�ł�����܂�
    //		DirectX::XMFLOAT4 bump{ 0.0f, 0.0f, 0.0f, 1.0f }; // �\�ʂ̓ʉ��̋��x�B�e�N�X�`���ƃ����Z�b�g������
    //		DirectX::XMFLOAT4 transparency{ 0.0f, 0.0f, 0.0f, 1.0f }; // ���̓��ߋ�B�l���傫���قǕs�����B
    //			   //
    //		DirectX::XMFLOAT4 specular{ 0.0f, 0.0f, 0.0f, 1.0f }; // The w component is 'shininess'.�@����_�ɓ���������������̕����ɔ��˂��鋭�x�B���ʂ̂悤�Ȍ���
    //		DirectX::XMFLOAT4 shininess{ 0.0f, 0.0f, 0.0f, 1.0f }; // ����
    //		DirectX::XMFLOAT4 reflection{ 0.0f, 0.0f, 0.0f, 1.0f }; // ����
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
    //
    //protected:
    //	GeometricMesh(ID3D11Device* device, const char* fbxFilename, bool triangulate = false, float samplingRate = 0,
    //		std::function<void(Mesh&, Mesh::Subset& subset)> draworderCallback = [](Mesh&, Mesh::Subset&) {}, bool avoidCreateComObjects = false) {}
    //	GeometricMesh(ID3D11Device* device, const char* fbxFilename, std::vector<std::string>& animationFilenames, bool triangulate, float samplingRate) {}
    //
    //	virtual ~GeometricMesh() {}
    //public:
    //	void Init(ID3D11Device* device, BOOL handedCoordinateSystem);
    //
    //	void Preparation(ID3D11DeviceContext* immediateContext, Shader shader, bool wireframe = false) {};
    //
    //protected:
    //	Microsoft::WRL::ComPtr<ID3D11Device> device;
    //	Microsoft::WRL::ComPtr<ID3D11RasterizerState>		wireframeRasterizerState;
    //	Microsoft::WRL::ComPtr<ID3D11RasterizerState>		solidRasterizerState;
public:
    GeometricMesh() {}
    virtual ~GeometricMesh() {}

    void Init(ID3D11Device* device, BOOL handedCoordinateSystem);
};

#endif // !MESH

