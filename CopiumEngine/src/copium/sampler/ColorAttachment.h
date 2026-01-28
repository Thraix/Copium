#pragma once

#include <vulkan/vulkan.hpp>

#include "copium/sampler/Sampler.h"
#include "copium/util/Common.h"

namespace Copium
{
  class ColorAttachment final : public Sampler
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(ColorAttachment);

  private:
    std::vector<VkImage> images;
    std::vector<VkDeviceMemory> imageMemories;
    std::vector<VkImageView> imageViews;
    int width;
    int height;

  public:
    ColorAttachment(const MetaFile& metaFile);
    ColorAttachment(int width, int height, const SamplerCreator& samplerCreator);
    ~ColorAttachment() override;

    void Resize(int width, int height);

    int GetWidth() const;
    int GetHeight() const;
    VkDescriptorImageInfo GetDescriptorImageInfo(int index) const override;
    VkImageView GetImageView(int index) const;

  private:
    void InitializeColorAttachment(int width, int height);
  };
}
