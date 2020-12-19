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

		/**
		 * Structure containing support details for a Vulkan Swap Chain.
		 */
		struct SwapChainSupportDetails {
			VkSurfaceCapabilitiesKHR capabilities = {};			// Swap Chain capabilities.
			std::vector<VkSurfaceFormatKHR> formats = {};		// Swap Chain formats.
			std::vector<VkPresentModeKHR> presentModes = {};	// Swap Chain present modes.
		};

		/**
		 * Query swap chain support details.
		 *
		 * @param vPhysicalDevice: The physical device to be checked for.
		 * @param vSurface: The surface to be checked with.
		 * @return SwapChainSupportDetails structure.
		 */
		SwapChainSupportDetails QuerySwapChainSupportDetails(VkPhysicalDevice vPhysicalDevice, VkSurfaceKHR vSurface);

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