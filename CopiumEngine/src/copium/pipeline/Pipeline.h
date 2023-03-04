#pragma once

#include "copium/buffer/CommandBuffer.h"
#include "copium/core/Vulkan.h"
#include "copium/pipeline/DescriptorSet.h"
#include "copium/pipeline/PipelineCreator.h"
#include "copium/util/Common.h"

#include <vector>
#include <vulkan/vulkan.hpp>

namespace Copium
{
  class Pipeline final
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(Pipeline);
  private:
    Vulkan& vulkan;

    std::vector<VkDescriptorSetLayout> descriptorSetLayouts{};
    std::vector<VkDescriptorSet> boundDescriptorSets;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;

  public:
    Pipeline(Vulkan& vulkan, PipelineCreator creator);
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