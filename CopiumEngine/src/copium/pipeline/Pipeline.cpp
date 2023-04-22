#include "copium/pipeline/Pipeline.h"

#include "copium/asset/AssetManager.h"
#include "copium/buffer/Framebuffer.h"
#include "copium/core/Vulkan.h"
#include "copium/pipeline/Shader.h"
#include "copium/renderer/RendererVertex.h"
#include "copium/mesh/VertexPassthrough.h"
#include "copium/mesh/Vertex.h"
#include "copium/util/FileSystem.h"

namespace Copium
{
  Pipeline::Pipeline(const MetaFile& metaFile)
    : shaderReflector{ShaderReflector{metaFile.GetMetaClass("Pipeline").GetValue("vert-filepath"), metaFile.GetMetaClass("Pipeline").GetValue("frag-filepath")}}
  {
    const MetaFileClass& metaFileClass = metaFile.GetMetaClass("Pipeline");
    VkRenderPass renderPass;
    if (metaFileClass.HasValue("framebuffer-uuid"))
    {
      Framebuffer& fb = AssetManager::LoadAsset<Framebuffer>(UUID{metaFileClass.GetValue("framebuffer-uuid")});
      renderPass = fb.GetRenderPass();
      framebuffer = fb;
    }
    else
    {
      renderPass = Vulkan::GetSwapChain().GetRenderPass();
    }
    PipelineCreator creator{renderPass, metaFileClass.GetValue("vert-filepath"), metaFileClass.GetValue("frag-filepath")};
    std::string type = metaFileClass.GetValue("type");
    if (type == "Renderer")
    {
      creator.SetVertexDescriptor(RendererVertex::GetDescriptor());
      creator.SetDepthTest(false);
    }
    else if (type == "Passthrough")
    {
      creator.SetVertexDescriptor(VertexPassthrough::GetDescriptor());
      creator.SetDepthTest(false);
    }
    else if (type == "Mesh")
    {
      creator.SetVertexDescriptor(Vertex::GetDescriptor());
    }
    InitializeDescriptorSetLayout(creator);
    InitializePipeline(creator);
  }

  Pipeline::Pipeline(PipelineCreator creator)
    : shaderReflector{creator.shaderReflector}
  {
    InitializeDescriptorSetLayout(creator);
    InitializePipeline(creator);
  }

  Pipeline::~Pipeline()
  {
    vkDestroyPipeline(Vulkan::GetDevice(), graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(Vulkan::GetDevice(), pipelineLayout, nullptr);
    for (auto&& descriptorSetLayout : descriptorSetLayouts)
    {
      vkDestroyDescriptorSetLayout(Vulkan::GetDevice(), descriptorSetLayout, nullptr);
    }
    if (framebuffer != NULL_ASSET_HANDLE)
    {
      AssetManager::UnloadAsset(framebuffer);
    }
  }

  void Pipeline::Bind(const CommandBuffer& commandBuffer)
  {
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
  }

  void Pipeline::SetDescriptorSet(const DescriptorSet& descriptorSet)
  {
    CP_ASSERT(descriptorSet.GetSetIndex() < boundDescriptorSets.size(), "DescriptorSet index is out of bounds");
    boundDescriptorSets[descriptorSet.GetSetIndex()] = descriptorSet;
  }

  void Pipeline::BindDescriptorSets(const CommandBuffer& commandBuffer)
  {
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, boundDescriptorSets.size(), boundDescriptorSets.data(), 0, nullptr);
  }

  std::unique_ptr<DescriptorSet> Pipeline::CreateDescriptorSet(DescriptorPool& descriptorPool, int setIndex) const
  {
    std::set<ShaderBinding> bindings;
    for (auto& binding : shaderReflector.bindings)
    {
      if (binding.set != setIndex)
        continue;
      bindings.emplace(binding);
    }

    return std::make_unique<DescriptorSet>(descriptorPool, descriptorSetLayouts[setIndex], bindings);
  }

