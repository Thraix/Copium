#pragma once

#include "copium/asset/Asset.h"
#include "copium/asset/AssetRef.h"
#include "copium/buffer/CommandBuffer.h"
#include "copium/buffer/Framebuffer.h"
#include "copium/pipeline/DescriptorSet.h"
#include "copium/pipeline/PipelineCreator.h"
#include "copium/util/Common.h"

#include <vector>
#include <vulkan/vulkan.hpp>

namespace Copium
{
  class Pipeline final : public Asset
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(Pipeline);
  private:
    ShaderReflector shaderReflector;
    std::vector<VkDescriptorSetLayout> descriptorSetLayouts{};
    std::vector<VkDescriptorSet> boundDescriptorSets;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    AssetRef<Framebuffer> framebuffer;

  public:

    Pipeline(const MetaFile& metaFile);
    Pipeline(PipelineCreator creator);
    ~Pipeline();
    void Bind(const CommandBuffer& commandBuffer);
    void SetDescriptorSet(const DescriptorSet& descriptorSet);
    void BindDescriptorSets(const CommandBuffer& commandBuffer);

    std::unique_ptr<DescriptorSet> CreateDescriptorSet(DescriptorPool& descriptorPool, int setIndex) const;
    DescriptorSet CreateDescriptorSetRef(DescriptorPool& descriptorPool, int setIndex) const;

  private:
    void InitializeDescriptorSetLayout(const PipelineCreator& creator);
    void InitializePipeline(const PipelineCreator& creator);
  };
}