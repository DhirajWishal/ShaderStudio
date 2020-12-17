// Copyright 2020 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanRenderTarget.h"
#include "Graphics/Backend/Vulkan/VulkanDevice.h"

namespace Graphics
{
	namespace VulkanBackend
	{
		void VulkanRenderTargetSB3D::Initialize(GDevice* pDevice, UI32 width, UI32 height, float xOffset, float yOffset)
		{
			mSwapChain.Initialize(dynamic_cast<VulkanDevice*>(pDevice), width, height, xOffset, yOffset);
		}

		void VulkanRenderTargetSB3D::Terminate(GDevice* pDevice)
		{
		}
	}
}
