#pragma once
#include <memory>

#include "copium/app/Application.h"
#include "copium/buffer/CommandBuffer.h"
#include "copium/core/Vulkan.h"
#include "copium/event/EventHandler.h"
#include "copium/renderer/Renderer.h"

namespace Copium
{
  // TODO: Better name?
  class Basic2DApplication : public Application, public EventHandler
  {
  private:
    struct VulkanWrapper
    {
      VulkanWrapper()
      {
        Vulkan::Initialize();
      }

      ~VulkanWrapper()
      {
        Vulkan::Destroy();
      }
    };

  public:
    Basic2DApplication(bool realTimeApplication, const std::string& vertexShader, const std::string& fragmentShader);
    ~Basic2DApplication();

    virtual EventResult OnEvent(const Event& event) override;

    virtual void PreRender();
    virtual void Render(Renderer& renderer) = 0;
    virtual void PostRender();
    bool Update() override;

    void SetViewMatrix(const glm::mat4& viewMatrix);
    void SetViewMatrixStatic(const glm::mat4& viewMatrix);
    void SetProjectionMatrix(const glm::mat4& projectionMatrix);
    void SetProjectionMatrixStatic(const glm::mat4& projectionMatrix);
    void Redraw();

  private:
    bool realTimeApplication;
    int redrawCount{2};

    VulkanWrapper vulkan;
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<CommandBuffer> commandBuffer;
    std::unique_ptr<Copium::DescriptorPool> descriptorPool;
    std::unique_ptr<Copium::DescriptorSet> projectionDescriptorSet;
    std::unique_ptr<Copium::DescriptorSet> viewDescriptorSet;
  };
}
