#pragma once

#include "Common.h"
#include "Instance.h"

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
    Sampler(Instance& instance)
      : instance{instance}
    {
      InitializeSampler();
    }

    virtual ~Sampler()
    {
      vkDestroySampler(instance.GetDevice(), sampler, nullptr);
    }

    void InitializeSampler()
    {
      VkPhysicalDeviceProperties properties{};
      vkGetPhysicalDeviceProperties(instance.GetPhysicalDevice(), &properties);

      VkSamplerCreateInfo createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
      createInfo.magFilter = VK_FILTER_LINEAR;
      createInfo.minFilter = VK_FILTER_LINEAR;
      createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
      createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
      createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
      createInfo.anisotropyEnable = VK_TRUE;
      createInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
      createInfo.unnormalizedCoordinates = VK_FALSE;
      createInfo.compareEnable = VK_FALSE;
      createInfo.compareOp = VK_COMPARE_OP_ALWAYS;
      createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
      createInfo.mipLodBias = 0.0f;
      createInfo.minLod = 0.0f;
      createInfo.maxLod = 0.0f;

      CP_VK_ASSERT(vkCreateSampler(instance.GetDevice(), &createInfo, nullptr, &sampler), "InitializeSampler : Failed to initialize texture sampler");
    }

    virtual VkDescriptorImageInfo GetDescriptorImageInfo(int index) const = 0;
  };
}
