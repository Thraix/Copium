#include "copium/core/SwapChain.h"

#include "copium/core/QueueFamilies.h"
#include "copium/core/Vulkan.h"
#include "copium/sampler/Image.h"

#include <GLFW/glfw3.h>

namespace Copium
{
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

  SwapChain::SwapChain()
  {
    Initialize();
    InitializeImageViews();
    InitializeDepthAttachment();
    InitializeRenderPass();
    InitializeFramebuffers();
    InitializeSyncObjects();
  }

  SwapChain::~SwapChain()
  {
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
      vkDestroyFence(Vulkan::GetDevice(), inFlightFences[i], nullptr);
      vkDestroySemaphore(Vulkan::GetDevice(), renderFinishedSemaphores[i], nullptr);
      vkDestroySemaphore(Vulkan::GetDevice(), imageAvailableSemaphores[i], nullptr);
    }
    Destroy();
    vkDestroyRenderPass(Vulkan::GetDevice(), renderPass, nullptr);
  }

  void SwapChain::BeginFrameBuffer(const CommandBuffer& commandBuffer) const
  {
    std::vector<VkClearValue> clearValues{2};
    clearValues[0].color = {{0.02f, 0.02f, 0.02f, 1.0f}};
    clearValues[1].depthStencil = {1.0f, 0};

    VkRenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = renderPass;
    renderPassBeginInfo.framebuffer = framebuffers[imageIndex];
    renderPassBeginInfo.renderArea.offset = {0, 0};
    renderPassBeginInfo.renderArea.extent = extent;
    renderPassBeginInfo.clearValueCount = clearValues.size();
    renderPassBeginInfo.pClearValues = clearValues.data();
    vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = extent.width;
    viewport.height = extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = extent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
  }

  void SwapChain::EndFrameBuffer(const CommandBuffer& commandBuffer) const
  {
    vkCmdEndRenderPass(commandBuffer);
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

  bool SwapChain::BeginPresent()
  {
    vkWaitForFences(Vulkan::GetDevice(), 1, &inFlightFences[flightIndex], VK_TRUE, UINT64_MAX);

    VkResult result = vkAcquireNextImageKHR(Vulkan::GetDevice(), handle, UINT64_MAX, imageAvailableSemaphores[flightIndex], VK_NULL_HANDLE, &imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
      Recreate();
      return false;
    }
    vkResetFences(Vulkan::GetDevice(), 1, &inFlightFences[flightIndex]);
    return true;
  }

  void SwapChain::SubmitToGraphicsQueue(const CommandBuffer& commandBuffer)
  {
    VkCommandBuffer cmd = commandBuffer;
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &imageAvailableSemaphores[flightIndex];
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmd;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &renderFinishedSemaphores[flightIndex];

    CP_VK_ASSERT(vkQueueSubmit(Vulkan::GetDevice().GetGraphicsQueue(), 1, &submitInfo, inFlightFences[flightIndex]), "Failed to submit command buffer");
  }

  void SwapChain::EndPresent()
  {
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &renderFinishedSemaphores[flightIndex];
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &handle;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr;

    VkResult result = vkQueuePresentKHR(Vulkan::GetDevice().GetPresentQueue(), &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || resizeFramebuffer)
    {
      Recreate();
      resizeFramebuffer = false;
    }

    flightIndex = (flightIndex + 1) % MAX_FRAMES_IN_FLIGHT;
  }

  void SwapChain::ResizeFramebuffer()
  {
    resizeFramebuffer = true;
  }

  void SwapChain::Recreate()
  {
    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(Vulkan::GetWindow().GetWindow(), &width, &height);
    while (width == 0 || height == 0)
    {
      glfwGetFramebufferSize(Vulkan::GetWindow().GetWindow(), &width, &height);
      glfwWaitEvents();
    }

    vkDeviceWaitIdle(Vulkan::GetDevice());

    Destroy();

    Initialize();
    InitializeImageViews();
    InitializeDepthAttachment();
    InitializeFramebuffers();
  }

  int SwapChain::GetFlightIndex() const
  {
    return flightIndex;
  }

  int SwapChain::GetImageCount() const
  {
    return images.size();
  }

  void SwapChain::Initialize()
  {
    SwapChainSupportDetails swapChainSupport{Vulkan::GetWindow().GetSurface(), Vulkan::GetDevice().GetPhysicalDevice()};

    VkSurfaceFormatKHR format = SelectSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = SelectSwapPresentMode(swapChainSupport.presentModes);
    extent = SelectSwapExtent(Vulkan::GetWindow().GetWindow(), swapChainSupport.capabilities);
    imageFormat = format.format;
    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount != 0)
    {
      imageCount = std::min(imageCount, swapChainSupport.capabilities.maxImageCount);
    }

    QueueFamiliesQuery queueFamilies{Vulkan::GetWindow().GetSurface(), Vulkan::GetDevice().GetPhysicalDevice()};
    std::vector<uint32_t> queueFamilyIndices{queueFamilies.graphicsFamily.value(), queueFamilies.presentFamily.value()};

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = Vulkan::GetWindow().GetSurface();
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

    CP_VK_ASSERT(vkCreateSwapchainKHR(Vulkan::GetDevice(), &createInfo, nullptr, &handle), "Failed to initialize the swapchain");

    vkGetSwapchainImagesKHR(Vulkan::GetDevice(), handle, &imageCount, nullptr);
    images.resize(imageCount);
    vkGetSwapchainImagesKHR(Vulkan::GetDevice(), handle, &imageCount, images.data());
  }

  void SwapChain::InitializeImageViews()
  {
    imageViews.resize(images.size());
    for (size_t i = 0; i < images.size(); i++)
    {
      imageViews[i] = Image::InitializeImageView(images[i], imageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
    }
  }

  void SwapChain::InitializeDepthAttachment()
  {
    depthAttachment = std::make_unique<DepthAttachment>(extent.width, extent.height, SamplerCreator{});
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

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = Image::SelectDepthFormat();
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    std::vector<VkAttachmentDescription> attachments{colorAttachment, depthAttachment};
    VkRenderPassCreateInfo renderPassCreateInfo{};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.attachmentCount = attachments.size();
    renderPassCreateInfo.pAttachments = attachments.data();
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpass;
    renderPassCreateInfo.dependencyCount = 1;
    renderPassCreateInfo.pDependencies = &dependency;

    CP_VK_ASSERT(vkCreateRenderPass(Vulkan::GetDevice(), &renderPassCreateInfo, nullptr, &renderPass), "Failed to initialze render pass");
  }

  void SwapChain::InitializeFramebuffers()
  {
    framebuffers.resize(images.size());

    for (size_t i = 0; i < imageViews.size(); ++i)
    {
      std::vector<VkImageView> attachments{imageViews[i], depthAttachment->GetImageView()};

      VkFramebufferCreateInfo createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
      createInfo.renderPass = renderPass;
      createInfo.attachmentCount = attachments.size();
      createInfo.pAttachments = attachments.data();
      createInfo.width = extent.width;
      createInfo.height = extent.height;
      createInfo.layers = 1;

      CP_VK_ASSERT(vkCreateFramebuffer(Vulkan::GetDevice(), &createInfo, nullptr, &framebuffers[i]), "Failed to initialize swap chain framebuffer");
    }
  }

  void SwapChain::InitializeSyncObjects()
  {
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    VkSemaphoreCreateInfo semaphoreCreateInfo{};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
      CP_VK_ASSERT(vkCreateSemaphore(Vulkan::GetDevice(), &semaphoreCreateInfo, nullptr, &imageAvailableSemaphores[i]), "Failed to initialize available image semaphore");
      CP_VK_ASSERT(vkCreateSemaphore(Vulkan::GetDevice(), &semaphoreCreateInfo, nullptr, &renderFinishedSemaphores[i]), "Failed to initialize render finished semaphore");

      VkFenceCreateInfo fenceCreateInfo{};
      fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
      fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

      CP_VK_ASSERT(vkCreateFence(Vulkan::GetDevice(), &fenceCreateInfo, nullptr, &inFlightFences[i]), "Failed to initialize in flight fence");
    }
  }

  void SwapChain::Destroy()
  {
    for (auto&& framebuffer : framebuffers)
    {
      vkDestroyFramebuffer(Vulkan::GetDevice(), framebuffer, nullptr);
    }
    for (auto&& swapChainImageView : imageViews)
    {
      vkDestroyImageView(Vulkan::GetDevice(), swapChainImageView, nullptr);
    }
    vkDestroySwapchainKHR(Vulkan::GetDevice(), handle, nullptr);
  }

  VkSurfaceFormatKHR SwapChain::SelectSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
  {
    for (auto&& availableFormat : availableFormats)
    {
      if (availableFormat.format == VK_FORMAT_R8G8B8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
      {
        return availableFormat;
      }
    }
    return availableFormats[0];
  }

  VkPresentModeKHR SwapChain::SelectSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
  {
    return VK_PRESENT_MODE_FIFO_KHR;
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
}
