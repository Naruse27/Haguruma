#ifndef GRAPHICS
#define GRAPHICS

#include <mutex>
#include <memory>
#include <d3d11.h>
#include <wrl.h>

#define DISALLOW_COPY_AND_ASSIGN(ClassName) \
ClassName(const ClassName&); \
ClassName& operator=(const ClassName&); \
ClassName(const ClassName&&); \
ClassName& operator=(const ClassName&&);

class Graphics
{
private:
	Graphics() {};
	~Graphics() {};
	static Graphics* instance;
public:
	void Initiraize(HWND hWnd);

	// スクリーン幅取得
	float GetScreenWidth() const { return screenWidth; }

	// スクリーン高さ取得
	float GetScreenHeight() const { return screenHeight; }

private:
	float screenWidth = 0.0f;
	float screenHeight = 0.0f;
};

#endif // !GRAPHICS

