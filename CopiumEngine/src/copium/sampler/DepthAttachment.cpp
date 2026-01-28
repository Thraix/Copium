#include "copium/sampler/DepthAttachment.h"

#include "copium/core/Vulkan.h"
#include "copium/sampler/Image.h"

namespace Copium
{
  DepthAttachment::DepthAttachment(int width, int height, const SamplerCreator& samplerCreator)
    : Sampler{samplerCreator}
  {
    InitializeDepthAttachment(width, height);
  }

  DepthAttachment::~DepthAttachment()
  {
    VkImage imageCpy = image;
    VkDeviceMemory imageMemoryCpy = imageMemory;
    VkImageView imageViewCpy = imageView;
    Vulkan::GetDevice().QueueIdleCommand(
      [imageCpy, imageMemoryCpy, imageViewCpy]()
      {
        vkDestroyImage(Vulkan::GetDevice(), imageCpy, nullptr);
        vkFreeMemory(Vulkan::GetDevice(), imageMemoryCpy, nullptr);
        vkDestroyImageView(Vulkan::GetDevice(), imageViewCpy, nullptr);
      });
  }

  void DepthAttachment::Resize(int width, int height)
  {
    vkDestroyImage(Vulkan::GetDevice(), image, nullptr);
    vkFreeMemory(Vulkan::GetDevice(), imageMemory, nullptr);
    vkDestroyImageView(Vulkan::GetDevice(), imageView, nullptr);
    InitializeDepthAttachment(width, height);
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
    VkFormat depthFormat = Image::SelectDepthFormat();
    Image::InitializeImage(width,
                           height,
                           depthFormat,
                           VK_IMAGE_TILING_OPTIMAL,
                           VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                           VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                           &image,
                           &imageMemory);
    imageView = Image::InitializeImageView(image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
  }
}