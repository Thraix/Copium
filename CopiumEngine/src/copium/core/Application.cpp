#include "copium/core/Application.h"

#include "copium/mesh/Vertex.h"
#include "copium/mesh/VertexPassthrough.h"
#include "copium/core/Device.h"
#include "copium/core/Instance.h"
#include "copium/core/SwapChain.h"
#include "copium/core/Window.h"

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
    VertexPassthrough{{ 1.0f, -1.0f}},
    VertexPassthrough{{ 1.0f,  1.0f}},
    VertexPassthrough{{-1.0f,  1.0f}},
  };

  const std::vector<uint16_t> indicesPassthrough = {
      0, 1, 2, 2, 3, 0,
  };

  struct alignas(64) ShaderUniform
  {
    alignas(16) glm::mat4 projection;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 model;
    alignas(16) glm::vec3 lightPos;
  };

  Application::Application()
  {
    InitializeVulkan();
    InitializeFrameBuffer();
    InitializeGraphicsPipeline();
    InitializeTextureSampler();
    InitializeUniformBuffer();
    InitializeDescriptorSets();
    InitializeMesh();
    InitializeCommandBuffer();
    InitializeRenderer();
  }

  Application::~Application()
  {
    vkDeviceWaitIdle(vulkan->GetDevice());
  }

  bool Application::Update()
  {
    if (framebuffer->GetWidth() != vulkan->GetSwapChain().GetExtent().width || framebuffer->GetHeight() != vulkan->GetSwapChain().GetExtent().height)
    {
      framebuffer->Resize(vulkan->GetSwapChain().GetExtent().width / 8, vulkan->GetSwapChain().GetExtent().height / 8);
      descriptorSetPassthrough->AddSampler(framebuffer->GetColorAttachment(), 0);
    }

    if (!vulkan->GetSwapChain().BeginPresent())
      return true;

    RecordCommandBuffer();
    vulkan->GetSwapChain().SubmitToGraphicsQueue(*commandBuffer);

    vulkan->GetSwapChain().EndPresent();
    return !glfwWindowShouldClose(vulkan->GetWindow().GetWindow());
  }

  void Application::InitializeVulkan()
  {
    vulkan = std::make_unique<Vulkan>();
    vulkan->SetInstance(std::make_unique<Instance>("Copium Engine"));
    vulkan->SetWindow(std::make_unique<Window>(*vulkan, "Copium Engine", 1920, 1080, Window::Mode::Windowed));
    vulkan->SetDevice(std::make_unique<Device>(*vulkan));
    vulkan->SetSwapChain(std::make_unique<SwapChain>(*vulkan));
    CP_ASSERT(vulkan->Valid(), "Vulkan Manager was not initialized correctly");
  }

  void Application::InitializeFrameBuffer()
  {
    framebuffer = std::make_unique<Framebuffer>(*vulkan, vulkan->GetSwapChain().GetExtent().width, vulkan->GetSwapChain().GetExtent().height);
  }

  void Application::InitializeTextureSampler()
  {
    texture2D = std::make_unique<Texture2D>(*vulkan, "res/textures/texture.png");
    texture2D2 = std::make_unique<Texture2D>(*vulkan, "res/textures/texture2.png");
  }

  void Application::InitializeUniformBuffer()
  {
    shaderUniformBuffer = std::make_unique<UniformBuffer>(*vulkan, sizeof(ShaderUniform));
  }

  void Application::InitializeDescriptorSets()
  {
    descriptorPool = std::make_unique<DescriptorPool>(*vulkan);

    descriptorSet = std::make_unique<DescriptorSet>(*vulkan, *descriptorPool, graphicsPipeline->GetDescriptorSetLayout(0));
    descriptorSet->AddUniform(*shaderUniformBuffer, 0);
    descriptorSet->AddSampler(*texture2D, 1);

    descriptorSetPassthrough = std::make_unique<DescriptorSet>(*vulkan, *descriptorPool, graphicsPipelinePassthrough->GetDescriptorSetLayout(0));
    descriptorSetPassthrough->AddSampler(framebuffer->GetColorAttachment(), 0);
  }

  void Application::InitializeGraphicsPipeline()
  {
    PipelineCreator creator{framebuffer->GetRenderPass(), "res/shaders/shader.vert", "res/shaders/shader.frag"};
    creator.AddDescriptorSetLayoutBinding(0, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);
    creator.AddDescriptorSetLayoutBinding(0, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
    creator.SetVertexDescriptor(Vertex::GetDescriptor());
    creator.SetCullMode(VK_CULL_MODE_NONE);
    graphicsPipeline = std::make_unique<Pipeline>(*vulkan, creator);

    PipelineCreator creatorPassthrough{vulkan->GetSwapChain().GetRenderPass(), "res/shaders/passthrough.vert", "res/shaders/passthrough.frag"};
    creatorPassthrough.AddDescriptorSetLayoutBinding(0, 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
    creatorPassthrough.SetVertexDescriptor(VertexPassthrough::GetDescriptor());
    creatorPassthrough.SetCullMode(VK_CULL_MODE_NONE);
    graphicsPipelinePassthrough = std::make_unique<Pipeline>(*vulkan, creatorPassthrough);
  }

  void Application::InitializeMesh()
  {
    mesh = std::make_unique<Mesh>(*vulkan, vertices, indices);
    meshPassthrough = std::make_unique<Mesh>(*vulkan, verticesPassthrough, indicesPassthrough);
  }

  void Application::InitializeCommandBuffer()
  {
    commandBuffer = std::make_unique<CommandBuffer>(*vulkan, CommandBuffer::Type::Dynamic);
  }

  void Application::InitializeRenderer()
  {
    renderer = std::make_unique<Renderer>(*vulkan, framebuffer->GetRenderPass(), *descriptorPool);
  }

  void Application::RecordCommandBuffer()
  {
    commandBuffer->Begin();

    framebuffer->Bind(*commandBuffer);
    graphicsPipeline->Bind(*commandBuffer);

    UpdateUniformBuffer();

    graphicsPipeline->SetDescriptorSet(0, *descriptorSet);
    graphicsPipeline->BindDescriptorSets(*commandBuffer);

    mesh->Bind(*commandBuffer);
    mesh->Render(*commandBuffer);

    renderer->Begin(*commandBuffer);
    for (int y = 0; y < 10; y++)
    {
      for (int x = 0; x < 10; x++)
      {
        renderer->Quad(glm::vec2(-1 + x * 0.2, -1 + y * 0.2), glm::vec2(-1 + (x + 0.5) * 0.2, -1 + (y + 0.5) * 0.2), glm::vec3{x * 0.1, y * 0.1, 1.0});
      }
    }
    renderer->Quad(glm::vec2(-0.5, -0.5), glm::vec2{-0.1, 0.5}, *texture2D);
    renderer->Quad(glm::vec2(0.1, -0.5), glm::vec2{0.5, 0.5}, *texture2D2);
    renderer->End();

    framebuffer->Unbind(*commandBuffer);

    vulkan->GetSwapChain().BeginFrameBuffer(*commandBuffer);

    graphicsPipelinePassthrough->Bind(*commandBuffer);
    graphicsPipelinePassthrough->SetDescriptorSet(0, *descriptorSetPassthrough);
    graphicsPipelinePassthrough->BindDescriptorSets(*commandBuffer);

    meshPassthrough->Bind(*commandBuffer);
    meshPassthrough->Render(*commandBuffer);

    vulkan->GetSwapChain().EndFrameBuffer(*commandBuffer);
    commandBuffer->End();
  }

  void Application::UpdateUniformBuffer()
  {
    static Timer startTimer;

    float time = startTimer.Elapsed();
    ShaderUniform shaderUniform;
    shaderUniform.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    shaderUniform.projection = glm::perspective(glm::radians(45.0f), framebuffer->GetWidth() / (float)framebuffer->GetHeight(), 0.1f, 10.0f);
    shaderUniform.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    shaderUniform.projection[1][1] *= -1;
    shaderUniform.lightPos = glm::rotate(glm::mat4{1.0f}, time * glm::radians(45.0f), glm::vec3(0, 1, 0)) * glm::vec4{0.3, 0.1, 0, 1};

    shaderUniformBuffer->Update(shaderUniform);
  }
}