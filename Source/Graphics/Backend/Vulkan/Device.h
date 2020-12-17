// Copyright 2020 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Queue.h"

namespace Graphics
{
	namespace VulkanBackend
	{
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

		/**
		 * Create a new physical device.
		 *
		 * @param vInstance: The vulkan instance.
		 * @param vSurface: The surface the physical device renders to.
		 * @param deviceExtensions: The device extensions.
		 * @return The Vulkan physical device handle.
		 */
		VkPhysicalDevice CreatePhysicalDevice(VkInstance vInstance, VkSurfaceKHR vSurface, const std::vector<const char*>& deviceExtensions);

		/**
		 * Create a new Vulkan logical device handle.
		 *
		 * @param enableValidation: Whether or not to enable validation.
		 * @param validationLayers: The validation layers to use.
		 * @param deviceExtensions: The device extensions to use.
		 * @param vPhysicalDevice: The physical device which the logical device is bound to.
		 * @param vQueue: The Vulkan queue.
		 * @return VkDevice handle.
		 */
		VkDevice CreateLogicalDevice(bool enableValidation, const std::vector<const char*>& validationLayers, const std::vector<const char*>& deviceExtensions, VkPhysicalDevice vPhysicalDevice, VulkanQueue* pQueue);
	}
}