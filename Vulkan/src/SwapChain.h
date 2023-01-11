#pragma once

#include "QueueFamilies.h"
#include <glfw/glfw3.h>
#include <vulkan/vulkan.h>
#include <vector>

struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;

	SwapChainSupportDetails(VkSurfaceKHR surface, VkPhysicalDevice device)
	{
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
		if (formatCount != 0)
		{
			formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
		if (presentModeCount != 0)
		{
			presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, presentModes.data());
		}
	}

	bool Valid()
	{
		return !formats.empty() && !presentModes.empty();
	}
};

class SwapChain
{
  // TODO: Remove, replaced by Instance::MAX_FRAMES_IN_FLIGHT
	static const int MAX_FRAMES_IN_FLIGHT = 2;

  // Needed for recreation and destruction
  GLFWwindow* window;
  VkSurfaceKHR surface;
  VkPhysicalDevice physicalDevice;
  VkDevice device;

  // Created by the class
  VkSwapchainKHR handle;
  VkRenderPass renderPass;
  VkFormat imageFormat;
  VkExtent2D extent;
  std::vector<VkImageView> imageViews;
  std::vector<VkImage> images;
  std::vector<VkFramebuffer> framebuffers;

  int flightIndex;
  uint32_t  imageIndex;
  std::vector<VkSemaphore> imageAvailableSemaphores;
  std::vector<VkSemaphore> renderFinishedSemaphores;
  std::vector<VkFence> inFlightFences;

public:
  SwapChain(GLFWwindow* window, VkSurfaceKHR surface, VkDevice device, VkPhysicalDevice physicalDevice)
    : window{window}, surface{surface}, physicalDevice{physicalDevice}, device{device}
  {
    Initialize();
    InitializeImageViews();
    InitializeRenderPass();
    InitializeFramebuffers();
    InitializeSyncObjects();
  }

  ~SwapChain()
  {
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
      vkDestroyFence(device, inFlightFences[i], nullptr);
      vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
      vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
    }
    Destroy();
    vkDestroyRenderPass(device, renderPass, nullptr);
  }

  VkSwapchainKHR GetHandle() const
  {
    return handle;
  }

  VkRenderPass GetRenderPass() const
  {
    return renderPass;
  }

  VkExtent2D GetExtent() const
  {
    return extent;
  }

  VkFramebuffer GetFramebuffer() const
  {
    return framebuffers[imageIndex];
  }

  bool BeginPresent()
  {
    vkWaitForFences(device, 1, &inFlightFences[flightIndex], VK_TRUE, UINT64_MAX);

    VkResult result = vkAcquireNextImageKHR(device, handle, UINT64_MAX, imageAvailableSemaphores[flightIndex], VK_NULL_HANDLE, &imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
      Recreate();
      return false;
    }

    vkResetFences(device, 1, &inFlightFences[flightIndex]);
    return true;
  }

  void EndPresent(VkQueue presentQueue, bool framebufferResized)
  {
    VkSwapchainKHR swapChains[] = {handle};
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &renderFinishedSemaphores[flightIndex];
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &handle;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr;

    VkResult result = vkQueuePresentKHR(presentQueue, &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized)
    {
      framebufferResized = false;
      Recreate();
    }

    flightIndex = (flightIndex + 1) % MAX_FRAMES_IN_FLIGHT;
  }

  int GetFlightIndex() const
  {
    return flightIndex;
  }

  int GetMaxFramesInFlight() const
  {
    return MAX_FRAMES_IN_FLIGHT;
  }

  VkFence GetInFlightFence() const
  {
    return inFlightFences[flightIndex];
  }

  VkSemaphore GetAvailableImageSemaphore() const
  {
    return imageAvailableSemaphores[flightIndex];
  }

  VkSemaphore GetRenderFinishedSemaphore() const
  {
    return renderFinishedSemaphores[flightIndex];
  }


  void Recreate()
  {
    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(window, &width, &height);
    while (width == 0 || height == 0)
    {
      glfwGetFramebufferSize(window, &width, &height);
      glfwWaitEvents();
    }
    
    vkDeviceWaitIdle(device);

    Destroy();

    Initialize();
    InitializeImageViews();
    InitializeFramebuffers();
  }

