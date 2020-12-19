// Copyright 2020 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanDevice.h"
#include "Instance.h"
#include "Window.h"
#include "Macros.h"

#include "RenderTarget/VulkanRenderTarget.h"

#include "Core/Types/Utilities.h"

#include <GLFW/glfw3.h>
#include <set>

namespace Graphics
{
	namespace VulkanBackend
	{
		namespace _Helpers
		{
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
		}

		void VulkanDevice::CreateWindow(UI32 width, UI32 height, const char* pTitle)
		{
			// Setup GLFW context.
			SetupGLFW();

			pWindow = new VulkanWindow();
			pWindow->Initialize(width, height, pTitle);
		}

		void VulkanDevice::DestroyWindow()
		{
			pWindow->Terminate();
			delete pWindow;
		}

		void VulkanDevice::Initialize(bool enableValidation)
		{
			if (enableValidation)
				INSERT_INTO_VECTOR(mValidationLayers, "VK_LAYER_KHRONOS_validation");

			// Create the instance.
			vInstance = CreateInstance(enableValidation, mValidationLayers);

			// Create the debug messenger.
			if (enableValidation)
				vDebugMessenger = CreateDebugMessenger(vInstance);

			// Create the surface.
			CreateSurface();

			std::vector<const char*> deviceExtensions;
			INSERT_INTO_VECTOR(deviceExtensions, VK_KHR_SWAPCHAIN_EXTENSION_NAME);

			// Create physical device.
			CreatePhysicalDevice(deviceExtensions);

			// Get swap chain support details.
			vSwapChainSupportDetails = QuerySwapChainSupportDetails(vPhysicalDevice, vSurface);

			QuerySurfaceCapabilities();
			GetMaxSupportedSampleCount();

			// Create queue.
			vQueue = CreateQueue(vPhysicalDevice);

			// Create logical device.
			CreateLogicalDevice(deviceExtensions);
		}

		void VulkanDevice::Terminate()
		{
			// Destroy logical device.
			vkDestroyDevice(vLogicalDevice, nullptr);

			// Terminate the debug messenger if created.
			if (vDebugMessenger)
				DestroyDebugMessenger(vInstance, vDebugMessenger);

			DestroySurface();

			DestroyInstance(vInstance);

			if (pWindow)
				DestroyWindow();

			// Destroy GLFW context.
			TerminateGLFW();
		}

		void VulkanDevice::BeginDraw()
		{
			pWindow->PollInputs();
		}

		void VulkanDevice::Update()
		{
		}

		void VulkanDevice::EndDraw()
		{
		}

		GRenderTarget* VulkanDevice::CreateRenderTarget(RenderTargetType type, UI32 width, UI32 height, float xOffset, float yOffset)
		{
			switch (type)
			{
			case Graphics::RenderTargetType::SCREEN_BOUND_2D:
				break;
			case Graphics::RenderTargetType::SCREEN_BOUND_3D:
			{
				VulkanRenderTargetSB3D* pRT = new VulkanRenderTargetSB3D();
				pRT->Initialize(this, width, height, xOffset, yOffset);

				return pRT;
			}
			case Graphics::RenderTargetType::OFF_SCREEN_2D:
				break;
			case Graphics::RenderTargetType::OFF_SCREEN_3D:
				break;
			default:
				break;
			}

			return nullptr;
		}

		void VulkanDevice::DestroyRenderTarget(GRenderTarget* pRenderTarget)
		{
			pRenderTarget->Terminate(this);
			delete pRenderTarget;
		}

		UI32 VulkanDevice::GetMaxFrameBufferCount() const
		{
			UI32 bufferCount = vSwapChainSupportDetails.capabilities.minImageCount + 1;
			if (vSwapChainSupportDetails.capabilities.maxImageCount > 0
				&& bufferCount > vSwapChainSupportDetails.capabilities.maxImageCount)
				bufferCount = vSwapChainSupportDetails.capabilities.maxImageCount;

			return bufferCount;
		}

