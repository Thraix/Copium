#pragma once

#include <vulkan/vulkan.hpp>

#include "copium/asset/Asset.h"
#include "copium/sampler/SamplerCreator.h"
#include "copium/util/Common.h"

namespace Copium
{
  class Sampler : public Asset
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(Sampler);

  protected:
    VkSampler sampler;

  public:
    // Sampler();
    Sampler(const SamplerCreator& samplerCreator);
    virtual ~Sampler();

    virtual VkDescriptorImageInfo GetDescriptorImageInfo(int index) const = 0;
    operator VkSampler() const;

  private:
    void InitializeSampler(const SamplerCreator& samplerCreator);
  };
}
