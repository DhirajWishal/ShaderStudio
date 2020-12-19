// Copyright 2020 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "SwapChain.h"
#include "Graphics/Backend/Vulkan/VulkanDevice.h"
#include "Graphics/Backend/Vulkan/Macros.h"

namespace Graphics
{
	namespace VulkanBackend
	{
		namespace _Helpers
		{
			VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
			{
				for (const auto& availableFormat : availableFormats)
					if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM
						&& availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
						return availableFormat;

				return availableFormats[0];
			}

			VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
			{
				VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

				for (const auto& availablePresentMode : availablePresentModes)
				{
					if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
						return availablePresentMode;
					else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
						bestMode = availablePresentMode;
				}

				return bestMode;
			}

			VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, UI32 width, UI32 height)
			{
				VkExtent2D actualExtent = {
					width,
					height
				};

				if ((width >= capabilities.maxImageExtent.width) || (width <= capabilities.minImageExtent.width))
					actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));

				if ((height >= capabilities.maxImageExtent.height) || (height <= capabilities.minImageExtent.height))
					actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

				return actualExtent;
			}

			std::vector<VkImageView> CreateImageViews(const std::vector<VkImage>& vImages, VkFormat imageFormat, VkDevice vDevice)
			{
				VkImageViewCreateInfo createInfo = {};
				createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				createInfo.flags = VK_NULL_HANDLE;
				createInfo.pNext = VK_NULL_HANDLE;
				createInfo.viewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D;
				createInfo.format = imageFormat;

				VkImageSubresourceRange vSubRange = {};
				vSubRange.baseArrayLayer = 1;
				vSubRange.layerCount = 1;
				vSubRange.levelCount = 1;
				vSubRange.baseMipLevel = 0;

				createInfo.subresourceRange = vSubRange;

				std::vector<VkImageView> vImageViews(vImages.size());
				UI8 counter = 0;
				for (auto itr = vImages.begin(); itr != vImages.end(); itr++)
				{
					createInfo.image = *itr;
					VK_ASSERT(vkCreateImageView(vDevice, &createInfo, nullptr, vImageViews.data() + counter), "Failed to create Vulkan Image Views for the Swap Chain images!");
					counter++;
				}

				return vImageViews;
			}
		}

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

		void SwapChain::Initialize(VulkanDevice* pDevice, UI32 width, UI32 height, float xOffset, float yOffset)
		{
			SwapChainSupportDetails& vSupport = pDevice->GetSwapChainSupportDetails();
			VkSurfaceFormatKHR surfaceFormat = _Helpers::ChooseSwapSurfaceFormat(vSupport.formats);
			VkPresentModeKHR presentMode = _Helpers::ChooseSwapPresentMode(vSupport.presentModes);

			VkCompositeAlphaFlagBitsKHR surfaceComposite = static_cast<VkCompositeAlphaFlagBitsKHR>(pDevice->GetSurfaceCapabilities().supportedCompositeAlpha);
			surfaceComposite = (surfaceComposite & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)
				? VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR
				: (surfaceComposite & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR)
				? VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR
				: (surfaceComposite & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR)
				? VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR
				: VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;

			VkSwapchainCreateInfoKHR vCI = {};
			vCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			vCI.flags = VK_NULL_HANDLE;
			vCI.pNext = VK_NULL_HANDLE;
			vCI.surface = pDevice->vSurface;
			vCI.minImageCount = pDevice->GetMaxFrameBufferCount();
			vCI.imageFormat = surfaceFormat.format;
			vCI.imageColorSpace = surfaceFormat.colorSpace;
			vCI.imageExtent = { width, height };
			vCI.imageArrayLayers = 1;
			vCI.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

			UI32 queueFamilyindices[2] = {
					pDevice->vQueue.mGraphicsFamily.value(),
					pDevice->vQueue.mTransferFamily.value()
			};

			if (pDevice->vQueue.mGraphicsFamily != pDevice->vQueue.mTransferFamily)
			{
				vCI.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
				vCI.queueFamilyIndexCount = 2;
				vCI.pQueueFamilyIndices = queueFamilyindices;
			}
			else
			{
				vCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
				vCI.queueFamilyIndexCount = 0;
				vCI.pQueueFamilyIndices = nullptr;
			}

			vCI.preTransform = vSupport.capabilities.currentTransform;
			vCI.compositeAlpha = surfaceComposite;
			vCI.presentMode = presentMode;
			vCI.clipped = VK_TRUE;
			vCI.oldSwapchain = VK_NULL_HANDLE;

			// Create the Vulkan Swap Chain.
			VK_ASSERT(vkCreateSwapchainKHR(pDevice->vLogicalDevice, &vCI, nullptr, &vSwapChain), "Failed to create the Vulkan Swap Chain!");

			// Get the swap chain images.
			vImages.resize(vCI.minImageCount);
			VK_ASSERT(vkGetSwapchainImagesKHR(pDevice->vLogicalDevice, vSwapChain, &vCI.minImageCount, vImages.data()), "Failed to get the Vulkan Swap Chain Images!");

			// Create swap chain image views.
			vImageViews = std::move(_Helpers::CreateImageViews(vImages, vCI.imageFormat, pDevice->vLogicalDevice));

			vFormat = vCI.imageFormat;
		}

		void SwapChain::Terminate(VulkanDevice* pDevice)
		{
			// Terminate the image views.
			for (auto itr = vImageViews.begin(); itr != vImageViews.end(); itr++)
				vkDestroyImageView(pDevice->vLogicalDevice, *itr, nullptr);

			vImageViews.clear();

			// Terminate the Swap Chain.
			vkDestroySwapchainKHR(pDevice->vLogicalDevice, vSwapChain, nullptr);
			vSwapChain = VK_NULL_HANDLE;
			vImages.clear();
		}
	}
}
