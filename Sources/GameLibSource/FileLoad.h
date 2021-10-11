#ifndef FILELOAD
#define FILELOAD

#include <string>
#include  <tchar.h>
#include <stdlib.h>
#include <filesystem>
#include <sstream>
#include <windef.h>
#include <windows.h>

#include "strconv.h"
#include <winnt.h>

static OPENFILENAME open_filename{};


void file_initilaize(HWND hwnd)
{
    open_filename.lStructSize = sizeof(OPENFILENAME);
    open_filename.hwndOwner = hwnd;
    open_filename.nMaxFile = MAX_PATH;
    open_filename.Flags = OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT;
}

//bool open_explorer(const char* filename, const char* extension, const char* title, std::string& output_path)
//{
//    TCHAR szPath[MAX_PATH]; // デフォディレクトリ
//    TCHAR szFile[MAX_PATH]; // 保存するファイル名(保存する場合はここに出力パス返ってくる)
//
//    // デフォディレクトリ設定
//    {
//        GetCurrentDirectory(MAX_PATH, szPath);
//        open_filename.lpstrInitialDir = szPath; // 初期フォルダ位置
//    }
//
//    // 文字列コピー(ファイル名)
//    {
//#ifdef UNICODE
//        ::mbstowcs(szFile, filename, (sizeof szFile) / 2);
//#else
//        ::strcpy_s(szFile, filename);
//#endif
//        open_filename.lpstrFile = szFile; // 初期にダイアログボックスに入ってる文字
//    }
//
//    // フィルター設定(複数指定することもできるらしい)
//    {
//#if 1
//        std::wostringstream fil;
//        fil << extension << L"ファイル(*." << extension << ")";// \0 * ." << extension << "\0";
//        std::string str = wide_to_sjis(fil.str().c_str()).c_str(); // wide文字からshiftJISに変換
//
//        char test[MAX_PATH]{};
//        str.copy(test, str.length());
//        open_filename.lpstrFilter = TEXT(test);
//#else
//        ofn.lpstrFilter = TEXT("bodyファイル(.body)\0.body\0");
//#endif
//    }
//
//    // 拡張子
//   open_filename.lpstrDefExt = TEXT(extension);
//
//    // title設定
//    open_filename.lpstrTitle = TEXT(title);
//
//
//    // window表示(実行)
//    bool result = GetSaveFileName(&open_filename);
//    output_path = result ? szFile : "";
//
//
//    return result; // 保存押されなかったらfalseが返される
//}
#endif // !FILELOAD

