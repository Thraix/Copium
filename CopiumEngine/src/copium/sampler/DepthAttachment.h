#pragma once

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
    DepthAttachment(int width, int height, const SamplerCreator& samplerCreator);
    ~DepthAttachment() override;

    void Resize(int width, int height);

    VkDescriptorImageInfo GetDescriptorImageInfo(int index) const override;
    VkImageView GetImageView() const;

  private:
    void InitializeDepthAttachment(int width, int height);
  };
}
