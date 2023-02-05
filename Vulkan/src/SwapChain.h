#pragma once

#include "Common.h"

#include <vulkan/vulkan.h>
#include <vector>
#include <GLFW/glfw3.h>

namespace Copium
{
  class Instance;
  class CommandBuffer;
  class DepthAttachment;

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
  private:
    Instance& instance;

    VkSwapchainKHR handle;
    VkRenderPass renderPass;
    VkFormat imageFormat;
    VkExtent2D extent;
    std::unique_ptr<DepthAttachment> depthAttachment;
    std::vector<VkImageView> imageViews;
    std::vector<VkImage> images;
    std::vector<VkFramebuffer> framebuffers;
    uint32_t  imageIndex;

  public:
    SwapChain(Instance& instance);
    ~SwapChain();

    void BeginFrameBuffer(const CommandBuffer& commandBuffer) const;
    void EndFrameBuffer(const CommandBuffer& commandBuffer) const;
    VkSwapchainKHR GetHandle() const;
    VkRenderPass GetRenderPass() const;
    VkExtent2D GetExtent() const;
    VkFramebuffer GetFramebuffer() const;
    bool BeginPresent(VkSemaphore signalSemaphore);
    void EndPresent(VkQueue presentQueue, VkSemaphore* waitSemaphore, bool framebufferResized);
    void Recreate();


  private:
    void Initialize();
    void InitializeImageViews();
    void InitializeDepthAttachment();
    void InitializeRenderPass();
    void InitializeFramebuffers();
    void Destroy();

    VkSurfaceFormatKHR SelectSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR SelectSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D SelectSwapExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities);
  };
}
