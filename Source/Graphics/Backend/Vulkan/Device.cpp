// Copyright 2020 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Device.h"
#include "Macros.h"
#include "Queue.h"
#include "Core/ErrorHandler/Logger.h"

#include <GLFW/glfw3.h>
#include <set>

namespace Graphics
{
	namespace VulkanBackend
	{
		SwapChainSupportDetails QuerySwapChainSupportDetails(VkPhysicalDevice vPhysicalDevice, VkSurfaceKHR vSurface)
		{
			SwapChainSupportDetails supportDetails = {};
			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vPhysicalDevice, vSurface, &supportDetails.capabilities);

			UI32 formatCount = 0;
			vkGetPhysicalDeviceSurfaceFormatsKHR(vPhysicalDevice, vSurface, &formatCount, nullptr);

			if (formatCount != 0)
			{
				supportDetails.formats.resize(formatCount);
				vkGetPhysicalDeviceSurfaceFormatsKHR(vPhysicalDevice, vSurface, &formatCount, supportDetails.formats.data());
			}

			UI32 presentModeCount = 0;
			vkGetPhysicalDeviceSurfacePresentModesKHR(vPhysicalDevice, vSurface, &presentModeCount, nullptr);

			if (presentModeCount != 0)
			{
				supportDetails.presentModes.resize(presentModeCount);
				vkGetPhysicalDeviceSurfacePresentModesKHR(vPhysicalDevice, vSurface, &presentModeCount, supportDetails.presentModes.data());
			}

			return supportDetails;
		}

		/**
		 * Get the max usable sample count of a physical device.
		 *
		 * @param vPhysicalDevice: The physical device.
		 * @return VkSampleCountFlags enum.
		 */
		VkSampleCountFlags GetMaxUsableSamples(VkPhysicalDevice vPhysicalDevice)
		{
			VkPhysicalDeviceProperties physicalDeviceProperties;
			vkGetPhysicalDeviceProperties(vPhysicalDevice, &physicalDeviceProperties);

			VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
			if (counts & VK_SAMPLE_COUNT_64_BIT) return VK_SAMPLE_COUNT_64_BIT;
			if (counts & VK_SAMPLE_COUNT_32_BIT) return VK_SAMPLE_COUNT_32_BIT;
			if (counts & VK_SAMPLE_COUNT_16_BIT) return VK_SAMPLE_COUNT_16_BIT;
			if (counts & VK_SAMPLE_COUNT_8_BIT) return VK_SAMPLE_COUNT_8_BIT;
			if (counts & VK_SAMPLE_COUNT_4_BIT) return VK_SAMPLE_COUNT_4_BIT;
			if (counts & VK_SAMPLE_COUNT_2_BIT) return VK_SAMPLE_COUNT_2_BIT;

			return VK_SAMPLE_COUNT_1_BIT;
		}

