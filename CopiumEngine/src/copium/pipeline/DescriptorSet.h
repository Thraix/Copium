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
    Vulkan& vulkan;
    DescriptorPool& descriptorPool;
    VkDescriptorSetLayout descriptorSetLayout;

    std::vector<VkDescriptorSet> descriptorSets;

  public:
    DescriptorSet(Vulkan& vulkan, DescriptorPool& descriptorPool, VkDescriptorSetLayout descriptorSetLayout);
    ~DescriptorSet();

    void SetUniformBuffer(const UniformBuffer& uniformBuffer, uint32_t binding);
    void SetSampler(const Sampler& sampler, uint32_t binding, int arrayIndex = 0);
    void SetSampler(const Sampler& sampler, uint32_t binding, int index, int arrayIndex = 0);
    void SetSamplers(const std::vector<const Sampler*>& sampler, uint32_t binding);
    operator VkDescriptorSet() const;
  };
}