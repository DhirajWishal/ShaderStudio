// Copyright 2020 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Types/DataTypes.h"

#include <optional>
#include <vulkan/vulkan.h>

namespace Graphics
{
	namespace VulkanBackend
	{
		struct VulkanQueue {
			std::optional<UI32> mGraphicsFamily;
			std::optional<UI32> mComputeFamily;
			std::optional<UI32> mTransferFamily;

			VkQueue vGraphicsQueue = VK_NULL_HANDLE;
			VkQueue vComputeQueue = VK_NULL_HANDLE;
			VkQueue vTransferQueue = VK_NULL_HANDLE;

			bool IsComplete() const;
		};

		/**
		 * Create a Vulkan queue.
		 * 
		 * @param vPhysicalDevice: The physical device to create the queue.
		 * @return The Vulkan Queue struct.
		 */
		VulkanQueue CreateQueue(VkPhysicalDevice vPhysicalDevice);

		/**
		 * Get the queues from the logical device.
		 * 
		 * @param vLogicalDevice: The logical device.
		 * @param pQueue: The queue pointer.
		 */
		void GetQueues(VkDevice vLogicalDevice, VulkanQueue* pQueue);
	}
}