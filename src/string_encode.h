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

#ifndef TEEMO_STRING_ENCODE_H_
#define TEEMO_STRING_ENCODE_H_
#pragma once

#include <string>

namespace teemo {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
std::wstring Utf8ToUnicode(const std::string& str);
std::string UnicodeToUtf8(const std::wstring& str);
#endif
}  // namespace teemo

#endif