		/**
		 * Error callback function for GLFW.
		 *
		 * @param errorCode: The error code.
		 * @param pDescription: The error description.
		 */
		static void GLFWErrorCallback(I32 errorCode, const char* pDescription)
		{
			Logger::LogError((TEXT("GLFW -> ") + StringToWString(pDescription)).c_str());
		}

		void VulkanDevice::SetupGLFW()
		{
			// Set the error callback.
			glfwSetErrorCallback(GLFWErrorCallback);

			glfwInit();
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
		}

		void VulkanDevice::TerminateGLFW()
		{
			glfwTerminate();
		}

		void VulkanDevice::CreateSurface()
		{
			VK_ASSERT(glfwCreateWindowSurface(vInstance, dynamic_cast<VulkanWindow*>(pWindow)->GetWindowHandle(), nullptr, &vSurface), "Failed to create the Vulkan Surface!");
		}

		void VulkanDevice::DestroySurface()
		{
			vkDestroySurfaceKHR(vInstance, vSurface, nullptr);
		}

		void VulkanDevice::CreatePhysicalDevice(const std::vector<const char*>& deviceExtensions)
		{
			UI32 deviceCount = 0;
			vkEnumeratePhysicalDevices(vInstance, &deviceCount, nullptr);

			if (deviceCount == 0)
			{
				Logger::LogError(TEXT("Failed to find GPUs with Vulkan support!"));
				return;
			}

			std::vector<VkPhysicalDevice> devices(deviceCount);
			vkEnumeratePhysicalDevices(vInstance, &deviceCount, devices.data());

			// Iterate through all the candidate devices and find the best device.
			for (const VkPhysicalDevice& device : devices)
			{
				if (_Helpers::IsPhysicalDeviceSuitable(device, vSurface, deviceExtensions))
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
			}

			//  Check if a physical device was found.
			if (vPhysicalDevice == VK_NULL_HANDLE)
			{
				Logger::LogError(TEXT("A suitable physical device was not found!"));
				return;
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
		}

		void VulkanDevice::CreateLogicalDevice(const std::vector<const char*>& deviceExtensions)
		{
			std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
			std::set<UI32> uniqueQueueFamilies = {
				vQueue.mGraphicsFamily.value(),
				vQueue.mComputeFamily.value(),
				vQueue.mTransferFamily.value()
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
			createInfo.enabledLayerCount = static_cast<UI32>(mValidationLayers.size());
			createInfo.ppEnabledLayerNames = mValidationLayers.data();

			// Create the logical device.
			VK_ASSERT(vkCreateDevice(vPhysicalDevice, &createInfo, nullptr, &vLogicalDevice), "Failed to create logical device!");
		}

		void VulkanDevice::QuerySurfaceCapabilities()
		{
			VK_ASSERT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vPhysicalDevice, vSurface, &vSurfaceCapabilities), "Failed to get the surface capabilities!");
		}

		void VulkanDevice::GetMaxSupportedSampleCount()
		{
			vSampleCount = vPhysicalDeviceProperties.limits.framebufferColorSampleCounts & vPhysicalDeviceProperties.limits.framebufferDepthSampleCounts;

			if (vSampleCount & VK_SAMPLE_COUNT_64_BIT) vSampleCount = VK_SAMPLE_COUNT_64_BIT;
			else if (vSampleCount & VK_SAMPLE_COUNT_32_BIT) vSampleCount = VK_SAMPLE_COUNT_32_BIT;
			else if (vSampleCount & VK_SAMPLE_COUNT_16_BIT) vSampleCount = VK_SAMPLE_COUNT_16_BIT;
			else if (vSampleCount & VK_SAMPLE_COUNT_8_BIT) vSampleCount = VK_SAMPLE_COUNT_8_BIT;
			else if (vSampleCount & VK_SAMPLE_COUNT_4_BIT) vSampleCount = VK_SAMPLE_COUNT_4_BIT;
			else if (vSampleCount & VK_SAMPLE_COUNT_2_BIT) vSampleCount = VK_SAMPLE_COUNT_2_BIT;
			else vSampleCount = VK_SAMPLE_COUNT_1_BIT;
		}
	}
}
