#pragma once

#include "copium/core/Instance.h"
#include "copium/util/Common.h"

#include <vulkan/vulkan.hpp>

namespace Copium
{
  class Sampler
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(Sampler);
  protected:
    Instance& instance;
    VkSampler sampler;
  public:
    Sampler(Instance& instance);
    virtual ~Sampler();

    virtual VkDescriptorImageInfo GetDescriptorImageInfo(int index) const = 0;
  private:
    void InitializeSampler();
  };
}
