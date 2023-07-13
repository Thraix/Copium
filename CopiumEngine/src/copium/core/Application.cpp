#include "copium/core/Application.h"

#include "copium/asset/AssetManager.h"
#include "copium/core/Vulkan.h"
#include "copium/event/EventDispatcher.h"
#include "copium/event/Input.h"
#include "copium/event/KeyPressEvent.h"
#include "copium/event/MouseMoveEvent.h"
#include "copium/event/MousePressEvent.h"
#include "copium/event/MouseScrollEvent.h"
#include "copium/event/ViewportResize.h"
#include "copium/event/WindowFocusEvent.h"
#include "copium/mesh/Vertex.h"
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
    case EventType::ViewportResize:
    {
      const ViewportResize& viewportResizeEvent = static_cast<const ViewportResize&>(event);
      AssetManager::GetAsset<Framebuffer>(framebuffer).Resize(viewportResizeEvent.GetViewport().GetSize().x, viewportResizeEvent.GetViewport().GetSize().y);
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

    descriptorSetImGui = Vulkan::GetImGuiInstance().CreateDescriptorSet();
    descriptorSetImGui->SetSampler(AssetManager::GetAsset<Framebuffer>(framebuffer).GetColorAttachment(), 0);
  }

  void Application::InitializeGraphicsPipeline()
  {
    graphicsPipeline = AssetManager::LoadAsset<Pipeline>("pipeline.meta");
  }

  void Application::InitializeMesh()
  {
    mesh = std::make_unique<Mesh>(vertices, indices);
  }

  void Application::InitializeCommandBuffer()
  {
    commandBuffer = std::make_unique<CommandBuffer>(CommandBufferType::Dynamic);
  }

  void Application::RecordCommandBuffer()
  {
    Framebuffer& fb = AssetManager::GetAsset<Framebuffer>(framebuffer);

    // TODO: Move this logic elsewhere
    Vulkan::GetImGuiInstance().Begin();

    ImGui::SetNextWindowPos(ImVec2{0, 0});
    ImGui::SetNextWindowSize(ImVec2{(float)Vulkan::GetWindow().GetWidth(), (float)Vulkan::GetWindow().GetHeight()});
    ImGui::Begin("Docker", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);
    ImGui::DockSpace(ImGui::GetID("Dockspace"));
    ImGui::End();
    ImGui::Begin("Viewport");
    BoundingBox viewport{ImGui::GetCursorScreenPos().x,
                         ImGui::GetCursorScreenPos().y + ImGui::GetContentRegionAvail().y,
                         ImGui::GetCursorScreenPos().x + ImGui::GetContentRegionAvail().x,
                         ImGui::GetCursorScreenPos().y};
    if (viewport.GetSize() != glm::vec2{fb.GetWidth(), fb.GetHeight()})
    {
      OnEvent(ViewportResize(viewport));
      CP_INFO("Viewport resize");
    }
    ImGui::Image(*descriptorSetImGui, ImGui::GetContentRegionAvail(), ImVec2{0, 1}, ImVec2{1, 0});
    ImGui::End();

    commandBuffer->Begin();

    Pipeline& pl = AssetManager::GetAsset<Pipeline>(graphicsPipeline);
    fb.Bind(*commandBuffer);
    pl.Bind(*commandBuffer);

    UpdateUniformBuffer();

    pl.SetDescriptorSet(*descriptorSet);
    pl.BindDescriptorSets(*commandBuffer);

    mesh->Bind(*commandBuffer);
    mesh->Render(*commandBuffer);

    // TODO: Move this logic elsewhere and only have the Rendering part here
    Input::PushViewport(viewport);
    scene->Update();
    Input::PopViewport();

    fb.Unbind(*commandBuffer);

    Vulkan::GetSwapChain().BeginFrameBuffer(*commandBuffer);

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

    UniformBuffer& uniformBuffer = descriptorSet->GetUniformBuffer("ubo");
    uniformBuffer.Set("projection", glm::perspective(glm::radians(45.0f), aspect, 0.1f, 10.0f));
    uniformBuffer.Set("view", glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
    uniformBuffer.Set("model", glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
    uniformBuffer.Set("lightPos", (glm::vec3)(glm::rotate(glm::mat4{1.0f}, time * glm::radians(45.0f), glm::vec3(0, 1, 0)) * glm::vec4{0.3, 0.1, 0, 1}));
    uniformBuffer.Update();
  }
}