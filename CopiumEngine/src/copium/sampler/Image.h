#pragma once

#include "copium/buffer/Buffer.h"
#include "copium/core/Instance.h"
#include "copium/util/Common.h"

#include  <vulkan/vulkan.hpp>

namespace Copium
{
  class Image
  {
		CP_STATIC_CLASS(Image);
  public:
    static void InitializeImage(Instance& instance, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage* image, VkDeviceMemory* imageMemory);
    static VkImageView InitializeImageView(Instance& instance, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
    static void TransitionImageLayout(Instance& instance, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    static void CopyBufferToImage(Instance& instance, const Buffer& buffer, VkImage image, uint32_t width, uint32_t height);
    static VkFormat SelectDepthFormat(Instance& instance);

  private:
    static bool HasStencilComponent(VkFormat format);
    static VkFormat SelectSupportedFormat(Instance& instance, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
  };
}