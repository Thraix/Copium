#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class SwapChain
{
  VkSwapchainKHR swapChain;
  VkFormat swapChainImageFormat;
  VkExtent2D swapChainExtent;
  std::vector<VkImageView> swapChainImageViews;
  std::vector<VkImage> swapChainImages;
  std::vector<VkFramebuffer> swapChainFramebuffers;

  SwapChain()
  {

  }

};
