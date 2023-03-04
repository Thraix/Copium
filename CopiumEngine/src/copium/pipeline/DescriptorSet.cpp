#include "copium/pipeline/DescriptorSet.h"

#include "copium/core/Device.h"
#include "copium/core/SwapChain.h"

namespace Copium
{
  DescriptorSet::DescriptorSet(Vulkan& vulkan, DescriptorPool& descriptorPool, VkDescriptorSetLayout descriptorSetLayout)
    : vulkan{vulkan}, descriptorPool{descriptorPool}, descriptorSetLayout{descriptorSetLayout}
  {
    descriptorSets = descriptorPool.AllocateDescriptorSets(descriptorSetLayout);
  }

  DescriptorSet::~DescriptorSet()
  {
    descriptorPool.FreeDescriptorSets(descriptorSets);
  }

  void DescriptorSet::AddUniform(const UniformBuffer& uniformBuffer, uint32_t binding)
  {
    for (size_t i = 0; i < descriptorSets.size(); ++i) {
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
      vkUpdateDescriptorSets(vulkan.GetDevice(), 1, &descriptorWrite, 0, nullptr);
    }
  }

  void DescriptorSet::AddSampler(const Sampler& sampler, uint32_t binding)
  {
    for (size_t i = 0; i < descriptorSets.size(); ++i) {
      VkDescriptorImageInfo imageInfo = sampler.GetDescriptorImageInfo(i);
      VkWriteDescriptorSet descriptorWrite{};
      descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      descriptorWrite.dstSet = descriptorSets[i];
      descriptorWrite.dstBinding = binding;
      descriptorWrite.dstArrayElement = 0;
      descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      descriptorWrite.descriptorCount = 1;
      descriptorWrite.pBufferInfo = nullptr;
      descriptorWrite.pImageInfo = &imageInfo;
      descriptorWrite.pTexelBufferView = nullptr;
      vkUpdateDescriptorSets(vulkan.GetDevice(), 1, &descriptorWrite, 0, nullptr);
    }
  }

  DescriptorSet::operator VkDescriptorSet() const
  {
    return descriptorSets[vulkan.GetSwapChain().GetFlightIndex()];
  }
}