#include "Graphics.h"

void Graphics::Initiraize(HWND hWnd)
{
    // ��ʂ̃T�C�Y���擾����B
    RECT rc;
    GetClientRect(hWnd, &rc);
    UINT screenWidth = rc.right - rc.left;
    UINT screenHeight = rc.bottom - rc.top;

    this->screenWidth = static_cast<float>(screenWidth);
    this->screenHeight = static_cast<float>(screenHeight);
}