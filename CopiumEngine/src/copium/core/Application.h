#pragma once

#include "copium/asset/AssetMeta.h"
#include "copium/core/Scene.h"
#include "copium/buffer/Framebuffer.h"
#include "copium/event/EventHandler.h"
#include "copium/mesh/Mesh.h"
#include "copium/pipeline/DescriptorPool.h"
#include "copium/pipeline/DescriptorSet.h"
#include "copium/pipeline/Pipeline.h"
#include "copium/renderer/Renderer.h"

namespace Copium
{
  class Application final : EventHandler
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(Application);
  private:
    AssetHandle framebuffer;
    AssetHandle texture2D;
    AssetHandle texture2D2;
    AssetHandle font;
    AssetHandle graphicsPipeline;
    AssetHandle graphicsPipelinePassthrough;
    std::unique_ptr<DescriptorPool> descriptorPool;
    std::unique_ptr<DescriptorSet> descriptorSet;
    std::unique_ptr<DescriptorSet> descriptorSetPassthrough;
    std::unique_ptr<Scene> scene;
    std::unique_ptr<Mesh> mesh;
    std::unique_ptr<Mesh> meshPassthrough;
    std::unique_ptr<CommandBuffer> commandBuffer;

  public:
    Application();
    ~Application();

    bool Update();
    EventResult OnEvent(const Event& event) override;
  private:
    void InitializeFrameBuffer();
    void InitializeScene();
    void InitializeTextureSampler();
    void InitializeDescriptorSets();
    void InitializeGraphicsPipeline();
    void InitializeMesh();
    void InitializeCommandBuffer();

    void RecordCommandBuffer();
    void UpdateUniformBuffer();
  };
}
