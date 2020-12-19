// Copyright 2020 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Graphics/Core/GDevice.h"
#include "RenderTarget/SwapChain.h"
#include "Queue.h"

namespace Graphics
{
	namespace VulkanBackend
	{
		class VulkanDevice : public GDevice {
		public:
			VulkanDevice() {}
			~VulkanDevice() {}

			virtual void CreateWindow(UI32 width, UI32 height, const char* pTitle) override final;
			virtual void DestroyWindow() override final;

			virtual void Initialize(bool enableValidation) override final;
			virtual void Terminate() override final;

			virtual void BeginDraw() override final;
			virtual void Update() override final;
			virtual void EndDraw() override final;

		public:
			virtual GRenderTarget* CreateRenderTarget(RenderTargetType type, UI32 width, UI32 height, float xOffset, float yOffset) override final;
			virtual void DestroyRenderTarget(GRenderTarget* pRenderTarget) override final;

		public:
			VkPhysicalDeviceProperties& GetPhysicalDeviceProperties() { return vPhysicalDeviceProperties; }
			SwapChainSupportDetails& GetSwapChainSupportDetails() { return vSwapChainSupportDetails; }
			VkSurfaceCapabilitiesKHR& GetSurfaceCapabilities() { return vSurfaceCapabilities; }
			UI32 GetMaxFrameBufferCount() const;

		private:
			void SetupGLFW();
			void TerminateGLFW();

			void CreateSurface();
			void DestroySurface();

			void CreatePhysicalDevice(const std::vector<const char*>& deviceExtensions);
			void CreateLogicalDevice(const std::vector<const char*>& deviceExtensions);

			void QuerySurfaceCapabilities();

			void GetMaxSupportedSampleCount();

		public:
			VkPhysicalDeviceProperties vPhysicalDeviceProperties = {};
			SwapChainSupportDetails vSwapChainSupportDetails = {};
			VkSurfaceCapabilitiesKHR vSurfaceCapabilities = {};

			std::vector<const char*> mValidationLayers;
			VulkanQueue vQueue = {};

			VkInstance vInstance = VK_NULL_HANDLE;
			VkDebugUtilsMessengerEXT vDebugMessenger = VK_NULL_HANDLE;

			VkSurfaceKHR vSurface = VK_NULL_HANDLE;

			VkPhysicalDevice vPhysicalDevice = VK_NULL_HANDLE;
			VkDevice vLogicalDevice = VK_NULL_HANDLE;

			VkSampleCountFlags vSampleCount = VkSampleCountFlagBits::VK_SAMPLE_COUNT_64_BIT;
		};
	}
}