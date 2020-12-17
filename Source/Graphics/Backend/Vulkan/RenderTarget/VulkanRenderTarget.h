// Copyright 2020 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Graphics/Core/GRenderTarget.h"
#include "SwapChain.h"

namespace Graphics
{
	namespace VulkanBackend
	{
		/**
		 * Vulkan Render Target SB3D (Screen Bound 3D) object.
		 */
		class VulkanRenderTargetSB3D : public GRenderTarget {
		public:
			VulkanRenderTargetSB3D() : GRenderTarget(RenderTargetType::SCREEN_BOUND_3D) {}
			~VulkanRenderTargetSB3D() {}

			virtual void Initialize(GDevice* pDevice, UI32 width, UI32 height, float xOffset, float yOffset) override final;
			virtual void Terminate(GDevice* pDevice) override final;

		private:
			SwapChain mSwapChain = {};
		};
	}
}