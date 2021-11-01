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

	void TimerAdd(float elapsedTime) { timer += elapsedTime; }
	const float& GetTimet() const { return timer; }
	void TimerClear() { timer = 0.0f; }

private:
	Scene* currentScene = nullptr; // ���݂̃V�[��
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;

	float timer = 0.0f;
};

#endif // !SCENEMANAGER

