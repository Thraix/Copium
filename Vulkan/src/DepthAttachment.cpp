#include "DepthAttachment.h"

#include "Image.h"

namespace Copium
{
  DepthAttachment::DepthAttachment(Instance& instance, int width, int height)
    : Sampler{instance}
  {
    InitializeDepthAttachment(width, height);
  }

  DepthAttachment::~DepthAttachment()
  {
    vkDestroyImage(instance.GetDevice(), image, nullptr);
    vkFreeMemory(instance.GetDevice(), imageMemory, nullptr);
    vkDestroyImageView(instance.GetDevice(), imageView, nullptr);
  }

  VkDescriptorImageInfo DepthAttachment::GetDescriptorImageInfo(int index) const
  {
    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.sampler = sampler;
    imageInfo.imageView = imageView;
    return imageInfo;
  }

  VkImageView DepthAttachment::GetImageView() const
  {
    return imageView;
  }

  void DepthAttachment::InitializeDepthAttachment(int width, int height)
  {
    VkFormat depthFormat = Image::SelectDepthFormat(instance);
    Image::InitializeImage(instance, width, height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &image, &imageMemory);
    imageView = Image::InitializeImageView(instance, image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
  }
}