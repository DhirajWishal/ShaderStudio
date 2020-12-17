// Copyright 2020 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Types/DataTypes.h"
#include <vulkan/vulkan.h>

namespace Graphics
{
	namespace VulkanBackend
	{
		class VulkanDevice;

		class SwapChain {
		public:
			SwapChain() {}
			~SwapChain() {}

			void Initialize(VulkanDevice* pDevice, UI32 width, UI32 height, float xOffset, float yOffset);
			void Terminate(VulkanDevice* pDevice);

		private:
			std::vector<VkImage> vImages;
			std::vector<VkImageView> vImageViews;

			VkSwapchainKHR vSwapChain = VK_NULL_HANDLE;
			VkFormat vFormat = VkFormat::VK_FORMAT_UNDEFINED;
		};
	}
}