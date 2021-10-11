#ifndef FONTMANAGER
#define FONTMANAGER

#include <d3d11_1.h>
#include <d2d1_1.h>
#include <dwrite.h>
#pragma comment(lib,"d2d1.lib")
#pragma comment(lib,"dwrite.lib")

#include <wrl.h>
#include <string>

#include "Vector.h"

class FontManager
{
public:
	FontManager() {};
	~FontManager() {};

	static FontManager& Instance() // ƒVƒ“ƒOƒ‹ƒgƒ“
	{
		static FontManager instance;
		return instance;
	}

	Microsoft::WRL::ComPtr<ID2D1DeviceContext>& GetID2D1DeviceContext() { return d2d1DeviceContext; }
	Microsoft::WRL::ComPtr<IDWriteTextFormat>& GetIDWriteTextFormat() { return dwriteTextFormat; }
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>& GetID2D1SolidColorBrush() { return d2dSolidColorBrush; }

	void TexOut(float elapsedTime, Vector2 position, Vector2 size, static const wchar_t message[], int num);
	void TexOut(Vector2 position, Vector2 size, static const wchar_t message[], int num);

private:
	Microsoft::WRL::ComPtr<ID2D1DeviceContext> d2d1DeviceContext;
	Microsoft::WRL::ComPtr<IDWriteTextFormat> dwriteTextFormat;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> d2dSolidColorBrush;
};


#endif // !FONTMANAGER