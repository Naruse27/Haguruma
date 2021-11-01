#ifndef MODEL
#define MODEL

#include <map>
#include <memory>
#include <vector>
//#include "Mesh.h"
#include "SkinnedMesh.h"
#include "Shader.h"

class Model
{
public:
	Model(ID3D11Device* device, const char* fbxFilename, bool triangulate, float samplingRate, bool avoidCreateComObjects, BOOL handedCoordinateSystem = false);
	//Model(ID3D11Device* device, const char* fbxFilename, std::vector<std::string>& animationFilenames, bool triangulate, float samplingRates, int index = 0);
	Model(ID3D11Device* device, std::vector<const char*> fbxFilename, bool triangulate, float samplingRate, BOOL handedCoordinateSystem = false);
	~Model();

	// �s��v�Z
	void UpdateTransform(const DirectX::XMFLOAT4X4& transform);

	// �A�j���[�V�����Đ�
	void PlayAnimation(int index);

	// �A�j���[�V�����X�V
	void UpdateAnimation(float elapsedTime, bool loop);

	//// �A�j���[�V�����X�V����
	//void UpdateAnimation(float elapsedTime);
	//
	//// �A�j���[�V�����Đ�
	//void PlayAnimation(int index, bool loop, float blendSeconds = 0.2f);
	//
	//// �A�j���[�V�����Đ�����
	//bool IsPlayAnimation() const;



	// �A�j���[�V�����ǂݍ��݊i�[
	void AnimationInput(const char* fbxFilename, int index);

	void Preparation(ID3D11DeviceContext* immediateContext, Shader shader, bool wireframe);
	void Render(ID3D11DeviceContext* immediateContext, const Vector4 materialColor = { 1.0f, 1.0f, 1.0f, 1.0f });// , const Animation::Keyframe* keyframe = nullptr);

	const std::vector<SkinnedMesh::Mesh>& GetMeshs() const
	{
		auto it = model.find(this->index);
		return it->second.GetMeshs();
	}
	const std::vector<SkinnedMesh::Animation>& GetAnimation()
	{
		auto it = model.find(this->index);
		return it->second.GetAnimation();
	}
	const SkinnedMesh::Animation::Keyframe& GetKeyframe() { return *keyframe; }

	//const int& GetClipIndex() { return clipIndex; }
	//const int& GetFrameIndex() { return clipIndex; }

private:
	bool JudgeInCamera(DirectX::XMFLOAT3 position);

private:
	std::map<int, SkinnedMesh> model;
	DirectX::XMFLOAT4X4 transform = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	int index = 0; // model��state

	SkinnedMesh::Animation::Keyframe* keyframe = nullptr;

	int currentAnimationIndex = -1;
	float currentAnimationSeconds = 0.0f;
	bool animationLoopFlag = false;
	bool animationEndFlag = false;
	float animationBlendTime = 0.0f;
	float animationBlendSeconds = 0.0f;

	//int clipIndex = 0;
	//int frameIndex = 0;

};

#endif // !MODEL

