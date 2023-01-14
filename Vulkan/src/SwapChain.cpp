#include "QueueFamilies.h"
#include "Instance.h"
#include <glfw/glfw3.h>
#include <vulkan/vulkan.h>
#include <vector>

SwapChainSupportDetails::SwapChainSupportDetails(VkSurfaceKHR surface, VkPhysicalDevice physicalDevice)
{
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
	if (formatCount != 0)
	{
		formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
	if (presentModeCount != 0)
	{
		presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data());
	}
}

bool SwapChainSupportDetails::Valid()
{
	return !formats.empty() && !presentModes.empty();
}

SwapChain::SwapChain(Instance& instance)
	: instance{instance}
{
	Initialize();
	InitializeImageViews();
	InitializeRenderPass();
	InitializeFramebuffers();
}

SwapChain::~SwapChain()
{
	Destroy();
	vkDestroyRenderPass(instance.GetDevice(), renderPass, nullptr);
}

VkSwapchainKHR SwapChain::GetHandle() const
{
	return handle;
}

VkRenderPass SwapChain::GetRenderPass() const
{
	return renderPass;
}

VkExtent2D SwapChain::GetExtent() const
{
	return extent;
}

VkFramebuffer SwapChain::GetFramebuffer() const
{
	return framebuffers[imageIndex];
}

bool SwapChain::BeginPresent(VkSemaphore signalSemaphore)
{
	VkResult result = vkAcquireNextImageKHR(instance.GetDevice(), handle, UINT64_MAX, signalSemaphore, VK_NULL_HANDLE, &imageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		Recreate();
		return false;
	}
	return true;
}

void SwapChain::EndPresent(VkQueue presentQueue, VkSemaphore* waitSemaphore, bool framebufferResized)
{
	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = waitSemaphore;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &handle;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr;

	VkResult result = vkQueuePresentKHR(presentQueue, &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized)
	{
		Recreate();
	}
}

void SwapChain::Recreate()
{
	int width = 0;
	int height = 0;
	glfwGetFramebufferSize(instance.GetWindow(), &width, &height);
	while (width == 0 || height == 0)
	{
		glfwGetFramebufferSize(instance.GetWindow(), &width, &height);
		glfwWaitEvents();
	}
	
	vkDeviceWaitIdle(instance.GetDevice());

	Destroy();

	Initialize();
	InitializeImageViews();
	InitializeFramebuffers();
}

void SwapChain::Initialize()
{
	SwapChainSupportDetails swapChainSupport{instance.GetSurface(), instance.GetPhysicalDevice()};

	VkSurfaceFormatKHR format = SelectSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = SelectSwapPresentMode(swapChainSupport.presentModes);
	extent = SelectSwapExtent(instance.GetWindow(), swapChainSupport.capabilities);
	imageFormat = format.format;
	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount != 0)
	{
		imageCount = std::min(imageCount, swapChainSupport.capabilities.maxImageCount);
	}

	QueueFamiliesQuery queueFamilies{instance.GetSurface(), instance.GetPhysicalDevice()};
	std::vector<uint32_t> queueFamilyIndices{queueFamilies.graphicsFamily.value(), queueFamilies.presentFamily.value()};

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = instance.GetSurface();
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

	CP_VK_ASSERT(vkCreateSwapchainKHR(instance.GetDevice(), &createInfo, nullptr, &handle), "Failed to initialize the swapchain");

	vkGetSwapchainImagesKHR(instance.GetDevice(), handle, &imageCount, nullptr);
	images.resize(imageCount);
	vkGetSwapchainImagesKHR(instance.GetDevice(), handle, &imageCount, images.data());
}

void SwapChain::InitializeImageViews()
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
		CP_VK_ASSERT(vkCreateImageView(instance.GetDevice(), &createInfo, nullptr, &imageViews[i]), "Failed to initialize swapchain image view");
	}
}

void SwapChain::InitializeRenderPass()
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
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo renderPassCreateInfo{};
	renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.attachmentCount = 1;
	renderPassCreateInfo.pAttachments = &colorAttachment;
	renderPassCreateInfo.subpassCount = 1;
	renderPassCreateInfo.pSubpasses = &subpass;
	renderPassCreateInfo.dependencyCount = 1;
	renderPassCreateInfo.pDependencies = &dependency;

	CP_VK_ASSERT(vkCreateRenderPass(instance.GetDevice(), &renderPassCreateInfo, nullptr, &renderPass), "Failed to initialze render pass");
}

void SwapChain::InitializeFramebuffers()
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

		CP_VK_ASSERT(vkCreateFramebuffer(instance.GetDevice(), &createInfo, nullptr, &framebuffers[i]), "Failed to initialize swap chain framebuffer");
	}
}

void SwapChain::Destroy()
{
	for (auto&& framebuffer : framebuffers)
	{
		vkDestroyFramebuffer(instance.GetDevice(), framebuffer, nullptr);
	}
	for (auto&& swapChainImageView : imageViews)
	{
		vkDestroyImageView(instance.GetDevice(), swapChainImageView, nullptr);
	}
	vkDestroySwapchainKHR(instance.GetDevice(), handle, nullptr);
}

VkSurfaceFormatKHR SwapChain::SelectSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
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

VkPresentModeKHR SwapChain::SelectSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
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

VkExtent2D SwapChain::SelectSwapExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities)
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
