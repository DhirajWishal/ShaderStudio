// Copyright 2020 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Types/DataTypes.h"

enum class ShaderCodeType : UI8 {
	UNDEFINED,
	GLSL,
	HLSL,
	SPIR_V
};

class ShaderCode {
public:
	ShaderCode() {}
	~ShaderCode() {}

	bool LoadCode(const char* pFile);

private:
	std::vector<UI32> mCode;
	ShaderCodeType mType = ShaderCodeType::UNDEFINED;
};