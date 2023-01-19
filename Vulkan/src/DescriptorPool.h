#pragma once

#include "Common.h"
#include "Instance.h"
#include <vulkan/vulkan.hpp>

class DescriptorPool final
{
  CP_DELETE_COPY_AND_MOVE_CTOR(DescriptorPool);
private:
  Instance& instance;

  VkDescriptorPool descriptorPool;
  static const int DESCRIPTOR_SET_COUNT = 100;
public:
  DescriptorPool(Instance& instance)
    : instance{instance}
  {
    std::vector<VkDescriptorPoolSize> poolSizes{2};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = DESCRIPTOR_SET_COUNT * instance.GetMaxFramesInFlight(); // TODO: how should this actually be determined?

    poolSizes[1].type = VK_DESCRIPTOR_TYPE_SAMPLER;
    poolSizes[1].descriptorCount = DESCRIPTOR_SET_COUNT * instance.GetMaxFramesInFlight(); // TODO: how should this actually be determined?

    VkDescriptorPoolCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    createInfo.poolSizeCount = poolSizes.size();
    createInfo.pPoolSizes = poolSizes.data();
    createInfo.maxSets = DESCRIPTOR_SET_COUNT * instance.GetMaxFramesInFlight();

    CP_VK_ASSERT(vkCreateDescriptorPool(instance.GetDevice(), &createInfo, nullptr, &descriptorPool), "Failed to initialize descriptor pool");
  }

  ~DescriptorPool()
  {
    vkDestroyDescriptorPool(instance.GetDevice(), descriptorPool, nullptr);
  }

  std::vector<VkDescriptorSet> AllocateDescriptorSets(VkDescriptorSetLayout descriptorSetLayout)
  {
    std::vector<VkDescriptorSet> descriptorSets;
    std::vector<VkDescriptorSetLayout> layouts{static_cast<size_t>(instance.GetMaxFramesInFlight()), descriptorSetLayout};
    VkDescriptorSetAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo.descriptorPool = descriptorPool;
    allocateInfo.descriptorSetCount = instance.GetMaxFramesInFlight();
    allocateInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(instance.GetMaxFramesInFlight());
    CP_VK_ASSERT(vkAllocateDescriptorSets(instance.GetDevice(), &allocateInfo, descriptorSets.data()), "Failed to allocate descriptor sets");

    return descriptorSets;
  }
};
