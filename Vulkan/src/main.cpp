#include "FileSystem.h"
#include "Buffer.h"
#include "Instance.h"
#define GLM_FORCE_LEFT_HANDED
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <iostream>
#include <vector>
#include <optional>
#include <set>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

struct Vertex {
	glm::vec2 pos;
	glm::vec3 color;
  
  static VkVertexInputBindingDescription getBindingDescription()
  {
    VkVertexInputBindingDescription description{};

    description.binding = 0;
    description.stride = sizeof(Vertex);
    description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return description;
  }

  static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions()
  {
    std::vector<VkVertexInputAttributeDescription> descriptions{2};

    descriptions[0].binding = 0;
    descriptions[0].location = 0;
    descriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    descriptions[0].offset = offsetof(Vertex, pos);

    descriptions[1].binding = 0;
    descriptions[1].location = 1;
    descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    descriptions[1].offset = offsetof(Vertex, color);

    return descriptions;
  }
};

const std::vector<Vertex> vertices = {
	Vertex{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
	Vertex{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
	Vertex{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
	Vertex{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
};

const std::vector<uint16_t> indices = {
    0, 1, 2, 2, 3, 0
};

struct UniformBufferObject
{
  glm::mat4 model;
  glm::mat4 view;
  glm::mat4 projection;
};

class Application final
{
private:
  std::unique_ptr<Instance> instance;
  VkDescriptorSetLayout uniformBufferLayout;
	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;
  VkPipelineLayout pipelineLayout;
  VkPipeline graphicsPipeline;
  std::unique_ptr<Buffer> vertexBuffer;
  std::unique_ptr<Buffer> indexBuffer;
  std::unique_ptr<Buffer> uniformBuffer;
  std::vector<VkCommandBuffer> commandBuffers;
  UniformBufferObject uniformBufferObject;

public:
  Application()
  {
    instance = std::make_unique<Instance>("Vulkan Tutorial");
    InitializeUniformBufferLayout();
    InitializeGraphicsPipeline();
    InitializeVertexBuffer();
    InitializeIndexBuffer();
    InitializeUniformBuffer();
    InitializeDescriptorPool();
    InitializeDescriptorSet();
    InitializeCommandBuffer();
  }

  ~Application()
  {
    vkDeviceWaitIdle(instance->GetDevice());
    uniformBuffer.reset();
    vertexBuffer.reset();
    indexBuffer.reset();
    vkDestroyPipeline(instance->GetDevice(), graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(instance->GetDevice(), pipelineLayout, nullptr);
    vkDestroyDescriptorPool(instance->GetDevice(), descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(instance->GetDevice(), uniformBufferLayout, nullptr);
    instance.reset();
  }

  Application(Application&&) = delete;
  Application(const Application&) = delete;
  Application& operator=(Application&&) = delete;
  Application& operator=(const Application&) = delete;

  bool Update()
  {
    if (!instance->BeginPresent())
      return true;

    RecordCommandBuffer(commandBuffers[instance->GetSwapChain().GetFlightIndex()]);

    instance->SubmitGraphicsQueue(std::vector<VkCommandBuffer>{commandBuffers[instance->GetSwapChain().GetFlightIndex()]});
    return instance->EndPresent();
  }

private:

  void InitializeUniformBufferLayout()
  {
    VkDescriptorSetLayoutBinding layoutBinding{};
    layoutBinding.binding = 0;
    layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layoutBinding.descriptorCount = 1;
    layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    layoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    createInfo.bindingCount = 1;
    createInfo.pBindings = &layoutBinding;

    VK_ASSERT(vkCreateDescriptorSetLayout(instance->GetDevice(), &createInfo, nullptr, &uniformBufferLayout), "Failed to initialize uniform buffer layout");
  }

  void InitializeDescriptorPool()
  {
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = instance->GetMaxFramesInFlight();

    VkDescriptorPoolCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    createInfo.poolSizeCount = 1;
    createInfo.pPoolSizes = &poolSize;
    createInfo.maxSets = instance->GetMaxFramesInFlight();

    VK_ASSERT(vkCreateDescriptorPool(instance->GetDevice(), &createInfo, nullptr, &descriptorPool), "Failed to initialize descriptor pool");
  }

  void InitializeDescriptorSet()
  {
    std::vector<VkDescriptorSetLayout> layouts{static_cast<size_t>(instance->GetMaxFramesInFlight()), uniformBufferLayout};
    VkDescriptorSetAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo.descriptorPool = descriptorPool;
    allocateInfo.descriptorSetCount = instance->GetMaxFramesInFlight();
    allocateInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(instance->GetMaxFramesInFlight());
    VK_ASSERT(vkAllocateDescriptorSets(instance->GetDevice(), &allocateInfo, descriptorSets.data()), "Failed to allocate descriptor sets");
		for (size_t i = 0; i < instance->GetMaxFramesInFlight(); ++i) {
				VkDescriptorBufferInfo bufferInfo = uniformBuffer->GetDescriptorBufferInfo(i);

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrite.dstSet = descriptorSets[i];
				descriptorWrite.dstBinding = 0;
				descriptorWrite.dstArrayElement = 0;
				descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				descriptorWrite.descriptorCount = 1;
				descriptorWrite.pBufferInfo = &bufferInfo;
				descriptorWrite.pImageInfo = nullptr;
				descriptorWrite.pTexelBufferView = nullptr;
				vkUpdateDescriptorSets(instance->GetDevice(), 1, &descriptorWrite, 0, nullptr);
		}

  }

  void InitializeGraphicsPipeline() 
  {
    std::vector<char> vertShaderCode = FileSystem::ReadFile("res/shaders/vert.spv");
    std::vector<char> fragShaderCode = FileSystem::ReadFile("res/shaders/frag.spv");

    VkShaderModule vertShaderModule = InitializeShaderModule(vertShaderCode);
    VkShaderModule fragShaderModule = InitializeShaderModule(fragShaderCode);

    VkPipelineShaderStageCreateInfo shaderStages[2];
    shaderStages[0] = {};
    shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStages[0].module = vertShaderModule;
    shaderStages[0].pName = "main";

    shaderStages[1] = {};
    shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStages[1].module = fragShaderModule;
    shaderStages[1].pName = "main";

    VkVertexInputBindingDescription bindingDescriptions{Vertex::getBindingDescription()};
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions{Vertex::getAttributeDescriptions()};

    VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo{};
    vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputCreateInfo.vertexBindingDescriptionCount = 1;
    vertexInputCreateInfo.pVertexBindingDescriptions = &bindingDescriptions;
    vertexInputCreateInfo.vertexAttributeDescriptionCount = attributeDescriptions.size();
    vertexInputCreateInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo{};
    inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{};
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = instance->GetSwapChain().GetExtent().width;
    viewport.height = instance->GetSwapChain().GetExtent().height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = instance->GetSwapChain().GetExtent();

    std::vector<VkDynamicState> dynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
		dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStateCreateInfo.dynamicStateCount = dynamicStates.size();
		dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();

    VkPipelineViewportStateCreateInfo viewportStateCreateInfo{};
    viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateCreateInfo.viewportCount = 1;
    viewportStateCreateInfo.pViewports = &viewport;
    viewportStateCreateInfo.scissorCount = 1;
    viewportStateCreateInfo.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizerCreateInfo{};
    rasterizerCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizerCreateInfo.depthClampEnable = VK_FALSE;
    rasterizerCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizerCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizerCreateInfo.lineWidth = 1.0f;
    rasterizerCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizerCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;

    rasterizerCreateInfo.depthBiasEnable = VK_FALSE;
    rasterizerCreateInfo.depthBiasConstantFactor = 0.0f;
    rasterizerCreateInfo.depthBiasClamp = 0.0f;
    rasterizerCreateInfo.depthBiasSlopeFactor = 0.0f;

    VkPipelineMultisampleStateCreateInfo multisampleCreateInfo{};
    multisampleCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleCreateInfo.sampleShadingEnable = VK_FALSE;
    multisampleCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampleCreateInfo.minSampleShading = 1.0f;
    multisampleCreateInfo.pSampleMask = nullptr;
    multisampleCreateInfo.alphaToCoverageEnable = VK_FALSE;
    multisampleCreateInfo.alphaToOneEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | 
                                          VK_COLOR_COMPONENT_G_BIT | 
                                          VK_COLOR_COMPONENT_B_BIT | 
                                          VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo{};
    colorBlendCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendCreateInfo.logicOpEnable = VK_FALSE;
    colorBlendCreateInfo.logicOp = VK_LOGIC_OP_COPY;
    colorBlendCreateInfo.attachmentCount = 1;
    colorBlendCreateInfo.pAttachments = &colorBlendAttachment;
    colorBlendCreateInfo.blendConstants[0] = 0.0f;
    colorBlendCreateInfo.blendConstants[1] = 0.0f;
    colorBlendCreateInfo.blendConstants[2] = 0.0f;
    colorBlendCreateInfo.blendConstants[3] = 0.0f;

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.setLayoutCount = 1;
    pipelineLayoutCreateInfo.pSetLayouts = &uniformBufferLayout;
    pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
    pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

    VK_ASSERT(vkCreatePipelineLayout(instance->GetDevice(), &pipelineLayoutCreateInfo, nullptr, &pipelineLayout), "Failed to initialize pipeline layout");

    VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};
    graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphicsPipelineCreateInfo.stageCount = 2;
    graphicsPipelineCreateInfo.pStages = shaderStages;
    graphicsPipelineCreateInfo.pVertexInputState = &vertexInputCreateInfo;
		graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
    graphicsPipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
    graphicsPipelineCreateInfo.pRasterizationState = &rasterizerCreateInfo;
    graphicsPipelineCreateInfo.pMultisampleState = &multisampleCreateInfo;
    graphicsPipelineCreateInfo.pDepthStencilState = nullptr;
    graphicsPipelineCreateInfo.pColorBlendState = &colorBlendCreateInfo;
    graphicsPipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
    graphicsPipelineCreateInfo.layout = pipelineLayout;
    graphicsPipelineCreateInfo.renderPass = instance->GetSwapChain().GetRenderPass();
    graphicsPipelineCreateInfo.subpass = 0;
    graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    graphicsPipelineCreateInfo.basePipelineIndex =  -1;

    VK_ASSERT(vkCreateGraphicsPipelines(instance->GetDevice(), VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &graphicsPipeline), "Failed to initialize graphics pipeline");

    vkDestroyShaderModule(instance->GetDevice(), vertShaderModule, nullptr);
    vkDestroyShaderModule(instance->GetDevice(), fragShaderModule, nullptr);
  }

  void InitializeVertexBuffer()
  {
		VkDeviceSize bufferSize = sizeof(Vertex) * vertices.size();
    Buffer stagingBuffer{*instance, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, bufferSize, 1};

    stagingBuffer.Update((void*)vertices.data(), 0);

    vertexBuffer = std::make_unique<Buffer>(*instance, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, bufferSize, 1);
    Buffer::CopyBuffer(*instance, stagingBuffer, *vertexBuffer);
	}

  void InitializeIndexBuffer()
  {
		VkDeviceSize bufferSize = sizeof(uint16_t) * indices.size();
    Buffer stagingBuffer{*instance, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, bufferSize, 1};

    stagingBuffer.Update((void*)indices.data(), 0);

    indexBuffer = std::make_unique<Buffer>(*instance, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, bufferSize, 1);
    Buffer::CopyBuffer(*instance, stagingBuffer, *indexBuffer);
	}

  void InitializeUniformBuffer() 
  {
    VkDeviceSize bufferSize = sizeof(UniformBufferObject) * instance->GetMaxFramesInFlight();
    uniformBuffer = std::make_unique<Buffer>(*instance, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, bufferSize, instance->GetMaxFramesInFlight());
    uniformBuffer->Map();
	}
  
  void InitializeCommandBuffer()
  {
    commandBuffers.resize(instance->GetMaxFramesInFlight());
		VkCommandBufferAllocateInfo allocateInfo{};
		allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocateInfo.commandPool = instance->GetCommandPool();
		allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocateInfo.commandBufferCount = commandBuffers.size();

		VK_ASSERT(vkAllocateCommandBuffers(instance->GetDevice(), &allocateInfo, commandBuffers.data()), "Failed to initialize command buffer");
  }

  void RecordCommandBuffer(VkCommandBuffer commandBuffer)
  {
    vkResetCommandBuffer(commandBuffer, 0);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    VK_ASSERT(vkBeginCommandBuffer(commandBuffer, &beginInfo), "Failed to begin command buffer");

    VkClearValue clearValue = {{{0.0f, 0.0f, 0.0f, 1.0f}}};

    VkRenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = instance->GetSwapChain().GetRenderPass();
    renderPassBeginInfo.framebuffer = instance->GetSwapChain().GetFramebuffer();
    renderPassBeginInfo.renderArea.offset = {0, 0};
    renderPassBeginInfo.renderArea.extent = instance->GetSwapChain().GetExtent();
    renderPassBeginInfo.clearValueCount = 1;
    renderPassBeginInfo.pClearValues = &clearValue;

    vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

    UpdateUniformBuffer();

    VkBuffer vbo = vertexBuffer->GetHandle();
    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vbo, &offset);
    vkCmdBindIndexBuffer(commandBuffer, indexBuffer->GetHandle(), 0, VK_INDEX_TYPE_UINT16);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = instance->GetSwapChain().GetExtent().width;
    viewport.height = instance->GetSwapChain().GetExtent().height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = instance->GetSwapChain().GetExtent();
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[instance->GetSwapChain().GetFlightIndex()], 0, nullptr);
    vkCmdDrawIndexed(commandBuffer, indices.size(), 1, 0, 0, 0);

    vkCmdEndRenderPass(commandBuffer);
    VK_ASSERT(vkEndCommandBuffer(commandBuffer), "Failed to end command buffer");
  }

  void UpdateUniformBuffer()
  {
		static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
    uniformBufferObject.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		uniformBufferObject.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		uniformBufferObject.projection = glm::perspective(glm::radians(45.0f), instance->GetSwapChain().GetExtent().width / (float) instance->GetSwapChain().GetExtent().height, 0.1f, 10.0f);
    uniformBufferObject.projection[1][1] *= -1;

    uniformBuffer->Update(&uniformBufferObject, instance->GetSwapChain().GetFlightIndex());
  }

  VkShaderModule InitializeShaderModule(const std::vector<char>& code)
  {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    VK_ASSERT(vkCreateShaderModule(instance->GetDevice(), &createInfo, nullptr, &shaderModule), "Failed to initialize shader module");

    return shaderModule;
  }
};

int main()
{
  glfwInit();

  {
    Application application;
    while (application.Update())
    {
      glfwPollEvents();
    }
  }

  glfwTerminate();
  return 0;
}