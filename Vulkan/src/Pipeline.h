#pragma once

#include "Common.h"
#include "Instance.h"
#include "FileSystem.h"
#include "PipelineCreator.h"

#include <vulkan/vulkan.hpp>
#include <map>


class Pipeline
{
  CP_DELETE_COPY_AND_MOVE_CTOR(Pipeline);
private:
  Instance& instance;

  std::map<uint32_t, VkDescriptorSetLayout> vertexDescriptorSetLayouts;
  std::map<uint32_t, VkDescriptorSetLayout> fragmentDescriptorSetLayouts;
  VkPipelineLayout pipelineLayout;
  VkPipeline graphicsPipeline;

public:
  Pipeline(Instance& instance, PipelineCreator creator)
    : instance{instance}
  {
    InitializeDescriptorSetLayouts(creator);
    InitializePipeline(creator);
  }

  ~Pipeline()
  {
    vkDestroyPipeline(instance.GetDevice(), graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(instance.GetDevice(), pipelineLayout, nullptr);
    for (auto&& descriptorSetLayout : vertexDescriptorSetLayouts)
    {
      vkDestroyDescriptorSetLayout(instance.GetDevice(), descriptorSetLayout.second, nullptr);
    }
    for (auto&& descriptorSetLayout : fragmentDescriptorSetLayouts)
    {
      vkDestroyDescriptorSetLayout(instance.GetDevice(), descriptorSetLayout.second, nullptr);
    }
  }

  void Bind(VkCommandBuffer commandBuffer)
  {
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = instance.GetSwapChain().GetExtent().width;
    viewport.height = instance.GetSwapChain().GetExtent().height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = instance.GetSwapChain().GetExtent();
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
  }

  VkPipelineLayout GetPipelineLayout() const
  {
    return pipelineLayout;
  }

  VkDescriptorSetLayout GetVertexDescriptorSetLayout(uint32_t binding)
  {
    return vertexDescriptorSetLayouts.at(binding);
  }

  VkDescriptorSetLayout GetFragmentDescriptorSetLayout(uint32_t binding)
  {
    return fragmentDescriptorSetLayouts.at(binding);
  }

private:
  void InitializeDescriptorSetLayouts(const PipelineCreator& creator)
  {
    {
      int i = 0;
      for (auto& binding : creator.vertexDescriptorSetLayouts)
      {
        vertexDescriptorSetLayouts.emplace(binding, InitializeDescriptorSetLayout(binding, VK_SHADER_STAGE_VERTEX_BIT));
        i++;
      }
    }
    {
      int i = 0;
      for (auto& binding : creator.fragmentDescriptorSetLayouts)
      {
        fragmentDescriptorSetLayouts.emplace(binding, InitializeDescriptorSetLayout(binding, VK_SHADER_STAGE_FRAGMENT_BIT));
        i++;
      }
    }
  }

  void InitializePipeline(const PipelineCreator& creator)
  {
    std::vector<char> vertShaderCode = FileSystem::ReadFile(creator.vertexShader);
    std::vector<char> fragShaderCode = FileSystem::ReadFile(creator.fragmentShader);

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

    VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo{};
    vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputCreateInfo.vertexBindingDescriptionCount = 1;
    vertexInputCreateInfo.pVertexBindingDescriptions = &creator.vertexInputBindingDescription;
    vertexInputCreateInfo.vertexAttributeDescriptionCount = creator.vertexInputAttributeDescriptions.size();
    vertexInputCreateInfo.pVertexAttributeDescriptions = creator.vertexInputAttributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo{};
    inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyCreateInfo.topology = creator.topology;
    inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{};
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = instance.GetSwapChain().GetExtent().width;
    viewport.height = instance.GetSwapChain().GetExtent().height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = instance.GetSwapChain().GetExtent();

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
    rasterizerCreateInfo.cullMode = creator.cullMode;
    rasterizerCreateInfo.frontFace = creator.frontFace;

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

    VkPipelineColorBlendAttachmentState colorBlendAttachment{}; // TODO: Add to PipelineCreator
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

    std::vector<VkDescriptorSetLayout> layouts{vertexDescriptorSetLayouts.size() + fragmentDescriptorSetLayouts.size()};
    int i = 0;
    for (auto&& descriptorSetLayout : vertexDescriptorSetLayouts)
    {
      layouts[i++] = descriptorSetLayout.second;
    }
    for (auto&& descriptorSetLayout : fragmentDescriptorSetLayouts)
    {
      layouts[i++] = descriptorSetLayout.second;
    }
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.setLayoutCount = layouts.size();
    pipelineLayoutCreateInfo.pSetLayouts = layouts.data();
    pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
    pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

    CP_VK_ASSERT(vkCreatePipelineLayout(instance.GetDevice(), &pipelineLayoutCreateInfo, nullptr, &pipelineLayout), "Failed to initialize pipeline layout");

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
    graphicsPipelineCreateInfo.renderPass = instance.GetSwapChain().GetRenderPass();
    graphicsPipelineCreateInfo.subpass = 0;
    graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    graphicsPipelineCreateInfo.basePipelineIndex =  -1;

    CP_VK_ASSERT(vkCreateGraphicsPipelines(instance.GetDevice(), VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &graphicsPipeline), "Failed to initialize graphics pipeline");

    vkDestroyShaderModule(instance.GetDevice(), vertShaderModule, nullptr);
    vkDestroyShaderModule(instance.GetDevice(), fragShaderModule, nullptr);
  }

  VkShaderModule InitializeShaderModule(const std::vector<char>& code)
  {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    CP_VK_ASSERT(vkCreateShaderModule(instance.GetDevice(), &createInfo, nullptr, &shaderModule), "Failed to initialize shader module");

    return shaderModule;
  }

  VkDescriptorSetLayout InitializeDescriptorSetLayout(uint32_t binding, VkShaderStageFlags flags)
  {
    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorSetLayoutBinding layoutBinding{};
    layoutBinding.binding = binding;
    layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layoutBinding.descriptorCount = 1;
    layoutBinding.stageFlags = flags;
    layoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    createInfo.bindingCount = 1;
    createInfo.pBindings = &layoutBinding;

    CP_VK_ASSERT(vkCreateDescriptorSetLayout(instance.GetDevice(), &createInfo, nullptr, &descriptorSetLayout), "Failed to initialize descriptor set layout");

    return descriptorSetLayout;
  }
};