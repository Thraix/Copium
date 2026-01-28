#pragma once

#include <vulkan/vulkan.hpp>

#include "copium/util/MetaFile.h"

namespace Copium
{
  class SamplerCreator
  {
    friend class Sampler;

  private:
    VkFilter minFilter{VK_FILTER_LINEAR};
    VkFilter magFilter{VK_FILTER_LINEAR};
    VkSamplerAddressMode addressMode{VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE};

  public:
    SamplerCreator();
    SamplerCreator(const MetaFileClass& metaClass);

    void SetMinFilter(VkFilter minFilter);
    void SetMagFilter(VkFilter magFilter);

  private:
    VkFilter GetFilterFromString(const std::string& str) const;
    VkSamplerAddressMode GetAddressModeFromString(const std::string& str) const;
  };
}
