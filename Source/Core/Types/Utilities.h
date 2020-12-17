// Copyright 2020 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>

/**
 * Convert wchar string to a char string.
 *
 * @param string: The const wchar* string.
 * @return std::string object.
 */
std::string WStringToString(const std::wstring& string);

/**
 * Convert char string to a wchar string.
 *
 * @param string: The const char* string.
 * @return std::wstring object.
 */
std::wstring StringToWString(const std::string& string);