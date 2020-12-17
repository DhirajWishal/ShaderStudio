// Copyright 2020 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "ShaderCode.h"
#include "Core/ErrorHandler/MessageBox.h"

#include <fstream>

bool ShaderCode::LoadCode(const char* pFile)
{
	std::ifstream file(pFile, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		MessageBox::IssueError(TEXT("Failed to open the shader file! \nMake sure that the provided shader path is correct."));
		return false;
	}

	UI64 size = file.tellg();
	file.seekg(0);

	mCode.resize(size);
	file.read(reinterpret_cast<char*>(mCode.data()), size);

	file.close();
	return true;
}
