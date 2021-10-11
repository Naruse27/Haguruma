#ifndef SCENEMANAGER
#define SCENEMANAGER

#include "Scene.h"

// �V�[���}�l�[�W���[
class SceneManager
{
private:
	SceneManager() {}
	~SceneManager() {}

public:
	static SceneManager& Instance() // �V���O���g��
	{
		static SceneManager instance;
		return instance;
	}

	void Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	// �X�V����
	void Update(float elapsedTime);

	// �`�揈��
	void Render(float elapsedTime);

	// �N���A
	void Clear();

	// �V�[���؂�ւ�
	void ChangeScene(Scene* scene);

private:
	Scene* currentScene = nullptr; // ���݂̃V�[��
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;

};

#endif // !SCENEMANAGER

