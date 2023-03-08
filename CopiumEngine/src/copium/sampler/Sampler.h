#pragma once

#include "copium/core/Vulkan.h"
#include "copium/util/Common.h"

#include <vulkan/vulkan.hpp>

namespace Copium
{
  class Sampler
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(Sampler);
  protected:
    Vulkan& vulkan;
    VkSampler sampler;
  public:
    Sampler(Vulkan& vulkan);
    virtual ~Sampler();

    virtual VkDescriptorImageInfo GetDescriptorImageInfo(int index) const = 0;
    operator VkSampler() const;
  private:
    void InitializeSampler();
  };
}
