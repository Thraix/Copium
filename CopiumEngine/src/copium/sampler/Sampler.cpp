#include "copium/sampler/Sampler.h"

#include "copium/core/Vulkan.h"

namespace Copium
{
  Sampler::Sampler(const SamplerCreator& samplerCreator)
  {
    InitializeSampler(samplerCreator);
  }

  Sampler::~Sampler()
  {
    VkSampler samplerCpy = sampler;
    Vulkan::GetDevice().QueueIdleCommand([samplerCpy]() {
      vkDestroySampler(Vulkan::GetDevice(), samplerCpy, nullptr);
    });
  }

  void Sampler::InitializeSampler(const SamplerCreator& samplerCreator)
  {
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(Vulkan::GetDevice().GetPhysicalDevice(), &properties);

    VkSamplerCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    createInfo.magFilter = samplerCreator.magFilter;
    createInfo.minFilter = samplerCreator.minFilter;
    createInfo.addressModeU = samplerCreator.addressMode;
    createInfo.addressModeV = samplerCreator.addressMode;
    createInfo.addressModeW = samplerCreator.addressMode;
    createInfo.anisotropyEnable = VK_TRUE;
    createInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    createInfo.unnormalizedCoordinates = VK_FALSE;
    createInfo.compareEnable = VK_FALSE;
    createInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    createInfo.mipLodBias = 0.0f;
    createInfo.minLod = 0.0f;
    createInfo.maxLod = 0.0f;

    CP_VK_ASSERT(vkCreateSampler(Vulkan::GetDevice(), &createInfo, nullptr, &sampler), "Failed to initialize texture sampler");
  }

  Sampler::operator VkSampler() const
  {
    return sampler;
  }
}