		/**
		 * Check if the extensions are supported for the Physical Device.
		 *
		 * @param vPhysicalDevice: The physical device to be checked for.
		 * @param deviceExtensions: The device extensions to be checked with.
		 * @return Boolean value.
		 */
		bool CheckDeviceExtensionSupport(VkPhysicalDevice vPhysicalDevice, std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME })
		{
			UI32 extensionCount = 0;
			vkEnumerateDeviceExtensionProperties(vPhysicalDevice, nullptr, &extensionCount, nullptr);

			std::vector<VkExtensionProperties> availableExtensions(extensionCount);
			vkEnumerateDeviceExtensionProperties(vPhysicalDevice, nullptr, &extensionCount, availableExtensions.data());

			std::set<String> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

			for (const VkExtensionProperties& extension : availableExtensions)
				requiredExtensions.erase(extension.extensionName);

			return requiredExtensions.empty();
		}

		/**
		 * Check if a physical device is suitable to use.
		 *
		 * @param vDevice: The physical device to be checked.
		 * @param vSurface: The surface the device will be using.
		 * @param deviceExtensions: The physical device extensions.
		 * @return Boolean value.
		 */
		bool IsPhysicalDeviceSuitable(VkPhysicalDevice vDevice, VkSurfaceKHR vSurface, const std::vector<const char*>& deviceExtensions)
		{
			VulkanQueue _queue = CreateQueue(vDevice);

			bool extensionsSupported = CheckDeviceExtensionSupport(vDevice, deviceExtensions);
			bool swapChainAdequate = false;
			if (extensionsSupported)
			{
				SwapChainSupportDetails swapChainSupport = QuerySwapChainSupportDetails(vDevice, vSurface);
				swapChainAdequate = (!swapChainSupport.formats.empty()) && (!swapChainSupport.presentModes.empty());
			}

			VkPhysicalDeviceFeatures supportedFeatures;
			vkGetPhysicalDeviceFeatures(vDevice, &supportedFeatures);

			return _queue.IsComplete()
				&& extensionsSupported
				&& swapChainAdequate
				&& supportedFeatures.samplerAnisotropy;
		}

		VkPhysicalDevice CreatePhysicalDevice(VkInstance vInstance, VkSurfaceKHR vSurface, const std::vector<const char*>& deviceExtensions)
		{
			UI32 deviceCount = 0;
			vkEnumeratePhysicalDevices(vInstance, &deviceCount, nullptr);

			if (deviceCount == 0)
			{
				Logger::LogError(TEXT("Failed to find GPUs with Vulkan support!"));
				return VkPhysicalDevice();
			}

			std::vector<VkPhysicalDevice> devices(deviceCount);
			vkEnumeratePhysicalDevices(vInstance, &deviceCount, devices.data());

			VkPhysicalDevice vPhysicalDevice = VK_NULL_HANDLE;
			VkPhysicalDeviceProperties vPhysicalDeviceProperties = {};
			// Iterate through all the candidate devices and find the best device.
			for (const VkPhysicalDevice& device : devices)
			{
				if (IsPhysicalDeviceSuitable(device, vSurface, deviceExtensions))
				{
					vkGetPhysicalDeviceProperties(device, &vPhysicalDeviceProperties);

					if (vPhysicalDeviceProperties.deviceType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
						vPhysicalDevice = device;
					else if (vPhysicalDeviceProperties.deviceType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
						vPhysicalDevice = device;
					else
						vPhysicalDevice = device;

					break;
				}

				vPhysicalDevice = VK_NULL_HANDLE;
			}

			//  Check if a physical device was found.
			if (vPhysicalDevice == VK_NULL_HANDLE)
			{
				Logger::LogError(TEXT("A suitable physical device was not found!"));
				return VkPhysicalDevice();
			}

#ifdef SS_DEBUG
			printf("\n\t---------- VULKAN PHYSICAL DEVICE INFO ----------\n");
			printf("API Version: %I32d\n", vPhysicalDeviceProperties.apiVersion);
			printf("Driver Version: %I32d\n", vPhysicalDeviceProperties.driverVersion);
			printf("Vendor ID: %I32d\n", vPhysicalDeviceProperties.vendorID);
			printf("Device ID: %I32d\n", vPhysicalDeviceProperties.deviceID);

			switch (vPhysicalDeviceProperties.deviceType)
			{
			case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_OTHER:
				printf("Device Type: Other GPU\n");
				break;
			case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
				printf("Device Type: Discrete GPU (dedicated)\n");
				break;
			case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
				printf("Device Type: Integrated GPU (onboard)\n");
				break;
			case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
				printf("Device Type: Virtual GPU\n");
				break;
			case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_CPU:
				printf("Device Type: CPU\n");
				break;
			default:
				printf("Device Type: *UNIDENTIFIED\n");
				break;
			}

			printf("Device Name: %s\n", vPhysicalDeviceProperties.deviceName);
			printf("\t-------------------------------------------------\n\n");

#endif	// SS_DEBUG

			return vPhysicalDevice;
		}

		VkDevice CreateLogicalDevice(bool enableValidation, const std::vector<const char*>& validationLayers, const std::vector<const char*>& deviceExtensions, VkPhysicalDevice vPhysicalDevice, VulkanQueue* pQueue)
		{
			std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
			std::set<UI32> uniqueQueueFamilies = {
				pQueue->mGraphicsFamily.value(),
				pQueue->mComputeFamily.value(),
				pQueue->mTransferFamily.value()
			};

			float queuePriority = 1.0f;
			for (UI32 queueFamily : uniqueQueueFamilies)
			{
				VkDeviceQueueCreateInfo queueCreateInfo = {};
				queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				queueCreateInfo.queueFamilyIndex = queueFamily;
				queueCreateInfo.queueCount = 1;
				queueCreateInfo.pQueuePriorities = &queuePriority;
				queueCreateInfos.push_back(queueCreateInfo);
			}

			// Physical device features.
			VkPhysicalDeviceFeatures deviceFeatures = {};
			deviceFeatures.samplerAnisotropy = VK_TRUE;
			deviceFeatures.sampleRateShading = VK_TRUE; // Enable sample shading feature for the device

			// Device create info.
			VkDeviceCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			createInfo.queueCreateInfoCount = static_cast<UI32>(queueCreateInfos.size());
			createInfo.pQueueCreateInfos = queueCreateInfos.data();
			createInfo.pEnabledFeatures = &deviceFeatures;
			createInfo.enabledExtensionCount = static_cast<UI32>(deviceExtensions.size());
			createInfo.ppEnabledExtensionNames = deviceExtensions.data();

			if (enableValidation)
			{
				createInfo.enabledLayerCount = static_cast<UI32>(validationLayers.size());
				createInfo.ppEnabledLayerNames = validationLayers.data();
			}
			else
			{
				createInfo.enabledLayerCount = 0;
			}

			// Create the logical device.
			VkDevice vLogicalDevice = VK_NULL_HANDLE;
			VK_ASSERT(vkCreateDevice(vPhysicalDevice, &createInfo, nullptr, &vLogicalDevice), "Failed to create logical device!");

			// Create queues.
			{
				// Graphics Queue.
				vkGetDeviceQueue(vLogicalDevice, pQueue->mGraphicsFamily.value(), 0, &pQueue->vGraphicsQueue);

				// Compute Queue.
				vkGetDeviceQueue(vLogicalDevice, pQueue->mComputeFamily.value(), 0, &pQueue->vComputeQueue);

				// Transfer Queue.
				vkGetDeviceQueue(vLogicalDevice, pQueue->mTransferFamily.value(), 0, &pQueue->vTransferQueue);
			}

			return vLogicalDevice;
		}
	}
}
