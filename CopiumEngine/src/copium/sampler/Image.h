#pragma once

#include "copium/buffer/Buffer.h"
#include "copium/core/Vulkan.h"
#include "copium/util/Common.h"

#include  <vulkan/vulkan.hpp>

namespace Copium
{
  class Image
  {
		CP_STATIC_CLASS(Image);
  public:
    static void InitializeImage(Vulkan& vulkan, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage* image, VkDeviceMemory* imageMemory);
    static VkImageView InitializeImageView(Vulkan& vulkan, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
    static void TransitionImageLayout(Vulkan& vulkan, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    static void CopyBufferToImage(Vulkan& vulkan, const Buffer& buffer, VkImage image, uint32_t width, uint32_t height);
    static VkFormat SelectDepthFormat(Vulkan& vulkan);

  private:
    static bool HasStencilComponent(VkFormat format);
    static VkFormat SelectSupportedFormat(Vulkan& vulkan, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
  };
}