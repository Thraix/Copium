#pragma once

#include <cstring>
#include "CommandBufferScoped.h"
#include "Common.h"
#include "Image.h"
#include "Instance.h"
#include "Sampler.h"

namespace Copium
{
  class Texture2D : public Sampler
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(Texture2D);
  private:
    VkImage image;
    VkDeviceMemory imageMemory;
    VkImageView imageView;
  public:
    Texture2D(Instance& instance, const std::string& filename);
    ~Texture2D() override;

    VkDescriptorImageInfo GetDescriptorImageInfo(int index) const override;
  private:
    void InitializeTextureImage(const std::string& filename);
  };
}
