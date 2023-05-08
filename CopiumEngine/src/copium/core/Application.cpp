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

namespace Copium
{
  const std::vector<Vertex> vertices = {
    Vertex{{-0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    Vertex{{ 0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    Vertex{{ 0.5f, 0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    Vertex{{-0.5f, 0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
    Vertex{{-0.5f, 0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    Vertex{{ 0.5f, 0.0f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    Vertex{{ 0.5f, 0.0f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    Vertex{{-0.5f, 0.0f,  0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
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
    InitializeRenderer();
    InitializeGraphicsPipeline();
    InitializeTextureSampler();
    InitializeDescriptorSets();
    InitializeMesh();
    InitializeCommandBuffer();
  }

  Application::~Application()
  {
    vkDeviceWaitIdle(Vulkan::GetDevice());
    AssetManager::UnloadAsset(texture2D);
    AssetManager::UnloadAsset(texture2D2);
    AssetManager::UnloadAsset(font);
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
    if (timer.Elapsed() >= 1.0)
    {
      fps = frameCounter;
      frameCounter = 0;
      timer.Start(); // Not quite accurate since the elapsed time might me 1.1, then we lose 0.1 precision
    }
    frameCounter++;

    return !glfwWindowShouldClose(Vulkan::GetWindow().GetWindow());
  }

  EventResult Application::OnEvent(const Event& event)
  {
    switch (event.GetType())
    {
    case EventType::WindowResize:
    {
      const WindowResizeEvent& windowResizeEvent = static_cast<const WindowResizeEvent&>(event);
      AssetManager::GetAsset<Framebuffer>(framebuffer).Resize(windowResizeEvent.GetWidth(), windowResizeEvent.GetHeight());
      descriptorSetPassthrough->SetSampler(AssetManager::GetAsset<Framebuffer>(framebuffer).GetColorAttachment(), 0);

      return EventResult::Continue;
    }
    case EventType::MouseMove:
    {
      const MouseMoveEvent& mouseMoveEvent = static_cast<const MouseMoveEvent&>(event);

      float aspect = Vulkan::GetSwapChain().GetExtent().width / (float)Vulkan::GetSwapChain().GetExtent().height;
      mousePos = {(mouseMoveEvent.GetPos().x / Vulkan::GetSwapChain().GetExtent().width - 0.5) * 2.0 * aspect,
                  -(mouseMoveEvent.GetPos().y / Vulkan::GetSwapChain().GetExtent().height - 0.5) * 2.0};

      return EventResult::Continue;
    }
    case EventType::MousePress:
    {
      const MousePressEvent& mousePressEvent = static_cast<const MousePressEvent&>(event);
      CP_INFO("%d", mousePressEvent.GetButton());

      return EventResult::Focus;
    }
    case EventType::KeyPress:
    {
      const KeyPressEvent& keyPressEvent = static_cast<const KeyPressEvent&>(event);
      CP_INFO("%d", keyPressEvent.GetButton());

      return EventResult::Handled;
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

  void Application::InitializeRenderer()
  {
    renderer = std::make_unique<Renderer>();
  }

  void Application::InitializeTextureSampler()
  {
    texture2D = AssetManager::LoadAsset<Texture2D>("fox.meta");
    texture2D2 = AssetManager::LoadAsset<Texture2D>("fox2.meta");
    font = AssetManager::LoadAsset<Font>("font.meta");
  }

  void Application::InitializeDescriptorSets()
  {
    descriptorPool = std::make_unique<DescriptorPool>();

    descriptorSet = AssetManager::GetAsset<Pipeline>(graphicsPipeline).CreateDescriptorSet(*descriptorPool, 0);
    descriptorSet->SetSampler(AssetManager::GetAsset<Texture2D>(texture2D), 1);

    descriptorSetPassthrough = AssetManager::GetAsset<Pipeline>(graphicsPipelinePassthrough).CreateDescriptorSet(*descriptorPool, 0);
    descriptorSetPassthrough->SetSampler(AssetManager::GetAsset<Framebuffer>(framebuffer).GetColorAttachment(), 0);

    descriptorSetRenderer = renderer->GetGraphicsPipeline().CreateDescriptorSet(*descriptorPool, 1);
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
    commandBuffer->Begin();

    Framebuffer& fb = AssetManager::GetAsset<Framebuffer>(framebuffer);
    Pipeline& pl = AssetManager::GetAsset<Pipeline>(graphicsPipeline);
    fb.Bind(*commandBuffer);
    pl.Bind(*commandBuffer);

    UpdateUniformBuffer();

    pl.SetDescriptorSet(*descriptorSet);
    pl.BindDescriptorSets(*commandBuffer);

    mesh->Bind(*commandBuffer);
    mesh->Render(*commandBuffer);

    renderer->SetDescriptorSet(*descriptorSetRenderer);
    renderer->Begin(*commandBuffer);
    for (int y = 0; y < 10; y++)
    {
      for (int x = 0; x < 10; x++)
      {
        renderer->Quad(glm::vec2{-1 + x * 0.2 + 0.05, -1 + y * 0.2 + 0.05}, glm::vec2{0.1, 0.1}, glm::vec3{x * 0.1, y * 0.1, 1.0});
      }
    }
    float aspect = fb.GetWidth() / (float)fb.GetHeight();
    renderer->Quad(glm::vec2{-0.8, -0.4}, glm::vec2{0.8, 0.8}, AssetManager::GetAsset<Texture2D>(texture2D));
    renderer->Quad(glm::vec2{ 0.1, -0.4}, glm::vec2{0.8, 0.8}, AssetManager::GetAsset<Font>(font));
    renderer->Quad(mousePos - glm::vec2(0.1), glm::vec2{0.2}, AssetManager::GetAsset<Texture2D>(texture2D2));
    std::string s = std::to_string(fps) + " fps";
    renderer->Text(s, glm::vec2{-aspect + 0.01, 0.94}, AssetManager::GetAsset<Font>(font), 0.06, glm::vec3{0.3, 0.2, 0.8});
    renderer->End();

    fb.Unbind(*commandBuffer);

    Vulkan::GetSwapChain().BeginFrameBuffer(*commandBuffer);

    Pipeline& plPassthrough = AssetManager::GetAsset<Pipeline>(graphicsPipelinePassthrough);
    plPassthrough.Bind(*commandBuffer);
    plPassthrough.SetDescriptorSet(*descriptorSetPassthrough);
    plPassthrough.BindDescriptorSets(*commandBuffer);

    meshPassthrough->Bind(*commandBuffer);
    meshPassthrough->Render(*commandBuffer);

    Vulkan::GetSwapChain().EndFrameBuffer(*commandBuffer);
    commandBuffer->End();
  }

  void Application::UpdateUniformBuffer()
  {
    static Timer startTimer;

    float time = startTimer.Elapsed();
    Framebuffer& fb = AssetManager::GetAsset<Framebuffer>(framebuffer);
    float aspect = fb.GetWidth() / (float)fb.GetHeight();
    time = 0;

    {
      glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 10.0f);
      projection[1][1] *= -1;

      UniformBuffer& uniformBuffer = descriptorSet->GetUniformBuffer("ubo");
      uniformBuffer.Set("projection", projection);
      uniformBuffer.Set("view", glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
      uniformBuffer.Set("model", glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
      uniformBuffer.Set("lightPos", (glm::vec3)(glm::rotate(glm::mat4{1.0f}, time * glm::radians(45.0f), glm::vec3(0, 1, 0)) * glm::vec4{0.3, 0.1, 0, 1}));
      uniformBuffer.Update();
    }

    {
      UniformBuffer& uniformBuffer = descriptorSetRenderer->GetUniformBuffer("ubo");
      uniformBuffer.Set("projection", glm::ortho(-aspect, aspect, 1.0f, -1.0f));
      // uniformBuffer.Set("view", glm::translate(glm::mat4(1), glm::vec3(0.1 * glm::sin(4 * time), 0.1 * glm::cos(4 * time), 0.0)));
      uniformBuffer.Set("view", glm::mat4(1));
      uniformBuffer.Update();
    }
  }
}