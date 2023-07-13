#include "copium/pipeline/DescriptorPool.h"

#include "copium/core/Vulkan.h"

namespace Copium
{
  DescriptorPool::DescriptorPool()
  {
    std::vector<VkDescriptorPoolSize> poolSizes{2};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = DESCRIPTOR_SET_COUNT * SwapChain::MAX_FRAMES_IN_FLIGHT; // TODO: how should this actually be determined?

    poolSizes[1].type = VK_DESCRIPTOR_TYPE_SAMPLER;
    poolSizes[1].descriptorCount = DESCRIPTOR_SET_COUNT * SwapChain::MAX_FRAMES_IN_FLIGHT; // TODO: how should this actually be determined?

    VkDescriptorPoolCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    createInfo.poolSizeCount = poolSizes.size();
    createInfo.pPoolSizes = poolSizes.data();
    createInfo.maxSets = DESCRIPTOR_SET_COUNT * SwapChain::MAX_FRAMES_IN_FLIGHT;
    createInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

    CP_VK_ASSERT(vkCreateDescriptorPool(Vulkan::GetDevice(), &createInfo, nullptr, &descriptorPool), "Failed to initialize descriptor pool");
  }

  DescriptorPool::~DescriptorPool()
  {
    vkDestroyDescriptorPool(Vulkan::GetDevice(), descriptorPool, nullptr);
  }

  std::vector<VkDescriptorSet> DescriptorPool::AllocateDescriptorSets(VkDescriptorSetLayout descriptorSetLayout)
  {
    std::vector<VkDescriptorSet> descriptorSets{SwapChain::MAX_FRAMES_IN_FLIGHT};
    std::vector<VkDescriptorSetLayout> layouts{SwapChain::MAX_FRAMES_IN_FLIGHT, descriptorSetLayout};
    VkDescriptorSetAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo.descriptorPool = descriptorPool;
    allocateInfo.descriptorSetCount = descriptorSets.size();
    allocateInfo.pSetLayouts = layouts.data();

    CP_VK_ASSERT(vkAllocateDescriptorSets(Vulkan::GetDevice(), &allocateInfo, descriptorSets.data()), "Failed to allocate descriptor sets");

    return descriptorSets;
  }

  void DescriptorPool::FreeDescriptorSets(const std::vector<VkDescriptorSet>& descriptorSets)
  {
    vkFreeDescriptorSets(Vulkan::GetDevice(), descriptorPool, descriptorSets.size(), descriptorSets.data());
  }

  DescriptorPool::operator VkDescriptorPool() const
  {
    return descriptorPool;
  }
}