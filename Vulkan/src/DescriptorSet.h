#pragma once

#include "Common.h"
#include "DescriptorPool.h"
#include "Sampler.h"
#include "UniformBuffer.h"
#include <vulkan/vulkan.hpp>

class DescriptorSet
{
  CP_DELETE_COPY_AND_MOVE_CTOR(DescriptorSet);
private:
  Instance& instance;
  DescriptorPool& descriptorPool;
  VkDescriptorSetLayout descriptorSetLayout;

  std::vector<VkDescriptorSet> descriptorSets;

public:
  DescriptorSet(Instance& instance, DescriptorPool& descriptorPool, VkDescriptorSetLayout descriptorSetLayout)
    : instance{instance}, descriptorPool{descriptorPool}, descriptorSetLayout{descriptorSetLayout}
  {
    descriptorSets = descriptorPool.AllocateDescriptorSets(descriptorSetLayout);
  }

  void AddUniform(const UniformBuffer& uniformBuffer, uint32_t binding)
  {
    for (size_t i = 0; i < instance.GetMaxFramesInFlight(); ++i) {
      VkDescriptorBufferInfo bufferInfo = uniformBuffer.GetDescriptorBufferInfo(i);

      VkWriteDescriptorSet descriptorWrite{};
      descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      descriptorWrite.dstSet = descriptorSets[i];
      descriptorWrite.dstBinding = binding;
      descriptorWrite.dstArrayElement = 0;
      descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      descriptorWrite.descriptorCount = 1;
      descriptorWrite.pBufferInfo = &bufferInfo;
      descriptorWrite.pImageInfo = nullptr;
      descriptorWrite.pTexelBufferView = nullptr;
      vkUpdateDescriptorSets(instance.GetDevice(), 1, &descriptorWrite, 0, nullptr);
    }
  }

  void AddSampler(const Sampler& sampler, uint32_t binding)
  {
    VkDescriptorImageInfo imageInfo = sampler.GetDescriptorImageInfo();
    for (auto&& descriptorSet : descriptorSets) {
      VkWriteDescriptorSet descriptorWrite{};
      descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      descriptorWrite.dstSet = descriptorSet;
      descriptorWrite.dstBinding = binding;
      descriptorWrite.dstArrayElement = 0;
      descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      descriptorWrite.descriptorCount = 1;
      descriptorWrite.pBufferInfo = nullptr;
      descriptorWrite.pImageInfo = &imageInfo;
      descriptorWrite.pTexelBufferView = nullptr;
      vkUpdateDescriptorSets(instance.GetDevice(), 1, &descriptorWrite, 0, nullptr);
    }
  }

  VkDescriptorSet GetHandle() const
  {
    return descriptorSets[instance.GetFlightIndex()];
  }
};