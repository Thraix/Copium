#include "copium/pipeline/DescriptorPool.h"

#include "copium/core/Vulkan.h"

namespace Copium
{
  DescriptorPool::DescriptorPool(int uniformDescriptorSets, int imageDescriptorSets)
  {
    std::vector<VkDescriptorPoolSize> poolSizes;
    if (uniformDescriptorSets != 0)
    {
      VkDescriptorPoolSize descriptorSetPoolSize;
      descriptorSetPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      descriptorSetPoolSize.descriptorCount = uniformDescriptorSets;
      poolSizes.emplace_back(descriptorSetPoolSize);
    }

    if (imageDescriptorSets != 0)
    {
      VkDescriptorPoolSize descriptorSetPoolSize;
      descriptorSetPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      descriptorSetPoolSize.descriptorCount = imageDescriptorSets;
      poolSizes.emplace_back(descriptorSetPoolSize);
    }


    VkDescriptorPoolCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    createInfo.poolSizeCount = poolSizes.size();
    createInfo.pPoolSizes = poolSizes.data();
    createInfo.maxSets = uniformDescriptorSets + imageDescriptorSets; // I have no actual idea if this is fine
    createInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

    CP_VK_ASSERT(vkCreateDescriptorPool(Vulkan::GetDevice(), &createInfo, nullptr, &descriptorPool), "Failed to initialize descriptor pool");
  }

  DescriptorPool::~DescriptorPool()
  {
    VkDescriptorPool descriptorPoolCpy = descriptorPool;
    Vulkan::GetDevice().QueueIdleCommand([descriptorPoolCpy]() {
      vkDestroyDescriptorPool(Vulkan::GetDevice(), descriptorPoolCpy, nullptr);
    });
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
    VkDescriptorPool descriptorPoolCpy = descriptorPool;
    std::vector<VkDescriptorSet> descriptorSetsCpy = descriptorSets;
    Vulkan::GetDevice().QueueIdleCommand([descriptorPoolCpy, descriptorSetsCpy]() {
      vkFreeDescriptorSets(Vulkan::GetDevice(), descriptorPoolCpy, descriptorSetsCpy.size(), descriptorSetsCpy.data());
    });
  }

  DescriptorPool::operator VkDescriptorPool() const
  {
    return descriptorPool;
  }
}
