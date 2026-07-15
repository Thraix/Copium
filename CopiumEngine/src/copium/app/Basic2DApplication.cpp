#include "copium/app/Basic2DApplication.h"

#include <glm/gtc/matrix_transform.hpp>
#include <thread>

#include "copium/core/Vulkan.h"
#include "copium/event/EventDispatcher.h"
#include "copium/event/WindowResizeEvent.h"
#include "copium/renderer/RendererVertex.h"

namespace Copium
{
  Basic2DApplication::Basic2DApplication(bool realTimeApplication,
                                         const std::string& vertexShader,
                                         const std::string& fragmentShader)
    : realTimeApplication{realTimeApplication}
  {
    EventDispatcher::AddEventHandler(this);

    PipelineCreator pipelineCreator{Vulkan::GetSwapChain().GetRenderPass(), vertexShader, fragmentShader};
    pipelineCreator.SetVertexDescriptor(RendererVertex::GetDescriptor());
    renderer = std::make_unique<Renderer>(AssetRef<Pipeline>(std::make_unique<Pipeline>(pipelineCreator)));

    Pipeline& pl = renderer->GetGraphicsPipeline();

    commandBuffer = std::make_unique<CommandBuffer>(CommandBufferType::Dynamic);
    descriptorPool = std::make_unique<DescriptorPool>(SwapChain::MAX_FRAMES_IN_FLIGHT * 2, 0);

    projectionDescriptorSet = pl.CreateDescriptorSet(*descriptorPool, 1);
    viewDescriptorSet = pl.CreateDescriptorSet(*descriptorPool, 2);

    float ratio = Vulkan::GetWindow().GetWidth() / (float)Vulkan::GetWindow().GetHeight();
    SetViewMatrixStatic(glm::translate(glm::mat4{1}, glm::vec3{0, 0, 0}));
    SetProjectionMatrixStatic(glm::ortho<float>(0, ratio, 0, 1));

    pl.SetDescriptorSet(*viewDescriptorSet);
    pl.SetDescriptorSet(*projectionDescriptorSet);
  }

  Basic2DApplication::~Basic2DApplication()
  {
    EventDispatcher::RemoveEventHandler(this);
  }

  EventResult Basic2DApplication::OnEvent(const Event& event)
  {
    switch (event.GetType())
    {
      case Copium::EventType::WindowResize:
      {
        const WindowResizeEvent& windowResize = static_cast<const WindowResizeEvent&>(event);
        float ratio = windowResize.GetWidth() / (float)windowResize.GetHeight();
        SetProjectionMatrixStatic(glm::ortho<float>(0, ratio, 0, 1));
        Redraw();
        break;
      }
      default:
        break;
    }
    return EventResult::Continue;
  }

  void Basic2DApplication::PreRender()
  {
  }

  void Basic2DApplication::PostRender()
  {
  }

  bool Basic2DApplication::Update()
  {
    PreRender();
    if (!realTimeApplication && redrawCount <= 0)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
      return !Vulkan::GetWindow().ShouldClose();
    }

    if (!Vulkan::GetSwapChain().BeginPresent())
      return true;

    commandBuffer->Begin();

    Vulkan::GetSwapChain().BeginFrameBuffer(*commandBuffer);
    renderer->Begin(*commandBuffer);
    Render(*renderer);
    renderer->End();
    Vulkan::GetSwapChain().EndFrameBuffer(*commandBuffer);

    commandBuffer->End();

    Vulkan::GetSwapChain().SubmitToGraphicsQueue(*commandBuffer);

    Vulkan::GetSwapChain().EndPresent();
    if (!realTimeApplication)
      redrawCount--;
    PostRender();
    return !Vulkan::GetWindow().ShouldClose();
  }

  void Basic2DApplication::SetViewMatrix(const glm::mat4& viewMatrix)
  {
    UniformBuffer& viewUbo = viewDescriptorSet->GetUniformBuffer("viewUbo");
    viewUbo.Set("view", glm::translate(glm::mat4{1}, glm::vec3{0, 0, 0}));
    viewUbo.Update();
  }

  void Basic2DApplication::SetViewMatrixStatic(const glm::mat4& viewMatrix)
  {
    UniformBuffer& viewUbo = viewDescriptorSet->GetUniformBuffer("viewUbo");
    viewUbo.Set("view", glm::translate(glm::mat4{1}, glm::vec3{0, 0, 0}));
    viewUbo.UpdateStatic();
  }

  void Basic2DApplication::SetProjectionMatrix(const glm::mat4& projectionMatrix)
  {
    UniformBuffer& projectionUbo = projectionDescriptorSet->GetUniformBuffer("projectionUbo");
    projectionUbo.Set("projection", projectionMatrix);
    projectionUbo.Update();
  }

  void Basic2DApplication::SetProjectionMatrixStatic(const glm::mat4& projectionMatrix)
  {
    UniformBuffer& projectionUbo = projectionDescriptorSet->GetUniformBuffer("projectionUbo");
    projectionUbo.Set("projection", projectionMatrix);
    projectionUbo.UpdateStatic();
  }

  void Basic2DApplication::Redraw()
  {
    redrawCount = SwapChain::MAX_FRAMES_IN_FLIGHT;
  }
}
