#pragma once

#include "copium/buffer/Framebuffer.h"
#include "copium/buffer/IndexBuffer.h"
#include "copium/buffer/UniformBuffer.h"
#include "copium/buffer/VertexBuffer.h"
#include "copium/core/Instance.h"
#include "copium/pipeline/DescriptorPool.h"
#include "copium/pipeline/DescriptorSet.h"
#include "copium/pipeline/Pipeline.h"
#include "copium/sampler/Texture2D.h"

namespace Copium
{
  class Application final
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(Application);
  private:
    std::unique_ptr<Instance> instance;
    std::unique_ptr<Pipeline> graphicsPipeline;
    std::unique_ptr<Texture2D> texture2D;
    std::unique_ptr<UniformBuffer> shaderUniformBuffer;
    std::unique_ptr<DescriptorPool> descriptorPool;
    std::unique_ptr<DescriptorSet> descriptorSet;
    std::unique_ptr<VertexBuffer> vertexBuffer;
    std::unique_ptr<IndexBuffer> indexBuffer;
    std::unique_ptr<CommandBuffer> commandBuffer;

    std::unique_ptr<Framebuffer> framebuffer;
    std::unique_ptr<Pipeline> graphicsPipelinePassthrough;
    std::unique_ptr<VertexBuffer> vertexBufferPassthrough;
    std::unique_ptr<IndexBuffer> indexBufferPassthrough;
    std::unique_ptr<DescriptorSet> descriptorSetPassthrough;

  public:
    Application();
    ~Application();

    bool Update();
  private:
    void InitializeInstance();
    void InitializeFrameBuffer();
    void InitializeTextureSampler();
    void InitializeUniformBuffer();
    void InitializeDescriptorSets();
    void InitializeGraphicsPipeline();
    void InitializeVertexBuffer();
    void InitializeIndexBuffer();
    void InitializeCommandBuffer();
    void RecordCommandBuffer();
    void UpdateUniformBuffer();
  };
}
