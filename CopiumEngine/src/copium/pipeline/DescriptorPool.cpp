#include "copium/pipeline/DescriptorPool.h"

namespace Copium
{
  DescriptorPool::DescriptorPool(Instance& instance)
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
    createInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

    CP_VK_ASSERT(vkCreateDescriptorPool(instance.GetDevice(), &createInfo, nullptr, &descriptorPool), "DescriptorPool : Failed to initialize descriptor pool");
  }

  DescriptorPool::~DescriptorPool()
  {
    vkDestroyDescriptorPool(instance.GetDevice(), descriptorPool, nullptr);
  }

  std::vector<VkDescriptorSet> DescriptorPool::AllocateDescriptorSets(VkDescriptorSetLayout descriptorSetLayout)
  {
    std::vector<VkDescriptorSet> descriptorSets;
    std::vector<VkDescriptorSetLayout> layouts{static_cast<size_t>(instance.GetMaxFramesInFlight()), descriptorSetLayout};
    VkDescriptorSetAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo.descriptorPool = descriptorPool;
    allocateInfo.descriptorSetCount = instance.GetMaxFramesInFlight();
    allocateInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(instance.GetMaxFramesInFlight());
    CP_VK_ASSERT(vkAllocateDescriptorSets(instance.GetDevice(), &allocateInfo, descriptorSets.data()), "AllocateDescriptorSets : Failed to allocate descriptor sets");

    return descriptorSets;
  }

  void DescriptorPool::FreeDescriptorSets(const std::vector<VkDescriptorSet>& descriptorSets)
  {
    vkFreeDescriptorSets(instance.GetDevice(), descriptorPool, descriptorSets.size(), descriptorSets.data());
  }
}