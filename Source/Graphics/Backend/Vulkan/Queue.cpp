// Copyright 2020 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Queue.h"

namespace Graphics
{
	namespace VulkanBackend
	{
		bool VulkanQueue::IsComplete() const
		{
			return mGraphicsFamily.has_value() && mComputeFamily.has_value() && mTransferFamily.has_value();
		}

		VulkanQueue CreateQueue(VkPhysicalDevice vPhysicalDevice)
		{
			VulkanQueue queue = {};
			UI32 queueFamilyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(vPhysicalDevice, &queueFamilyCount, nullptr);

			std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(vPhysicalDevice, &queueFamilyCount, queueFamilies.data());

			I32 i = 0;
			for (auto itr = queueFamilies.begin(); itr != queueFamilies.end(); itr++, i++)
			{
				if (itr->queueCount > 0)
				{
					// Set graphics family.
					if (itr->queueFlags & VK_QUEUE_GRAPHICS_BIT)
						queue.mGraphicsFamily = i;

					// Set compute family.
					if (itr->queueFlags & VK_QUEUE_COMPUTE_BIT)
						queue.mComputeFamily = i;

					// Set transfer family.
					if (itr->queueFlags & VK_QUEUE_TRANSFER_BIT)
						queue.mTransferFamily = i;

					// Escape from the loop if the queues were found.
					if (queue.IsComplete())
						break;
				}
			}

			return queue;
		}

		void GetQueues(VkDevice vLogicalDevice, VulkanQueue* pQueue)
		{
			vkGetDeviceQueue(vLogicalDevice, pQueue->mGraphicsFamily.value(), 0, &pQueue->vGraphicsQueue);
			vkGetDeviceQueue(vLogicalDevice, pQueue->mComputeFamily.value(), 0, &pQueue->vComputeQueue);
			vkGetDeviceQueue(vLogicalDevice, pQueue->mTransferFamily.value(), 0, &pQueue->vTransferQueue);
		}
	}
}