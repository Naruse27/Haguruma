#ifndef SCENELOADING
#define SCENELOADING

#include "Scene.h"

// ���[�f�B���O�V�[��
class SceneLoading : public Scene
{
public:
	SceneLoading(Scene* nextScene) : nextScene(nextScene) {}
	~SceneLoading() override {}

	// ������
	virtual void Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext) override;

	// �I����
	virtual void Finalize() override;

	// �X�V����
	virtual void Update(float elapsedTime) override;

	// �`�揈��
	virtual void Render(float elapsedTime) override;

private:
	// ���[�f�B���O�X���b�h
	static void LoadingThread(SceneLoading* scene);

private:
	float angle = 0.0f;
	Scene* nextScene = nullptr;
};


#endif // !SCENELOADING

