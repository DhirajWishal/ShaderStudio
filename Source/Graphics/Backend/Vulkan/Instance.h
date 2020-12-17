// Copyright 2020 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <vector>
#include <vulkan/vulkan.h>

namespace Graphics
{
	namespace VulkanBackend
	{
		/**
		 * Create a new instance.
		 *
		 * @param enableValidation: Whether or not to enable API validation.
		 * @param validationLayers: The validation layers to use.
		 * @return The vulkan instance handle.
		 */
		VkInstance CreateInstance(bool enableValidation, std::vector<const char*> validationLayers);

		/**
		 * Destroy instance.
		 *
		 * @param vInstance: The vulkan instance.
		 */
		void DestroyInstance(VkInstance vInstance);

		/**
		 * Create a new debug messenger.
		 *
		 * @param vInstance: The vulkan instance handle.
		 * @return The vulkan debug utils messenger EXT handle.
		 */
		VkDebugUtilsMessengerEXT CreateDebugMessenger(VkInstance vInstance);

		/**
		 * Destroy a created debug messenger.
		 *
		 * @param vInstance: The vulkan instance.
		 * @param vDebugMessenger: The vulkan debug messenger handle.
		 */
		void DestroyDebugMessenger(VkInstance vInstance, VkDebugUtilsMessengerEXT vDebugMessenger);
	}
}