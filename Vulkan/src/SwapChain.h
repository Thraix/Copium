#pragma once

#include "Common.h"

#include <vulkan/vulkan.h>
#include <vector>
#include <GLFW/glfw3.h>

class Instance;

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

  // Created by the class
  VkSwapchainKHR handle;
  VkRenderPass renderPass;
  VkFormat imageFormat;
  VkExtent2D extent;
  VkImage depthImage;
  VkImageView depthImageView;
  VkDeviceMemory depthImageMemory;
  std::vector<VkImageView> imageViews;
  std::vector<VkImage> images;
  std::vector<VkFramebuffer> framebuffers;

  uint32_t  imageIndex;

public:
  SwapChain(Instance& instance);
  ~SwapChain();

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
  void InitializeDepthBuffer();
  void InitializeRenderPass();
  void InitializeFramebuffers();
  void Destroy();

  VkSurfaceFormatKHR SelectSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
  VkFormat SelectDepthFormat();
  VkFormat SelectSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
  VkPresentModeKHR SelectSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
  VkExtent2D SelectSwapExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities);
};
