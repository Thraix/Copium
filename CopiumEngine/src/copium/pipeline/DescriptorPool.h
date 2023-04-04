#pragma once

#include "copium/util/Common.h"

#include <vulkan/vulkan.hpp>

namespace Copium
{
  class DescriptorPool final
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(DescriptorPool);
  private:
    VkDescriptorPool descriptorPool;
    static const int DESCRIPTOR_SET_COUNT = 100;
  public:
    DescriptorPool();
    ~DescriptorPool();

    std::vector<VkDescriptorSet> AllocateDescriptorSets(VkDescriptorSetLayout descriptorSetLayout);
    void FreeDescriptorSets(const std::vector<VkDescriptorSet>& descriptorSets);
  };
}
