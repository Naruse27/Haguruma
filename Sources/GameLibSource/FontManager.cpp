#include "FontManager.h"
#include "misc.h"

std::string name;



void FontManager::TexOut(float elapsedTime, Vector2 position, Vector2 size, static const wchar_t message[], int num)
{
	HRESULT hr{ S_OK };
	static float count{ 0 };
	count += elapsedTime * 10;
	d2d1DeviceContext->BeginDraw();
	//static const wchar_t m[]{ L"海賊王を夢見る少年モンキー・D・ルフィを主人公とする、「ひとつなぎの大秘宝（ワンピース）」を巡る海洋冒険ロマン。夢への冒険・仲間たちとの友情といったテーマを前面に掲げ、バトルやギャグシーン、感動エピソードをメインとする少年漫画の王道を行く物語として人気を博している。また、長年にわたりながら深く練り込まれた壮大な世界観・巧緻な設定のストーリーも特徴。" };
	if (count > num)
	{
		count = 0;
	}
	d2d1DeviceContext->DrawTextA(message, static_cast<UINT32>(count), dwriteTextFormat.Get(), D2D1::RectF(position.x, position.y, size.x, size.y), d2dSolidColorBrush.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
	hr = d2d1DeviceContext->EndDraw();
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void FontManager::TexOut(Vector2 position, Vector2 size, static const wchar_t message[], int num)
{
	HRESULT hr{ S_OK };

	d2d1DeviceContext->BeginDraw();
	//static const wchar_t message[]{ L"海賊王を夢見る少年モンキー・D・ルフィを主人公とする、「ひとつなぎの大秘宝（ワンピース）」を巡る海洋冒険ロマン。夢への冒険・仲間たちとの友情といったテーマを前面に掲げ、バトルやギャグシーン、感動エピソードをメインとする少年漫画の王道を行く物語として人気を博している。また、長年にわたりながら深く練り込まれた壮大な世界観・巧緻な設定のストーリーも特徴。" };
	d2d1DeviceContext->DrawTextA(message, static_cast<UINT32>(num), dwriteTextFormat.Get(), D2D1::RectF(position.x, position.y, size.x, size.y), d2dSolidColorBrush.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
	hr = d2d1DeviceContext->EndDraw();
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}