private:

  void Initialize()
  {
    SwapChainSupportDetails swapChainSupport{surface, physicalDevice};

    VkSurfaceFormatKHR format = SelectSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = SelectSwapPresentMode(swapChainSupport.presentModes);
    extent = SelectSwapExtent(window, swapChainSupport.capabilities);
    imageFormat = format.format;
    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount != 0)
    {
      imageCount = std::min(imageCount, swapChainSupport.capabilities.maxImageCount);
    }

    QueueFamilies queueFamilies{surface, physicalDevice};
    std::vector<uint32_t> queueFamilyIndices{queueFamilies.graphicsFamily.value(), queueFamilies.presentFamily.value()};

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = format.format;
    createInfo.imageColorSpace = format.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;
    if (queueFamilies.graphicsFamily != queueFamilies.presentFamily)
    {
      createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
      createInfo.queueFamilyIndexCount = 2;
      createInfo.pQueueFamilyIndices = queueFamilyIndices.data();
    }
    else
    {
      createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
      createInfo.queueFamilyIndexCount = 0;
      createInfo.pQueueFamilyIndices = nullptr;
    }

    VK_ASSERT(vkCreateSwapchainKHR(device, &createInfo, nullptr, &handle), "Failed to initialize the swapchain");

    vkGetSwapchainImagesKHR(device, handle, &imageCount, nullptr);
    images.resize(imageCount);
    vkGetSwapchainImagesKHR(device, handle, &imageCount, images.data());
  }

  void InitializeImageViews()
  {
    imageViews.resize(images.size());
    for (size_t i = 0; i < images.size(); i++)
    {
      VkImageViewCreateInfo createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
      createInfo.image = images[i];
      createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
      createInfo.format = imageFormat;
      createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
      createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
      createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
      createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
      createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      createInfo.subresourceRange.baseMipLevel = 0;
      createInfo.subresourceRange.levelCount = 1;
      createInfo.subresourceRange.baseArrayLayer = 0;
      createInfo.subresourceRange.layerCount = 1;
      VK_ASSERT(vkCreateImageView(device, &createInfo, nullptr, &imageViews[i]), "Failed to initialize swapchain image view");
    }
  }

  void InitializeRenderPass()
  {
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = imageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassCreateInfo{};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.attachmentCount = 1;
    renderPassCreateInfo.pAttachments = &colorAttachment;
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpass;
    renderPassCreateInfo.dependencyCount = 1;
    renderPassCreateInfo.pDependencies = &dependency;

    VK_ASSERT(vkCreateRenderPass(device, &renderPassCreateInfo, nullptr, &renderPass), "Failed to initialze render pass");
  }

  void InitializeFramebuffers()
  {
    framebuffers.resize(images.size());

    for (size_t i = 0; i < imageViews.size(); ++i)
    {
      VkImageView attachments[] = {imageViews[i]};

      VkFramebufferCreateInfo createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
      createInfo.renderPass = renderPass;
      createInfo.attachmentCount = 1;
      createInfo.pAttachments = attachments;
      createInfo.width = extent.width;
      createInfo.height = extent.height;
      createInfo.layers = 1;

      VK_ASSERT(vkCreateFramebuffer(device, &createInfo, nullptr, &framebuffers[i]), "Failed to initialize swap chain framebuffer");
    }
  }

  void InitializeSyncObjects()
  {
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    VkSemaphoreCreateInfo semaphoreCreateInfo{};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
      VK_ASSERT(vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &imageAvailableSemaphores[i]), "Failed to initialize available image semaphore");
      VK_ASSERT(vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &renderFinishedSemaphores[i]), "Failed to initialize render finished semaphore");

      VkFenceCreateInfo fenceCreateInfo{};
      fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
      fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

      VK_ASSERT(vkCreateFence(device, &fenceCreateInfo, nullptr, &inFlightFences[i]), "Failed to initialize in flight fence");
    }
  }

  void Destroy()
  {
    for (auto&& framebuffer : framebuffers)
    {
      vkDestroyFramebuffer(device, framebuffer, nullptr);
    }
    for (auto&& swapChainImageView : imageViews)
    {
      vkDestroyImageView(device, swapChainImageView, nullptr);
    }
    vkDestroySwapchainKHR(device, handle, nullptr);
  }

  VkSurfaceFormatKHR SelectSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
  {
    for (auto&& availableFormat : availableFormats)
    {
      if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) 
      {
        return availableFormat;
      }
    }
    return availableFormats[0];
  }

  VkPresentModeKHR SelectSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
  {
    for (auto&& availablePresentMode : availablePresentModes)
    {
      if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
      {
        return availablePresentMode;
      }
    }

    // VK_PRESENT_MODE_FIFO_KHR is guaranteed to be present
    return VK_PRESENT_MODE_FIFO_KHR;
  }

  VkExtent2D SelectSwapExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities)
  {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
      return capabilities.currentExtent;

		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		VkExtent2D extent{width, height};
		extent.width = std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		extent.height = std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
		return extent;
  }


};
