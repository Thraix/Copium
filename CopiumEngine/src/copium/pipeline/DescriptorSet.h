#pragma once

#include "copium/buffer/UniformBuffer.h"
#include "copium/pipeline/DescriptorPool.h"
#include "copium/pipeline/ShaderBinding.h"
#include "copium/sampler/Sampler.h"
#include "copium/util/Common.h"

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

#include <map>
#include <set>
#include <vector>

namespace Copium
{
  class DescriptorSet final
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(DescriptorSet);
  private:
    DescriptorPool& descriptorPool;
    VkDescriptorSetLayout descriptorSetLayout;

    std::set<ShaderBinding> bindings;
    std::vector<VkDescriptorSet> descriptorSets;
    std::map<std::string, std::unique_ptr<UniformBuffer>> uniformBuffers;

  public:
    DescriptorSet(DescriptorPool& descriptorPool, VkDescriptorSetLayout descriptorSetLayout, const std::set<ShaderBinding>& bindings);
    ~DescriptorSet();

    void SetSampler(const Sampler& sampler, uint32_t binding, int arrayIndex = 0);
    void SetSamplerDynamic(const Sampler& sampler, uint32_t binding, int arrayIndex = 0);
    void SetSamplers(const std::vector<const Sampler*>& sampler, uint32_t binding);
    void SetSamplersDynamic(const std::vector<const Sampler*>& samplers, uint32_t binding);
    UniformBuffer& GetUniformBuffer(const std::string& uniformBuffer);
    uint32_t GetSetIndex() const;
    VkDescriptorSet GetVkDescriptorSet(int flightIndex) const;

    operator VkDescriptorSet() const;
  private:
    void SetUniformBuffer(const UniformBuffer& uniformBuffer, uint32_t binding);
  };
}
