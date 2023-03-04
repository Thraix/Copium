#pragma once

#include "copium/core/Vulkan.h"
#include "copium/sampler/Sampler.h"
#include "copium/util/Common.h"

#include <vulkan/vulkan.hpp>

namespace Copium
{
  class DepthAttachment final : public Sampler
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(DepthAttachment);
  private:
    VkImage image;
    VkDeviceMemory imageMemory;
    VkImageView imageView;
  public:
    DepthAttachment(Vulkan& vulkan, int width, int height);
    ~DepthAttachment() override;

    VkDescriptorImageInfo GetDescriptorImageInfo(int index) const override;
    VkImageView GetImageView() const;

  private:
    void InitializeDepthAttachment(int width, int height);
  
  };
}
