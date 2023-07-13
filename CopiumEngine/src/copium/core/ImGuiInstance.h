#pragma once

#include <memory>

#include "copium/buffer/CommandBuffer.h"
#include "copium/pipeline/DescriptorPool.h"
#include "copium/pipeline/DescriptorSet.h"

namespace Copium
{
  class ImGuiInstance
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(ImGuiInstance);

  private:
    std::unique_ptr<DescriptorPool> descriptorPool;
    VkDescriptorSetLayout descriptorSetLayout;
    std::set<ShaderBinding> shaderBindings;

  public:
    ImGuiInstance();
    ~ImGuiInstance();

    void Begin();
    void End();
    void Render(CommandBuffer& commandBuffer);

    std::unique_ptr<DescriptorSet> CreateDescriptorSet();

  private:
    void InitializeImGui();
    void InitializeDescriptorSetLayout();

    static void CheckVkResult(VkResult err);
  };
}
