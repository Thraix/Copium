#include "copium/core/Application.h"

#include "copium/core/Vulkan.h"
#include "copium/mesh/Vertex.h"
#include "copium/mesh/VertexPassthrough.h"

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
  }

  bool Application::Update()
  {
    if (framebuffer->GetWidth() != Vulkan::GetSwapChain().GetExtent().width || framebuffer->GetHeight() != Vulkan::GetSwapChain().GetExtent().height)
    {
      framebuffer->Resize(Vulkan::GetSwapChain().GetExtent().width / 8, Vulkan::GetSwapChain().GetExtent().height / 8);
      descriptorSetPassthrough->SetSampler(framebuffer->GetColorAttachment(), 0);
    }

    if (!Vulkan::GetSwapChain().BeginPresent())
      return true;

    RecordCommandBuffer();
    Vulkan::GetSwapChain().SubmitToGraphicsQueue(*commandBuffer);

    Vulkan::GetSwapChain().EndPresent();
    return !glfwWindowShouldClose(Vulkan::GetWindow().GetWindow());
  }

  void Application::InitializeFrameBuffer()
  {
    framebuffer = std::make_unique<Framebuffer>(Vulkan::GetSwapChain().GetExtent().width, Vulkan::GetSwapChain().GetExtent().height);
  }

  void Application::InitializeRenderer()
  {
    renderer = std::make_unique<Renderer>(framebuffer->GetRenderPass());
  }

  void Application::InitializeTextureSampler()
  {
    texture2D = std::make_unique<Texture2D>("res/textures/texture.png");
    texture2D2 = std::make_unique<Texture2D>("res/textures/texture2.png");
  }

  void Application::InitializeDescriptorSets()
  {
    descriptorPool = std::make_unique<DescriptorPool>();

    descriptorSet = graphicsPipeline->CreateDescriptorSet(*descriptorPool, 0);
    descriptorSet->SetSampler(*texture2D, 1);

    descriptorSetPassthrough = graphicsPipelinePassthrough->CreateDescriptorSet(*descriptorPool, 0);
    descriptorSetPassthrough->SetSampler(framebuffer->GetColorAttachment(), 0);

    descriptorSetRenderer = renderer->GetGraphicsPipeline().CreateDescriptorSet(*descriptorPool, 1);
  }

  void Application::InitializeGraphicsPipeline()
  {
    PipelineCreator creator{framebuffer->GetRenderPass(), "res/shaders/shader.vert", "res/shaders/shader.frag"};
    creator.SetVertexDescriptor(Vertex::GetDescriptor());
    graphicsPipeline = std::make_unique<Pipeline>(creator);

    PipelineCreator creatorPassthrough{Vulkan::GetSwapChain().GetRenderPass(), "res/shaders/passthrough.vert", "res/shaders/passthrough.frag"};
    creatorPassthrough.SetVertexDescriptor(VertexPassthrough::GetDescriptor());
    graphicsPipelinePassthrough = std::make_unique<Pipeline>(creatorPassthrough);
  }

  void Application::InitializeMesh()
  {
    mesh = std::make_unique<Mesh>(vertices, indices);
    meshPassthrough = std::make_unique<Mesh>(verticesPassthrough, indicesPassthrough);
  }

  void Application::InitializeCommandBuffer()
  {
    commandBuffer = std::make_unique<CommandBuffer>(CommandBuffer::Type::Dynamic);
  }

  void Application::RecordCommandBuffer()
  {
    commandBuffer->Begin();

    framebuffer->Bind(*commandBuffer);
    graphicsPipeline->Bind(*commandBuffer);

    UpdateUniformBuffer();

    graphicsPipeline->SetDescriptorSet(*descriptorSet);
    graphicsPipeline->BindDescriptorSets(*commandBuffer);

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
    renderer->Quad(glm::vec2{-0.9, -0.4}, glm::vec2{0.8, 0.8}, *texture2D);
    renderer->Quad(glm::vec2{ 0.1, -0.4}, glm::vec2{0.8, 0.8}, *texture2D2);
    renderer->End();

    framebuffer->Unbind(*commandBuffer);

    Vulkan::GetSwapChain().BeginFrameBuffer(*commandBuffer);

    graphicsPipelinePassthrough->Bind(*commandBuffer);
    graphicsPipelinePassthrough->SetDescriptorSet(*descriptorSetPassthrough);
    graphicsPipelinePassthrough->BindDescriptorSets(*commandBuffer);

    meshPassthrough->Bind(*commandBuffer);
    meshPassthrough->Render(*commandBuffer);

    Vulkan::GetSwapChain().EndFrameBuffer(*commandBuffer);
    commandBuffer->End();
  }

  void Application::UpdateUniformBuffer()
  {
    static Timer startTimer;

    float time = startTimer.Elapsed();
    float aspect = framebuffer->GetWidth() / (float)framebuffer->GetHeight();

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
      uniformBuffer.Set("projection", glm::ortho(-aspect, aspect, -1.0f, 1.0f));
      uniformBuffer.Set("view", glm::translate(glm::mat4(1), glm::vec3(0.1 * glm::sin(4 * time), 0.1 * glm::cos(4 * time), 0.0)));
      uniformBuffer.Update();
    }
  }
}