#pragma once

#include "Common.h"
#include "Instance.h"
#include "Image.h"
#include "Sampler.h"

namespace Copium
{
  class DepthAttachment : public Sampler
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(DepthAttachment);
  private:
    VkImage image;
    VkDeviceMemory imageMemory;
    VkImageView imageView;
  public:
    DepthAttachment(Instance& instance, int width, int height)
      : Sampler{instance}
    {
      InitializeDepthAttachment(width, height);
    }

    ~DepthAttachment() override
    {
      vkDestroyImage(instance.GetDevice(), image, nullptr);
      vkFreeMemory(instance.GetDevice(), imageMemory, nullptr);
      vkDestroyImageView(instance.GetDevice(), imageView, nullptr);
    }

    VkDescriptorImageInfo GetDescriptorImageInfo(int index) const override
    {
      VkDescriptorImageInfo imageInfo{};
      imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      imageInfo.sampler = sampler;
      imageInfo.imageView = imageView;
      return imageInfo;
    }

    VkImageView GetImageView() const
    {
      return imageView;
    }

  private:
    void InitializeDepthAttachment(int width, int height)
    {
      VkFormat depthFormat = Image::SelectDepthFormat(instance);
      Image::InitializeImage(instance, width, height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &image, &imageMemory);
      imageView = Image::InitializeImageView(instance, image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
    }
  };
}
