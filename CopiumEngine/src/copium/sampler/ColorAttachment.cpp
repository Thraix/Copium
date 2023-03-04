#include "copium/sampler/ColorAttachment.h"

#include "copium/core/Device.h"
#include "copium/core/SwapChain.h"
#include "copium/sampler/Image.h"

namespace Copium
{
  ColorAttachment::ColorAttachment(Vulkan& vulkan, int width, int height)
    : Sampler{vulkan}
  {
    InitializeColorAttachment(width, height);
  }

  ColorAttachment::~ColorAttachment()
  {
    for (auto&& image : images)
      vkDestroyImage(vulkan.GetDevice(), image, nullptr);
    for (auto&& imageMemory : imageMemories)
      vkFreeMemory(vulkan.GetDevice(), imageMemory, nullptr);
    for (auto&& imageView : imageViews)
      vkDestroyImageView(vulkan.GetDevice(), imageView, nullptr);
  }

  VkDescriptorImageInfo ColorAttachment::GetDescriptorImageInfo(int index) const
  {
    CP_ASSERT(index >= 0 && index < imageViews.size(), "GetDescriptorImageInfo : index out of bound for color attachment");

    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.sampler = sampler;
    imageInfo.imageView = imageViews[index];
    return imageInfo;
  }

  VkImageView ColorAttachment::GetImageView(int index)
  {
    CP_ASSERT(index >= 0 && index < imageViews.size(), "GetImageView : Index out of bound");

    return imageViews[index];
  }

  void ColorAttachment::InitializeColorAttachment(int width, int height)
  {
    images.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
    imageViews.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
    imageMemories.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
    for (size_t i = 0; i < images.size(); i++)
    {
      Image::InitializeImage(vulkan, width, height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &images[i], &imageMemories[i]);
      imageViews[i] = Image::InitializeImageView(vulkan, images[i], VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
    }
  }
}
