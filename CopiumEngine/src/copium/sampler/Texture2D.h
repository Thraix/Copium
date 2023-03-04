#pragma once

#include "copium/buffer/CommandBufferScoped.h"
#include "copium/core/Vulkan.h"
#include "copium/sampler/Image.h"
#include "copium/sampler/Sampler.h"
#include "copium/util/Common.h"

#include <cstring>

namespace Copium
{
  class Texture2D final : public Sampler
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(Texture2D);
  private:
    VkImage image;
    VkDeviceMemory imageMemory;
    VkImageView imageView;
  public:
    Texture2D(Vulkan& vulkan, const std::string& filename);
    ~Texture2D() override;

    VkDescriptorImageInfo GetDescriptorImageInfo(int index) const override;
  private:
    void InitializeTextureImage(const std::string& filename);
  };
}
