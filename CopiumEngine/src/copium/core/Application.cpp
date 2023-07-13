#include "copium/core/Application.h"

#include "copium/asset/AssetManager.h"
#include "copium/core/Vulkan.h"
#include "copium/event/EventDispatcher.h"
#include "copium/event/KeyPressEvent.h"
#include "copium/event/MouseMoveEvent.h"
#include "copium/event/MousePressEvent.h"
#include "copium/event/MouseScrollEvent.h"
#include "copium/event/WindowFocusEvent.h"
#include "copium/event/WindowResizeEvent.h"
#include "copium/mesh/Vertex.h"
#include "copium/mesh/VertexPassthrough.h"
#include "copium/sampler/Font.h"

#include <glm/gtc/matrix_transform.hpp>

#include <imgui.h>

namespace Copium
{
  const std::vector<Vertex> vertices = {
    Vertex{{-0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    Vertex{{-0.5f, 0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
    Vertex{{ 0.5f, 0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    Vertex{{ 0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    Vertex{{-0.5f, 0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    Vertex{{-0.5f, 0.0f,  0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
    Vertex{{ 0.5f, 0.0f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    Vertex{{ 0.5f, 0.0f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
  };

  const std::vector<uint16_t> indices = {
      0, 1, 2, 2, 3, 0,
      4, 5, 6, 6, 7, 4
  };

  const std::vector<VertexPassthrough> verticesPassthrough = {
    VertexPassthrough{{-1.0f, -1.0f}},
    VertexPassthrough{{-1.0f,  1.0f}},
    VertexPassthrough{{ 1.0f,  1.0f}},
    VertexPassthrough{{ 1.0f, -1.0f}},
  };

  const std::vector<uint16_t> indicesPassthrough = {
      0, 1, 2, 2, 3, 0,
  };

  Application::Application()
  {
    EventDispatcher::AddEventHandler(this);
    InitializeFrameBuffer();
    InitializeGraphicsPipeline();
    InitializeTextureSampler();
    InitializeDescriptorSets();
    InitializeMesh();
    InitializeCommandBuffer();
    InitializeScene();
  }

  Application::~Application()
  {
    vkDeviceWaitIdle(Vulkan::GetDevice());
    AssetManager::UnloadAsset(texture2D);
    AssetManager::UnloadAsset(graphicsPipeline);
    AssetManager::UnloadAsset(graphicsPipelinePassthrough);
    AssetManager::UnloadAsset(framebuffer);
    EventDispatcher::RemoveEventHandler(this);
  }

  bool Application::Update()
  {
    static Copium::Timer timer;

    if (!Vulkan::GetSwapChain().BeginPresent())
      return true;

    RecordCommandBuffer();

    Vulkan::GetSwapChain().SubmitToGraphicsQueue(*commandBuffer);

    Vulkan::GetSwapChain().EndPresent();
    return !Vulkan::GetWindow().ShouldClose();
  }

  EventResult Application::OnEvent(const Event& event)
  {
    scene->OnEvent(event);
    switch (event.GetType())
    {
    case EventType::WindowResize:
    {
      const WindowResizeEvent& windowResizeEvent = static_cast<const WindowResizeEvent&>(event);
      AssetManager::GetAsset<Framebuffer>(framebuffer).Resize(windowResizeEvent.GetWidth(), windowResizeEvent.GetHeight());
      descriptorSetPassthrough->SetSampler(AssetManager::GetAsset<Framebuffer>(framebuffer).GetColorAttachment(), 0);
      descriptorSetImGui->SetSampler(AssetManager::GetAsset<Framebuffer>(framebuffer).GetColorAttachment(), 0);

      return EventResult::Continue;
    }
    case EventType::MousePress:
    {
      const MousePressEvent& mousePressEvent = static_cast<const MousePressEvent&>(event);
      CP_INFO("%d", mousePressEvent.GetButton());

      return EventResult::Focus;
    }
    case EventType::MouseScroll:
    {
      const MouseScrollEvent& mouseScrollEvent = static_cast<const MouseScrollEvent&>(event);
      CP_INFO("%d %d", mouseScrollEvent.GetScrollX(), mouseScrollEvent.GetScrollY());

      return EventResult::Continue;
    }
    case EventType::WindowFocus:
    {
      const WindowFocusEvent& windowFocusEvent = static_cast<const WindowFocusEvent&>(event);
      CP_INFO("Window Focused: %s", windowFocusEvent.IsFocused() ? "true" : "false");

      return EventResult::Continue;
    }
    }

    return EventResult::Continue;
  }

  void Application::InitializeFrameBuffer()
  {
    framebuffer = AssetManager::LoadAsset<Framebuffer>("framebuffer.meta");
  }

  void Application::InitializeScene()
  {
    scene = std::make_unique<Scene>(*commandBuffer, *descriptorPool);
  }

  void Application::InitializeTextureSampler()
  {
    texture2D = AssetManager::LoadAsset<Texture2D>("fox.meta");
  }

  void Application::InitializeDescriptorSets()
  {
    descriptorPool = std::make_unique<DescriptorPool>();

    descriptorSet = AssetManager::GetAsset<Pipeline>(graphicsPipeline).CreateDescriptorSet(*descriptorPool, 0);
    descriptorSet->SetSampler(AssetManager::GetAsset<Texture2D>(texture2D), 1);

    descriptorSetPassthrough = AssetManager::GetAsset<Pipeline>(graphicsPipelinePassthrough).CreateDescriptorSet(*descriptorPool, 0);
    descriptorSetPassthrough->SetSampler(AssetManager::GetAsset<Framebuffer>(framebuffer).GetColorAttachment(), 0);

    descriptorSetImGui = Vulkan::GetImGuiInstance().CreateDescriptorSet();
    descriptorSetImGui->SetSampler(AssetManager::GetAsset<Framebuffer>(framebuffer).GetColorAttachment(), 0);
  }

  void Application::InitializeGraphicsPipeline()
  {
    graphicsPipeline = AssetManager::LoadAsset<Pipeline>("pipeline.meta");
    graphicsPipelinePassthrough = AssetManager::LoadAsset<Pipeline>("passthrough.meta");
  }

  void Application::InitializeMesh()
  {
    mesh = std::make_unique<Mesh>(vertices, indices);
    meshPassthrough = std::make_unique<Mesh>(verticesPassthrough, indicesPassthrough);
  }

  void Application::InitializeCommandBuffer()
  {
    commandBuffer = std::make_unique<CommandBuffer>(CommandBufferType::Dynamic);
  }

  void Application::RecordCommandBuffer()
  {
    Vulkan::GetImGuiInstance().Begin();
    commandBuffer->Begin();
    ImGui::Begin("Viewport");
    ImGui::Image(*descriptorSetImGui, ImVec2{480, 270}, ImVec2{0, 1}, ImVec2{1, 0});
    ImGui::End();

    Framebuffer& fb = AssetManager::GetAsset<Framebuffer>(framebuffer);
    Pipeline& pl = AssetManager::GetAsset<Pipeline>(graphicsPipeline);
    fb.Bind(*commandBuffer);
    pl.Bind(*commandBuffer);

    UpdateUniformBuffer();

    pl.SetDescriptorSet(*descriptorSet);
    pl.BindDescriptorSets(*commandBuffer);

    mesh->Bind(*commandBuffer);
    mesh->Render(*commandBuffer);

    scene->Update();

    fb.Unbind(*commandBuffer);

    Vulkan::GetSwapChain().BeginFrameBuffer(*commandBuffer);

    Pipeline& plPassthrough = AssetManager::GetAsset<Pipeline>(graphicsPipelinePassthrough);
    plPassthrough.Bind(*commandBuffer);
    plPassthrough.SetDescriptorSet(*descriptorSetPassthrough);
    plPassthrough.BindDescriptorSets(*commandBuffer);

    meshPassthrough->Bind(*commandBuffer);
    meshPassthrough->Render(*commandBuffer);

    Vulkan::GetImGuiInstance().End();
    Vulkan::GetImGuiInstance().Render(*commandBuffer);

    Vulkan::GetSwapChain().EndFrameBuffer(*commandBuffer);
    commandBuffer->End();
  }

  void Application::UpdateUniformBuffer()
  {
    static Timer startTimer;

    float time = startTimer.Elapsed();
    Framebuffer& fb = AssetManager::GetAsset<Framebuffer>(framebuffer);
    float aspect = fb.GetWidth() / (float)fb.GetHeight();

    {
      UniformBuffer& uniformBuffer = descriptorSet->GetUniformBuffer("ubo");
      uniformBuffer.Set("projection", glm::perspective(glm::radians(45.0f), aspect, 0.1f, 10.0f));
      uniformBuffer.Set("view", glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
      uniformBuffer.Set("model", glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
      uniformBuffer.Set("lightPos", (glm::vec3)(glm::rotate(glm::mat4{1.0f}, time * glm::radians(45.0f), glm::vec3(0, 1, 0)) * glm::vec4{0.3, 0.1, 0, 1}));
      uniformBuffer.Update();
    }
  }
}