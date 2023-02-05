#pragma once

#include "Common.h"
#include "Instance.h"
#include "Image.h"
#include "Sampler.h"

namespace Copium
{
  class ColorAttachment : public Sampler
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(ColorAttachment);
  private:
    std::vector<VkImage> images;
    std::vector<VkDeviceMemory> imageMemories;
    std::vector<VkImageView> imageViews;
  public:
    ColorAttachment(Instance& instance, int width, int height)
      : Sampler{instance}
    {
      InitializeColorAttachment(width, height);
    }

    ~ColorAttachment() override
    {
      for (auto&& image : images)
        vkDestroyImage(instance.GetDevice(), image, nullptr);
      for (auto&& imageMemory : imageMemories)
        vkFreeMemory(instance.GetDevice(), imageMemory, nullptr);
      for (auto&& imageView : imageViews)
        vkDestroyImageView(instance.GetDevice(), imageView, nullptr);
    }

    VkDescriptorImageInfo GetDescriptorImageInfo(int index) const override
    {
      CP_ASSERT(index >= 0 && index < imageViews.size(), "GetDescriptorImageInfo : index out of bound for color attachment");

      VkDescriptorImageInfo imageInfo{};
      imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      imageInfo.sampler = sampler;
      imageInfo.imageView = imageViews[index];
      return imageInfo;
    }

    VkImageView GetImageView(int index)
    {
      CP_ASSERT(index >= 0 && index < imageViews.size(), "GetImageView : Index out of bound");

      return imageViews[index];
    }

  private:
    void InitializeColorAttachment(int width, int height)
    {
      images.resize(instance.GetMaxFramesInFlight());
      imageMemories.resize(instance.GetMaxFramesInFlight());
      imageViews.resize(instance.GetMaxFramesInFlight());
      for (size_t i = 0; i < images.size(); i++)
      {
        Image::InitializeImage(instance, width, height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &images[i], &imageMemories[i]);
        imageViews[i] = Image::InitializeImageView(instance, images[i], VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
      }
    }
  };
}
