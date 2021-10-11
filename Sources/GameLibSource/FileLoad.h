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
//    TCHAR szPath[MAX_PATH]; // �f�t�H�f�B���N�g��
//    TCHAR szFile[MAX_PATH]; // �ۑ�����t�@�C����(�ۑ�����ꍇ�͂����ɏo�̓p�X�Ԃ��Ă���)
//
//    // �f�t�H�f�B���N�g���ݒ�
//    {
//        GetCurrentDirectory(MAX_PATH, szPath);
//        open_filename.lpstrInitialDir = szPath; // �����t�H���_�ʒu
//    }
//
//    // ������R�s�[(�t�@�C����)
//    {
//#ifdef UNICODE
//        ::mbstowcs(szFile, filename, (sizeof szFile) / 2);
//#else
//        ::strcpy_s(szFile, filename);
//#endif
//        open_filename.lpstrFile = szFile; // �����Ƀ_�C�A���O�{�b�N�X�ɓ����Ă镶��
//    }
//
//    // �t�B���^�[�ݒ�(�����w�肷�邱�Ƃ��ł���炵��)
//    {
//#if 1
//        std::wostringstream fil;
//        fil << extension << L"�t�@�C��(*." << extension << ")";// \0 * ." << extension << "\0";
//        std::string str = wide_to_sjis(fil.str().c_str()).c_str(); // wide��������shiftJIS�ɕϊ�
//
//        char test[MAX_PATH]{};
//        str.copy(test, str.length());
//        open_filename.lpstrFilter = TEXT(test);
//#else
//        ofn.lpstrFilter = TEXT("body�t�@�C��(.body)\0.body\0");
//#endif
//    }
//
//    // �g���q
//   open_filename.lpstrDefExt = TEXT(extension);
//
//    // title�ݒ�
//    open_filename.lpstrTitle = TEXT(title);
//
//
//    // window�\��(���s)
//    bool result = GetSaveFileName(&open_filename);
//    output_path = result ? szFile : "";
//
//
//    return result; // �ۑ�������Ȃ�������false���Ԃ����
//}
#endif // !FILELOAD

