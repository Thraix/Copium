#pragma once

#include <vulkan/vulkan.hpp>

#include "copium/buffer/Buffer.h"
#include "copium/util/Common.h"

namespace Copium
{
  class Image
  {
    CP_STATIC_CLASS(Image);

  public:
    static void InitializeImage(uint32_t width,
                                uint32_t height,
                                VkFormat format,
                                VkImageTiling tiling,
                                VkImageUsageFlags usage,
                                VkMemoryPropertyFlags properties,
                                VkImage* image,
                                VkDeviceMemory* imageMemory);
    static VkImageView InitializeImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
    static void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    static void CopyBufferToImage(const Buffer& buffer, VkImage image, uint32_t width, uint32_t height);
    static VkFormat SelectDepthFormat();

  private:
    static bool HasStencilComponent(VkFormat format);
    static VkFormat SelectSupportedFormat(const std::vector<VkFormat>& candidates,
                                          VkImageTiling tiling,
                                          VkFormatFeatureFlags features);
  };
}