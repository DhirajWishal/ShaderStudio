// Copyright 2020 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanDevice.h"
#include "Instance.h"
#include "Window.h"
#include "Macros.h"
#include "Device.h"

#include "RenderTarget/VulkanRenderTarget.h"

#include "Core/Types/Utilities.h"

#include <GLFW/glfw3.h>

namespace Graphics
{
	namespace VulkanBackend
	{
		void VulkanDevice::Initialize(bool enableValidation)
		{
			if (enableValidation)
				INSERT_TO_VECTOR(mValidationLayers, "VK_LAYER_KHRONOS_validation");

			// Create the instance.
			vInstance = CreateInstance(enableValidation, mValidationLayers);

			// Create the debug messenger.
			if (enableValidation)
				vDebugMessenger = CreateDebugMessenger(vInstance);

			// Create the surface.
			CreateSurface();

			std::vector<const char*> deviceExtensions;
			INSERT_TO_VECTOR(deviceExtensions, VK_KHR_SWAPCHAIN_EXTENSION_NAME);

			// Create physical device.
			vPhysicalDevice = CreatePhysicalDevice(vInstance, vSurface, deviceExtensions);

			// Get swap chain support details.
			vSwapChainSupportDetails = QuerySwapChainSupportDetails(vPhysicalDevice, vSurface);

			// Get surface capabilities.
			QuerySurfaceCapabilities();

			QueryPhysicalDeviceProperties();
			GetMaxSupportedSampleCount();

			// Create queue.
			vQueue = CreateQueue(vPhysicalDevice);

			// Create logical device.
			vLogicalDevice = CreateLogicalDevice(enableValidation, mValidationLayers, deviceExtensions, vPhysicalDevice, &vQueue);
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

		void VulkanDevice::QuerySurfaceCapabilities()
		{
			VK_ASSERT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vPhysicalDevice, vSurface, &vSurfaceCapabilities), "Failed to get the surface capabilities!");
		}

		void VulkanDevice::QueryPhysicalDeviceProperties()
		{
			vkGetPhysicalDeviceProperties(vPhysicalDevice, &vPhysicalDeviceProperties);
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
