#pragma once

#include "Common.h"
#include "Instance.h"
#include "Sampler.h"

#include <vulkan/vulkan.hpp>

namespace Copium
{
  class ColorAttachment final : public Sampler
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(ColorAttachment);
  private:
    std::vector<VkImage> images;
    std::vector<VkDeviceMemory> imageMemories;
    std::vector<VkImageView> imageViews;
  public:
    ColorAttachment(Instance& instance, int width, int height);
    ~ColorAttachment() override;

    VkDescriptorImageInfo GetDescriptorImageInfo(int index) const override;
    VkImageView GetImageView(int index);

  private:
    void InitializeColorAttachment(int width, int height);
  };
}
