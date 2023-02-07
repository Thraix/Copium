#pragma once

#include "copium/buffer/UniformBuffer.h"
#include "copium/pipeline/DescriptorPool.h"
#include "copium/sampler/Sampler.h"
#include "copium/util/Common.h"

#include <vulkan/vulkan.hpp>

namespace Copium
{
  class DescriptorSet final
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(DescriptorSet);
  private:
    Instance& instance;
    DescriptorPool& descriptorPool;
    VkDescriptorSetLayout descriptorSetLayout;

    std::vector<VkDescriptorSet> descriptorSets;

  public:
    DescriptorSet(Instance& instance, DescriptorPool& descriptorPool, VkDescriptorSetLayout descriptorSetLayout);
    ~DescriptorSet();

    void AddUniform(const UniformBuffer& uniformBuffer, uint32_t binding);
    void AddSampler(const Sampler& sampler, uint32_t binding);
    operator VkDescriptorSet() const;
  };
}