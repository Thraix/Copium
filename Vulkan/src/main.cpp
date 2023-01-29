#include "Buffer.h"
#include "DescriptorPool.h"
#include "DescriptorSet.h"
#include "IndexBuffer.h"
#include "Instance.h"
#include "Pipeline.h"
#include "Texture2D.h"
#include "Timer.h"
#include "UniformBuffer.h"
#include "Vertex.h"
#include "VertexBuffer.h"

#include <GLFW/glfw3.h>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <optional>
#include <set>
#include <vector>

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

struct alignas(64) ShaderUniform
{
  alignas(16) glm::mat4 projection;
  alignas(16) glm::mat4 view;
  alignas(16) glm::mat4 model;
  alignas(16) glm::vec3 lightPos;
};

class Application final
{
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

public:
  Application()
  {
    InitializeInstance();
    InitializeGraphicsPipeline();
    InitializeTextureSampler();
    InitializeUniformBuffer();
    InitializeDescriptorSets();
    InitializeVertexBuffer();
    InitializeIndexBuffer();
    InitializeCommandBuffer();
  }

  ~Application()
  {
    vkDeviceWaitIdle(instance->GetDevice());
  }

  Application(Application&&) = delete;
  Application(const Application&) = delete;
  Application& operator=(Application&&) = delete;
  Application& operator=(const Application&) = delete;

  bool Update()
  {
    if (!instance->BeginPresent())
      return true;

    RecordCommandBuffer();
    commandBuffer->SubmitAsGraphicsQueue();

    return instance->EndPresent();
  }

private:

  void InitializeInstance()
  {
    instance = std::make_unique<Instance>("Vulkan Tutorial");
  }

  void InitializeTextureSampler()
  {
    texture2D = std::make_unique<Texture2D>(*instance, "res/textures/texture.png");
  }

  void InitializeUniformBuffer()
  {
    shaderUniformBuffer = std::make_unique<UniformBuffer>(*instance, sizeof(ShaderUniform));
  }

  void InitializeDescriptorSets()
  {
    descriptorPool = std::make_unique<DescriptorPool>(*instance);

    descriptorSet = std::make_unique<DescriptorSet>(*instance, *descriptorPool, graphicsPipeline->GetDescriptorSetLayout(0));
    descriptorSet->AddUniform(*shaderUniformBuffer, 0);
    descriptorSet->AddTexture2D(*texture2D, 1);
  }

  void InitializeGraphicsPipeline() 
  {
    PipelineCreator creator{"res/shaders/shader.vert", "res/shaders/shader.frag"};
    creator.AddDescriptorSetLayoutBinding(0, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);
    creator.AddDescriptorSetLayoutBinding(0, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
    creator.SetVertexDescriptor(Vertex::GetDescriptor());
    creator.SetCullMode(VK_CULL_MODE_NONE);
    graphicsPipeline = std::make_unique<Pipeline>(*instance, creator);
  }

  void InitializeVertexBuffer()
  {
    vertexBuffer = std::make_unique<VertexBuffer>(*instance, Vertex::GetDescriptor(), vertices.size());
    vertexBuffer->Update(0, (void*)vertices.data());
	}

  void InitializeIndexBuffer()
  {
		VkDeviceSize bufferSize = sizeof(uint16_t) * indices.size();
    indexBuffer = std::make_unique<IndexBuffer>(*instance, indices.size());
    indexBuffer->UpdateStaging((void*)indices.data());
	}
  
  void InitializeCommandBuffer()
  {
    commandBuffer = std::make_unique<CommandBuffer>(*instance, CommandBufferType::Dynamic);
  }

  void RecordCommandBuffer()
  {
    commandBuffer->Begin();
    std::vector<VkClearValue> clearValues{2};
    clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValues[1].depthStencil = {1.0f, 0};

    // TODO: framebuffer->Bind();
    VkRenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = instance->GetSwapChain().GetRenderPass();
    renderPassBeginInfo.framebuffer = instance->GetSwapChain().GetFramebuffer();
    renderPassBeginInfo.renderArea.offset = {0, 0};
    renderPassBeginInfo.renderArea.extent = instance->GetSwapChain().GetExtent();
    renderPassBeginInfo.clearValueCount = clearValues.size();
    renderPassBeginInfo.pClearValues = clearValues.data();
    vkCmdBeginRenderPass(commandBuffer->GetHandle(), &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    graphicsPipeline->Bind(commandBuffer->GetHandle());

    UpdateUniformBuffer();

    vertexBuffer->Bind(commandBuffer->GetHandle());
    indexBuffer->Bind(commandBuffer->GetHandle());

    graphicsPipeline->SetDescriptorSet(0, *descriptorSet);
    graphicsPipeline->BindDescriptorSets(commandBuffer->GetHandle());

    indexBuffer->Draw(commandBuffer->GetHandle());

    vkCmdEndRenderPass(commandBuffer->GetHandle());
    commandBuffer->End();
  }

  void UpdateUniformBuffer()
  {
    static Timer startTimer;

    float time = startTimer.Elapsed();
    ShaderUniform shaderUniform;
    shaderUniform.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    shaderUniform.projection = glm::perspective(glm::radians(45.0f), instance->GetSwapChain().GetExtent().width / (float)instance->GetSwapChain().GetExtent().height, 0.1f, 10.0f);
    shaderUniform.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    shaderUniform.projection[1][1] *= -1;
    shaderUniform.lightPos = glm::rotate(glm::mat4{1.0f}, time * glm::radians(45.0f), glm::vec3(0, 1, 0)) * glm::vec4{0.3, 0.1, 0, 1};

    shaderUniformBuffer->Update(shaderUniform);
  }

  VkShaderModule InitializeShaderModule(const std::vector<char>& code)
  {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    CP_VK_ASSERT(vkCreateShaderModule(instance->GetDevice(), &createInfo, nullptr, &shaderModule), "Failed to initialize shader module");

    return shaderModule;
  }
};

int main()
{
  CP_ASSERT(glfwInit() == GLFW_TRUE, "Failed to initialize the glfw context");
  {
    Application application;
    Timer timer;
    int frames = 0;
    while (application.Update())
    {
      glfwPollEvents();
      if (timer.Elapsed() >= 1.0)
      {
        CP_DEBUG("%d fps", frames);
        frames = 0;
        timer.Start();
      }
      frames++;
    }
  }

  glfwTerminate();
  return 0;
}