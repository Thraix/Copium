#pragma once

#include "copium/asset/Asset.h"
#include "copium/util/Common.h"

#include <vulkan/vulkan.hpp>

namespace Copium
{
  class Sampler : public Asset
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(Sampler);
  protected:
    VkSampler sampler;
  public:
    Sampler();
    virtual ~Sampler();

    virtual VkDescriptorImageInfo GetDescriptorImageInfo(int index) const = 0;
    operator VkSampler() const;
  private:
    void InitializeSampler();
  };
}
