#include "Image.h"

#include "CommandBufferScoped.h"

namespace Copium
{
  void Image::InitializeImage(Instance& instance, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage* image, VkDeviceMemory* imageMemory)
  {
    VkImageCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    createInfo.imageType = VK_IMAGE_TYPE_2D;
    createInfo.extent.width = width;
    createInfo.extent.height = height;
    createInfo.extent.depth = 1;
    createInfo.mipLevels = 1;
    createInfo.arrayLayers = 1;
    createInfo.format = format;
    createInfo.tiling = tiling;
    createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    createInfo.usage = usage;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    createInfo.flags = 0;

    CP_VK_ASSERT(vkCreateImage(instance.GetDevice(), &createInfo, nullptr, image), "InitializeImage : Failed to initialize image");

    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(instance.GetDevice(), *image, &memoryRequirements);

    VkMemoryAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize = memoryRequirements.size;
    allocateInfo.memoryTypeIndex = instance.FindMemoryType(memoryRequirements.memoryTypeBits, properties);

    CP_VK_ASSERT(vkAllocateMemory(instance.GetDevice(), &allocateInfo, nullptr, imageMemory), "InitializeImage : Failed to initiallizse image memory");

    vkBindImageMemory(instance.GetDevice(), *image, *imageMemory, 0);
  }

  VkImageView Image::InitializeImageView(Instance& instance, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
  {
    VkImageView imageView;
    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = image;
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = format;
    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.subresourceRange.aspectMask = aspectFlags;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;
    CP_VK_ASSERT(vkCreateImageView(instance.GetDevice(), &createInfo, nullptr, &imageView), "InitializeImageView : Failed to initialize image view");
    return imageView;
  }

  void Image::TransitionImageLayout(Instance& instance, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
  {
    CommandBufferScoped commandBuffer{instance};

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = 0;

    VkPipelineStageFlags srcStage;
    VkPipelineStageFlags dstStage;

    if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
      barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

      if (HasStencilComponent(format)) {
        barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
      }
    }
    else {
      barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
      barrier.srcAccessMask = 0;
      barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

      srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
      dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
      barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

      srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
      dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
      barrier.srcAccessMask = 0;
      barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

      srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
      dstStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
      barrier.srcAccessMask = 0;
      barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

      srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
      dstStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    }
    else
    {
      CP_ABORT("TransitioinImageLayout : Unsupported layout transition");
    }

    vkCmdPipelineBarrier(commandBuffer, srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
  }

  void Image::CopyBufferToImage(Instance& instance, const Buffer& buffer, VkImage image, uint32_t width, uint32_t height)
  {
    CommandBufferScoped commandBuffer{instance};

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = {0, 0, 0};
    region.imageExtent = {width, height, 1};

    vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
  }


  VkFormat Image::SelectDepthFormat(Instance& instance)
  {
    return SelectSupportedFormat(instance, {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT}, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
  }

  bool Image::HasStencilComponent(VkFormat format)
  {
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
  }

  VkFormat Image::SelectSupportedFormat(Instance& instance, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
  {
    for (VkFormat format : candidates)
    {
      VkFormatProperties properties;
      vkGetPhysicalDeviceFormatProperties(instance.GetPhysicalDevice(), format, &properties);
      if (tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & features) == features)
      {
        return format;
      }
      else if (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & features) == features)
      {
        return format;
      }
    }
    CP_ABORT("SelectSupportedFormat : Failed to select supported format");
  }
}
