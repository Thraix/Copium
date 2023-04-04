#pragma once

#include "copium/buffer/CommandBuffer.h"
#include "copium/sampler/DepthAttachment.h"
#include "copium/util/Common.h"

#include <GLFW/glfw3.h>
#include <vector>
#include <vulkan/vulkan.h>

namespace Copium
{
  struct SwapChainSupportDetails
  {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;

    SwapChainSupportDetails(VkSurfaceKHR surface, VkPhysicalDevice physicalDevice);
    bool Valid();
  };

  class SwapChain final
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(SwapChain);
  public:
    static const int MAX_FRAMES_IN_FLIGHT = 2;
  private:
    VkSwapchainKHR handle;
    VkRenderPass renderPass;
    VkFormat imageFormat;
    VkExtent2D extent;
    std::unique_ptr<DepthAttachment> depthAttachment;
    std::vector<VkImageView> imageViews;
    std::vector<VkImage> images;
    std::vector<VkFramebuffer> framebuffers;
    uint32_t  imageIndex;
    bool resizeFramebuffer;

    int flightIndex;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;

  public:
    SwapChain();
    ~SwapChain();

    void BeginFrameBuffer(const CommandBuffer& commandBuffer) const;
    void EndFrameBuffer(const CommandBuffer& commandBuffer) const;
    VkSwapchainKHR GetHandle() const;
    VkRenderPass GetRenderPass() const;
    VkExtent2D GetExtent() const;
    VkFramebuffer GetFramebuffer() const;
    bool BeginPresent();
    void SubmitToGraphicsQueue(const CommandBuffer& commandBuffer);
    void EndPresent();
    void ResizeFramebuffer();
    void Recreate();

    int GetFlightIndex() const;

  private:
    void Initialize();
    void InitializeImageViews();
    void InitializeDepthAttachment();
    void InitializeRenderPass();
    void InitializeFramebuffers();
    void InitializeSyncObjects();
    void Destroy();

    VkSurfaceFormatKHR SelectSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR SelectSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D SelectSwapExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities);
  };
}
