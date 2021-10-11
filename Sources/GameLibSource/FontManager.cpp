#include "FontManager.h"
#include "misc.h"

std::string name;



void FontManager::TexOut(float elapsedTime, Vector2 position, Vector2 size, static const wchar_t message[], int num)
{
	HRESULT hr{ S_OK };
	static float count{ 0 };
	count += elapsedTime * 10;
	d2d1DeviceContext->BeginDraw();
	//static const wchar_t m[]{ L"�C�����𖲌��鏭�N�����L�[�ED�E���t�B����l���Ƃ���A�u�ЂƂȂ��̑���i�����s�[�X�j�v������C�m�`�����}���B���ւ̖`���E���Ԃ����Ƃ̗F��Ƃ������e�[�}��O�ʂɌf���A�o�g����M���O�V�[���A�����G�s�\�[�h�����C���Ƃ��鏭�N����̉������s������Ƃ��Đl�C�𔎂��Ă���B�܂��A���N�ɂ킽��Ȃ���[�����荞�܂ꂽ�s��Ȑ��E�ρE�I�k�Ȑݒ�̃X�g�[���[�������B" };
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
	//static const wchar_t message[]{ L"�C�����𖲌��鏭�N�����L�[�ED�E���t�B����l���Ƃ���A�u�ЂƂȂ��̑���i�����s�[�X�j�v������C�m�`�����}���B���ւ̖`���E���Ԃ����Ƃ̗F��Ƃ������e�[�}��O�ʂɌf���A�o�g����M���O�V�[���A�����G�s�\�[�h�����C���Ƃ��鏭�N����̉������s������Ƃ��Đl�C�𔎂��Ă���B�܂��A���N�ɂ킽��Ȃ���[�����荞�܂ꂽ�s��Ȑ��E�ρE�I�k�Ȑݒ�̃X�g�[���[�������B" };
	d2d1DeviceContext->DrawTextA(message, static_cast<UINT32>(num), dwriteTextFormat.Get(), D2D1::RectF(position.x, position.y, size.x, size.y), d2dSolidColorBrush.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
	hr = d2d1DeviceContext->EndDraw();
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}