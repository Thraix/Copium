#include "FileSystem.h"
#include "Buffer.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "Instance.h"
#include "Timer.h"
#include "UniformBuffer.h"
#include "Vertex.h"
#include "Pipeline.h"
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <optional>
#include <set>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

const std::vector<Vertex> vertices = {
	Vertex{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
	Vertex{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
	Vertex{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
	Vertex{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
};

const std::vector<uint16_t> indices = {
    0, 1, 2, 2, 3, 0
};

struct ShaderUniform
{
  glm::mat4 model;
  glm::mat4 view;
  glm::mat4 projection;
};

class Application final
{
private:
  std::unique_ptr<Instance> instance;
  std::unique_ptr<Pipeline> graphicsPipeline;
  std::unique_ptr<VertexBuffer> vertexBuffer;
  std::unique_ptr<IndexBuffer> indexBuffer;
  std::unique_ptr<UniformBuffer<ShaderUniform>> uniformBuffer;
  std::vector<VkCommandBuffer> commandBuffers;

public:
  Application()
  {
    InitializeInstance();
    InitializeGraphicsPipeline();
    InitializeUniformBuffer();
    InitializeVertexBuffer();
    InitializeIndexBuffer();
    InitializeCommandBuffers();
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

    RecordCommandBuffer(commandBuffers[instance->GetFlightIndex()]);

    instance->SubmitGraphicsQueue(std::vector<VkCommandBuffer>{commandBuffers[instance->GetFlightIndex()]});
    return instance->EndPresent();
  }

private:

  void InitializeInstance()
  {
    instance = std::make_unique<Instance>("Vulkan Tutorial");
  }

  void InitializeUniformBuffer()
  {
    uniformBuffer = std::make_unique<UniformBuffer<ShaderUniform>>(*instance, *graphicsPipeline, 0, graphicsPipeline->GetVertexDescriptorSetLayout(0));
  }

  void InitializeGraphicsPipeline() 
  {
    PipelineCreator creator{"res/shaders/vert.spv", "res/shaders/frag.spv"};
    creator.AddVertexDescriptorSetLayoutBinding(0);
    creator.SetVertexInputBindingDescription(Vertex::GetBindingDescription());
    creator.SetVertexInputAttributeDescription(Vertex::GetAttributeDescriptions());
    graphicsPipeline = std::make_unique<Pipeline>(*instance, creator);
  }

  void InitializeVertexBuffer()
  {
		VkDeviceSize bufferSize = sizeof(Vertex) * vertices.size();
    vertexBuffer = std::make_unique<VertexBuffer>(*instance, bufferSize);
    vertexBuffer->UpdateStaging((void*)vertices.data());
	}

  void InitializeIndexBuffer()
  {
		VkDeviceSize bufferSize = sizeof(uint16_t) * indices.size();
    indexBuffer = std::make_unique<IndexBuffer>(*instance, indices.size());
    indexBuffer->UpdateStaging((void*)indices.data());
	}
  
  void InitializeCommandBuffers()
  {
    commandBuffers.resize(instance->GetMaxFramesInFlight());
		VkCommandBufferAllocateInfo allocateInfo{};
		allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocateInfo.commandPool = instance->GetCommandPool();
		allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocateInfo.commandBufferCount = commandBuffers.size();

		CP_VK_ASSERT(vkAllocateCommandBuffers(instance->GetDevice(), &allocateInfo, commandBuffers.data()), "Failed to initialize command buffer");
  }

  void RecordCommandBuffer(VkCommandBuffer commandBuffer)
  {
    vkResetCommandBuffer(commandBuffer, 0);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    CP_VK_ASSERT(vkBeginCommandBuffer(commandBuffer, &beginInfo), "Failed to begin command buffer");

    VkClearValue clearValue = {{{0.0f, 0.0f, 0.0f, 1.0f}}};

    // TODO: framebuffer->Bind();
    VkRenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = instance->GetSwapChain().GetRenderPass();
    renderPassBeginInfo.framebuffer = instance->GetSwapChain().GetFramebuffer();
    renderPassBeginInfo.renderArea.offset = {0, 0};
    renderPassBeginInfo.renderArea.extent = instance->GetSwapChain().GetExtent();
    renderPassBeginInfo.clearValueCount = 1;
    renderPassBeginInfo.pClearValues = &clearValue;
    vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    graphicsPipeline->Bind(commandBuffer);

    UpdateUniformBuffer();

    vertexBuffer->Bind(commandBuffer);
    indexBuffer->Bind(commandBuffer);
    uniformBuffer->Bind(commandBuffer);

    indexBuffer->Draw(commandBuffer);

    vkCmdEndRenderPass(commandBuffer);
    CP_VK_ASSERT(vkEndCommandBuffer(commandBuffer), "Failed to end command buffer");
  }

  void UpdateUniformBuffer()
  {
		static Timer startTimer;

    float time = startTimer.Elapsed();
    ShaderUniform shaderUniform;
    shaderUniform.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		shaderUniform.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		shaderUniform.projection = glm::perspective(glm::radians(45.0f), instance->GetSwapChain().GetExtent().width / (float) instance->GetSwapChain().GetExtent().height, 0.1f, 10.0f);
    shaderUniform.projection[1][1] *= -1;

    uniformBuffer->Update(shaderUniform);
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
        std::cout << frames << "fps" << std::endl;
        frames = 0;
        timer.Start();
      }
      frames++;
    }
  }

  glfwTerminate();
  return 0;
}