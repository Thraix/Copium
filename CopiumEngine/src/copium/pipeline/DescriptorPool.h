#pragma once

#include <vulkan/vulkan.hpp>

#include "copium/util/Common.h"

namespace Copium
{
  class DescriptorPool final
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(DescriptorPool);

  private:
    VkDescriptorPool descriptorPool;

  public:
    DescriptorPool(int uniformDescriptorSets, int imageDescriptorSets);
    ~DescriptorPool();

    std::vector<VkDescriptorSet> AllocateDescriptorSets(VkDescriptorSetLayout descriptorSetLayout);
    void FreeDescriptorSets(const std::vector<VkDescriptorSet>& descriptorSets);

    operator VkDescriptorPool() const;
  };
}
