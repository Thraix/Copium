#pragma once

#include "Common.h"
#include "Instance.h"
#include "Pipeline.h"

class Sampler
{
  CP_DELETE_COPY_AND_MOVE_CTOR(Sampler);
private:
  Instance& instance;
  VkImageView imageView;

  VkSampler textureSampler;
public:
  Sampler(Instance& instance, VkImageView imageView)
    : instance{instance}, imageView{imageView}
  {
    InitializeSampler(imageView);
  }

  ~Sampler()
  {
    vkDestroySampler(instance.GetDevice(), textureSampler, nullptr);
  }

  VkDescriptorImageInfo GetDescriptorImageInfo() const
  {
    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = imageView;
    imageInfo.sampler = textureSampler;

    return imageInfo;
  }
private:
  void InitializeSampler(VkImageView imageView)
  {
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(instance.GetPhysicalDevice(), &properties);

    VkSamplerCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    createInfo.magFilter = VK_FILTER_LINEAR;
    createInfo.minFilter = VK_FILTER_LINEAR;
    createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    createInfo.anisotropyEnable = VK_TRUE;
    createInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    createInfo.unnormalizedCoordinates = VK_FALSE;
    createInfo.compareEnable = VK_FALSE;
    createInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    createInfo.mipLodBias = 0.0f;
    createInfo.minLod = 0.0f;
    createInfo.maxLod = 0.0f;

    CP_VK_ASSERT(vkCreateSampler(instance.GetDevice(), &createInfo, nullptr, &textureSampler), "Failed to initialize texture sampler");
  }
};
