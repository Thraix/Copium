#include "copium/sampler/Sampler.h"

#include "copium/core/Vulkan.h"

namespace Copium
{
  Sampler::Sampler()
  {
    InitializeSampler();
  }

  Sampler::~Sampler()
  {
    vkDestroySampler(Vulkan::GetDevice(), sampler, nullptr);
  }

  void Sampler::InitializeSampler()
  {
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(Vulkan::GetDevice().GetPhysicalDevice(), &properties);

    VkSamplerCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    createInfo.magFilter = VK_FILTER_LINEAR; // TODO: Some way to control this
    createInfo.minFilter = VK_FILTER_LINEAR; // TODO: Some way to control this
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

    CP_VK_ASSERT(vkCreateSampler(Vulkan::GetDevice(), &createInfo, nullptr, &sampler), "InitializeSampler : Failed to initialize texture sampler");
  }

  Sampler::operator VkSampler() const
  {
    return sampler;
  }
}