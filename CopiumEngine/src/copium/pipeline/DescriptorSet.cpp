#include "copium/pipeline/DescriptorSet.h"

#include "copium/core/Vulkan.h"

namespace Copium
{
  DescriptorSet::DescriptorSet(DescriptorPool& descriptorPool, VkDescriptorSetLayout descriptorSetLayout, const std::set<ShaderBinding>& bindings)
    : descriptorPool{descriptorPool}, descriptorSetLayout{descriptorSetLayout}, bindings{bindings}
  {
    CP_ASSERT(!bindings.empty(), "DescriptorSet : cannot initialize DescriptorSet with empty ShaderBindings");

    descriptorSets = descriptorPool.AllocateDescriptorSets(descriptorSetLayout);
    for (auto& binding : bindings)
    {
      if (binding.bindingType == BindingType::UniformBuffer)
      {
        std::unique_ptr<UniformBuffer> uniformBuffer = std::make_unique<UniformBuffer>(binding);
        SetUniformBuffer(*uniformBuffer, binding.binding);
        uniformBuffers.emplace(binding.name, std::move(uniformBuffer));
      }
    }
    // TODO: Set default samplers and uniforms?
  }

  DescriptorSet::~DescriptorSet()
  {
    descriptorPool.FreeDescriptorSets(descriptorSets);
  }

  void DescriptorSet::SetSampler(const Sampler& sampler, uint32_t binding, int arrayIndex)
  {
    for (size_t i = 0; i < descriptorSets.size(); ++i)
    {
      VkDescriptorImageInfo imageInfo = sampler.GetDescriptorImageInfo(i);
      VkWriteDescriptorSet descriptorWrite{};
      descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      descriptorWrite.dstSet = descriptorSets[i];
      descriptorWrite.dstBinding = binding;
      descriptorWrite.dstArrayElement = arrayIndex;
      descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      descriptorWrite.descriptorCount = 1;
      descriptorWrite.pBufferInfo = nullptr;
      descriptorWrite.pImageInfo = &imageInfo;
      descriptorWrite.pTexelBufferView = nullptr;
      vkUpdateDescriptorSets(Vulkan::GetDevice(), 1, &descriptorWrite, 0, nullptr);
    }
  }

  void DescriptorSet::SetSamplerDynamic(const Sampler& sampler, uint32_t binding, int arrayIndex)
  {
    VkDescriptorImageInfo imageInfo = sampler.GetDescriptorImageInfo(Vulkan::GetSwapChain().GetFlightIndex());
    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = descriptorSets[Vulkan::GetSwapChain().GetFlightIndex()];
    descriptorWrite.dstBinding = binding;
    descriptorWrite.dstArrayElement = arrayIndex;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pBufferInfo = nullptr;
    descriptorWrite.pImageInfo = &imageInfo;
    descriptorWrite.pTexelBufferView = nullptr;
    vkUpdateDescriptorSets(Vulkan::GetDevice(), 1, &descriptorWrite, 0, nullptr);
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
      vkUpdateDescriptorSets(Vulkan::GetDevice(), descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
    }
  }

  UniformBuffer& DescriptorSet::GetUniformBuffer(const std::string& uniformBuffer)
  {
    auto it = uniformBuffers.find(uniformBuffer);
    CP_ASSERT(it != uniformBuffers.end(), "GetUniformBuffer : UniformBuffer not found = %s", uniformBuffer.c_str());
    return *it->second;
  }

  uint32_t DescriptorSet::GetSetIndex() const
  {
    return bindings.begin()->set;
  }

  DescriptorSet::operator VkDescriptorSet() const
  {
    return descriptorSets[Vulkan::GetSwapChain().GetFlightIndex()];
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
      vkUpdateDescriptorSets(Vulkan::GetDevice(), 1, &descriptorWrite, 0, nullptr);
    }
  }

}