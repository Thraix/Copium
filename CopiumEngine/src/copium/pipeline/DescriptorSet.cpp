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

  void DescriptorSet::SetUniformBuffer(const UniformBuffer& uniformBuffer, uint32_t binding)
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

  void DescriptorSet::SetSampler(const Sampler& sampler, uint32_t binding, int arrayIndex)
  {
    for (size_t i = 0; i < descriptorSets.size(); ++i)
    {
      SetSampler(sampler, binding, i, arrayIndex);
    }
  }

  void DescriptorSet::SetSampler(const Sampler& sampler, uint32_t binding, int index, int arrayIndex)
  {
    CP_ASSERT(index >= 0 && index < descriptorSets.size(), "AddSampler : index is out of range");
    VkDescriptorImageInfo imageInfo = sampler.GetDescriptorImageInfo(index);
    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = descriptorSets[index];
    descriptorWrite.dstBinding = binding;
    descriptorWrite.dstArrayElement = arrayIndex;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pBufferInfo = nullptr;
    descriptorWrite.pImageInfo = &imageInfo;
    descriptorWrite.pTexelBufferView = nullptr;
    vkUpdateDescriptorSets(vulkan.GetDevice(), 1, &descriptorWrite, 0, nullptr);
  }

  void DescriptorSet::SetSamplers(const std::vector<const Sampler*>& samplers, uint32_t binding)
  {
    for (size_t i = 0; i < descriptorSets.size(); ++i) {
      std::vector<VkWriteDescriptorSet> descriptorWrites{samplers.size()};
      for (size_t j = 0; j < samplers.size(); j++)
      {
        VkDescriptorImageInfo imageInfo = samplers[j]->GetDescriptorImageInfo(i);
        descriptorWrites[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[j].dstSet = descriptorSets[i];
        descriptorWrites[j].dstBinding = binding;
        descriptorWrites[j].dstArrayElement = j;
        descriptorWrites[j].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[j].descriptorCount = 1;
        descriptorWrites[j].pBufferInfo = nullptr;
        descriptorWrites[j].pImageInfo = &imageInfo;
        descriptorWrites[j].pTexelBufferView = nullptr;
      }
      vkUpdateDescriptorSets(vulkan.GetDevice(), descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
    }
  }

  DescriptorSet::operator VkDescriptorSet() const
  {
    return descriptorSets[vulkan.GetSwapChain().GetFlightIndex()];
  }
}