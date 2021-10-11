#include "Graphics.h"

void Graphics::Initiraize(HWND hWnd)
{
    // 画面のサイズを取得する。
    RECT rc;
    GetClientRect(hWnd, &rc);
    UINT screenWidth = rc.right - rc.left;
    UINT screenHeight = rc.bottom - rc.top;

    this->screenWidth = static_cast<float>(screenWidth);
    this->screenHeight = static_cast<float>(screenHeight);
}