#pragma once

#include "Common.h"
#include "Buffer.h"
#include "Pipeline.h"
#include <vulkan/vulkan.hpp>

template <typename T>
class UniformBuffer : public Buffer
{
	CP_DELETE_COPY_AND_MOVE_CTOR(UniformBuffer);
private:
  Pipeline& pipeline;

  VkDescriptorPool descriptorPool;
  std::vector<VkDescriptorSet> descriptorSets;

public:
  UniformBuffer(Instance& instance, Pipeline& pipeline, uint32_t binding, VkDescriptorSetLayout layout)
    : Buffer{instance, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, sizeof(T), instance.GetMaxFramesInFlight()}, pipeline{pipeline}
  {
    InitializeDescriptorPool();
    InitializeDescriptorSet(binding, layout);
  }

  ~UniformBuffer() override
  {
    vkDestroyDescriptorPool(instance.GetDevice(), descriptorPool, nullptr);
  }

  void Update(const T& t)
  {
    Buffer::Update((void*)&t, instance.GetFlightIndex());
  }

  void Bind(VkCommandBuffer commandBuffer) const
  {
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.GetPipelineLayout(), 0, 1, &descriptorSets[instance.GetFlightIndex()], 0, nullptr);
  }

private:
  void InitializeDescriptorPool()
  {
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = instance.GetMaxFramesInFlight();

    VkDescriptorPoolCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    createInfo.poolSizeCount = 1;
    createInfo.pPoolSizes = &poolSize;
    createInfo.maxSets = instance.GetMaxFramesInFlight();

    CP_VK_ASSERT(vkCreateDescriptorPool(instance.GetDevice(), &createInfo, nullptr, &descriptorPool), "Failed to initialize descriptor pool");
  }

  void InitializeDescriptorSet(uint32_t binding, VkDescriptorSetLayout layout)
  {
    std::vector<VkDescriptorSetLayout> layouts{static_cast<size_t>(instance.GetMaxFramesInFlight()), layout};
    VkDescriptorSetAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo.descriptorPool = descriptorPool;
    allocateInfo.descriptorSetCount = instance.GetMaxFramesInFlight();
    allocateInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(instance.GetMaxFramesInFlight());
    CP_VK_ASSERT(vkAllocateDescriptorSets(instance.GetDevice(), &allocateInfo, descriptorSets.data()), "Failed to allocate descriptor sets");
    for (size_t i = 0; i < instance.GetMaxFramesInFlight(); ++i) {
      VkDescriptorBufferInfo bufferInfo = GetDescriptorBufferInfo(i);

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

  VkDescriptorBufferInfo GetDescriptorBufferInfo(int index)
  {
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = handle;
    bufferInfo.offset = (VkDeviceSize)index * size;
		bufferInfo.range = size;
    return bufferInfo;
  }
};
