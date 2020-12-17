// Copyright 2020 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Types/DataTypes.h"

namespace Graphics
{
	class GDevice;

	enum class RenderTargetType : UI8 {
		SCREEN_BOUND_2D,
		SCREEN_BOUND_3D,
		OFF_SCREEN_2D,
		OFF_SCREEN_3D,
	};

	class GRenderTarget {
	public:
		GRenderTarget(RenderTargetType type) : mType(type) {}
		virtual ~GRenderTarget() {}

		virtual void Initialize(GDevice* pDevice, UI32 width, UI32 height, float xOffset, float yOffset) {}
		virtual void Terminate(GDevice* pDevice) {}

		//virtual void AddDrawEntity(VertexBufferObject vertexData, IndexBufferObject indexData, std::vector<Shaders> shaders) {}

	protected:
		RenderTargetType mType = RenderTargetType::SCREEN_BOUND_3D;
	};
}