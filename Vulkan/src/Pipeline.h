#pragma once

#include "CommandBuffer.h"
#include "Common.h"
#include "DescriptorSet.h"
#include "Instance.h"
#include "PipelineCreator.h"

#include <vector>
#include <vulkan/vulkan.hpp>

namespace Copium
{
  class Pipeline final
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(Pipeline);
  private:
    Instance& instance;

    std::vector<VkDescriptorSetLayout> descriptorSetLayouts{};
    std::vector<VkDescriptorSet> boundDescriptorSets;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;

  public:
    Pipeline(Instance& instance, PipelineCreator creator);
    ~Pipeline();
    void Bind(const CommandBuffer& commandBuffer);
    void SetDescriptorSet(uint32_t setIndex, const DescriptorSet& descriptorSet);
    void BindDescriptorSets(const CommandBuffer& commandBuffer);
    VkDescriptorSetLayout GetDescriptorSetLayout(uint32_t setIndex) const;

  private:
    void InitializeDescriptorSetLayout(const PipelineCreator& creator);
    void InitializePipeline(const PipelineCreator& creator);
  };
}