/*******************************************************************************
* Copyright (C) 2019 - 2023, winsoft666, <winsoft666@outlook.com>.
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
*
* Expect bugs
*
* Please use and enjoy. Please let me know of any bugs/improvements
* that you have found/implemented and I will fix/incorporate them into this
* file.
*******************************************************************************/

#include "string_encode.h"
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include <windows.h>

namespace teemo {
std::wstring Utf8ToUnicode(const std::string& str) {
  std::wstring strRes;
  int iSize = ::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);

  if (iSize == 0)
    return strRes;

  wchar_t* szBuf = new (std::nothrow) wchar_t[iSize];

  if (!szBuf)
    return strRes;

  memset(szBuf, 0, iSize * sizeof(wchar_t));
  ::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, szBuf, iSize);

  strRes = szBuf;
  delete[] szBuf;

  return strRes;
}

std::string UnicodeToUtf8(const std::wstring& str) {
  std::string strRes;

  int iSize =
      ::WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, NULL, 0, NULL, NULL);

  if (iSize == 0)
    return strRes;

  char* szBuf = new (std::nothrow) char[iSize];

  if (!szBuf)
    return strRes;

  memset(szBuf, 0, iSize);

  ::WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, szBuf, iSize, NULL, NULL);

  strRes = szBuf;
  delete[] szBuf;

  return strRes;
}
}  // namespace teemo
#endif