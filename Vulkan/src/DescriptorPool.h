#pragma once

#include "Common.h"
#include "Instance.h"

#include <vulkan/vulkan.hpp>

namespace Copium
{
  class DescriptorPool final
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(DescriptorPool);
  private:
    Instance& instance;

    VkDescriptorPool descriptorPool;
    static const int DESCRIPTOR_SET_COUNT = 100;
  public:
    DescriptorPool(Instance& instance);
    ~DescriptorPool();

    std::vector<VkDescriptorSet> AllocateDescriptorSets(VkDescriptorSetLayout descriptorSetLayout);
    void FreeDescriptorSets(const std::vector<VkDescriptorSet>& descriptorSets);
  };
}
