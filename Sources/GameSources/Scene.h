#ifndef SCENE
#define SCENE

#include <d3d11.h>

// �V�[��
class Scene
{
public:
	Scene() {}
	virtual ~Scene() {}

	// ������
	virtual void Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext) = 0;

	// �I����
	virtual void Finalize() = 0;

	// �X�V����
	virtual void Update(float elapsedTime) = 0;

	// �`�揈��
	virtual void Render(float elapsedTime) = 0;

	// �����������Ă��邩
	bool IsReady() const { return ready; }

	// ���������ݒ�
	void SetReady() { ready = true; }

protected:
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;

private:
	bool ready = false;
};

#endif // !SCENE