  void Pipeline::InitializeDescriptorSetLayout(const PipelineCreator& creator)
  {
    boundDescriptorSets.resize(creator.descriptorSetLayouts.size());
    descriptorSetLayouts.resize(creator.descriptorSetLayouts.size());
    int i = 0;
    for (auto&& bindings : creator.descriptorSetLayouts)
    {
      std::vector<VkDescriptorSetLayoutBinding> layoutBindings{bindings.second.size()};
      int j = 0;
      for (auto&& binding : bindings.second)
      {
        layoutBindings[j].binding = binding.binding;
        layoutBindings[j].descriptorType = binding.type;
        layoutBindings[j].descriptorCount = binding.count;
        layoutBindings[j].stageFlags = binding.flags;
        layoutBindings[j].pImmutableSamplers = nullptr;
        j++;
      }

      VkDescriptorSetLayoutCreateInfo createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
      createInfo.bindingCount = layoutBindings.size();
      createInfo.pBindings = layoutBindings.data();

      CP_VK_ASSERT(vkCreateDescriptorSetLayout(Vulkan::GetDevice(), &createInfo, nullptr, &descriptorSetLayouts[i++]), "Failed to initialize descriptor set layout");
    }
  }

  void Pipeline::InitializePipeline(const PipelineCreator& creator)
  {
    Shader shader{Shader::Type::GlslFile, creator.vertexShader, creator.fragmentShader};

    VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo{};
    vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputCreateInfo.vertexBindingDescriptionCount = creator.vertexDescriptor.GetBindings().size();
    vertexInputCreateInfo.pVertexBindingDescriptions = creator.vertexDescriptor.GetBindings().data();
    vertexInputCreateInfo.vertexAttributeDescriptionCount = creator.vertexDescriptor.GetAttributes().size();
    vertexInputCreateInfo.pVertexAttributeDescriptions = creator.vertexDescriptor.GetAttributes().data();

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo{};
    inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyCreateInfo.topology = creator.topology;
    inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{};
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = 1;
    viewport.height = 1;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = {1, 1};

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

    VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo{};
    depthStencilCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilCreateInfo.depthTestEnable = creator.depthTest ? VK_TRUE : VK_FALSE;
    depthStencilCreateInfo.depthWriteEnable = creator.depthTest ? VK_TRUE : VK_FALSE;
    depthStencilCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencilCreateInfo.depthBoundsTestEnable = VK_FALSE;
    depthStencilCreateInfo.minDepthBounds = 0.0f;
    depthStencilCreateInfo.maxDepthBounds = 1.0f;
    depthStencilCreateInfo.stencilTestEnable = VK_FALSE;
    depthStencilCreateInfo.front = {};
    depthStencilCreateInfo.back = {};


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

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.setLayoutCount = descriptorSetLayouts.size();
    pipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayouts.data();
    pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
    pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

    CP_VK_ASSERT(vkCreatePipelineLayout(Vulkan::GetDevice(), &pipelineLayoutCreateInfo, nullptr, &pipelineLayout), "Failed to initialize pipeline layout");

    const std::vector<VkPipelineShaderStageCreateInfo>& shaderStages = shader.GetShaderStages();
    VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};
    graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphicsPipelineCreateInfo.stageCount = shaderStages.size();
    graphicsPipelineCreateInfo.pStages = shaderStages.data();
    graphicsPipelineCreateInfo.pVertexInputState = &vertexInputCreateInfo;
    graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
    graphicsPipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
    graphicsPipelineCreateInfo.pRasterizationState = &rasterizerCreateInfo;
    graphicsPipelineCreateInfo.pMultisampleState = &multisampleCreateInfo;
    graphicsPipelineCreateInfo.pDepthStencilState = &depthStencilCreateInfo;
    graphicsPipelineCreateInfo.pColorBlendState = &colorBlendCreateInfo;
    graphicsPipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
    graphicsPipelineCreateInfo.layout = pipelineLayout;
    graphicsPipelineCreateInfo.renderPass = creator.renderPass;
    graphicsPipelineCreateInfo.subpass = 0;
    graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    graphicsPipelineCreateInfo.basePipelineIndex = -1;

    CP_VK_ASSERT(vkCreateGraphicsPipelines(Vulkan::GetDevice(), VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &graphicsPipeline), "Failed to initialize graphics pipeline");
  }
}