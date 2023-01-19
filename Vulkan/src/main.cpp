#include "FileSystem.h"
#include "Buffer.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "Instance.h"
#include "Timer.h"
#include "UniformBuffer.h"
#include "Sampler.h"
#include "Vertex.h"
#include "Pipeline.h"
#include "DescriptorPool.h"
#include "DescriptorSet.h"
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <optional>
#include <set>
#include <glm/glm.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

const std::vector<Vertex> vertices = {
	Vertex{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
	Vertex{{ 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
	Vertex{{ 0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
  Vertex{{-0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
};

const std::vector<uint16_t> indices = {
    0, 1, 2, 2, 3, 0
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
  std::unique_ptr<VertexBuffer> vertexBuffer;
  std::unique_ptr<IndexBuffer> indexBuffer;
  std::unique_ptr<UniformBuffer> shaderUniformBuffer;
  std::unique_ptr<Sampler> sampler;
  std::unique_ptr<DescriptorPool> descriptorPool;
  std::unique_ptr<DescriptorSet> uniformDescriptorSet;
  std::unique_ptr<DescriptorSet> samplerDescriptorSet;
  std::vector<VkCommandBuffer> commandBuffers;

  VkImage textureImage;
  VkDeviceMemory textureImageMemory;
  VkImageView textureImageView;

public:
  Application()
  {
    InitializeInstance();
    InitializeGraphicsPipeline();
    InitializeTextureImage();
    InitializeTextureImageView();
    InitializeTextureSampler();
    InitializeUniformBuffer();
    InitializeDescriptorSets();
    InitializeVertexBuffer();
    InitializeIndexBuffer();
    InitializeCommandBuffers();
  }

  ~Application()
  {
    vkDeviceWaitIdle(instance->GetDevice());
    vkDestroyImage(instance->GetDevice(), textureImage, nullptr);
    vkFreeMemory(instance->GetDevice(), textureImageMemory, nullptr);
    vkDestroyImageView(instance->GetDevice(), textureImageView, nullptr);
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

  void InitializeTextureImage()
  {
    int texWidth;
    int texHeight;
    int texChannels;
    stbi_uc* pixels = stbi_load("res/textures/texture.png", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

    CP_ASSERT(pixels, "Failed to load texture image");

    VkDeviceSize bufferSize = texWidth * texHeight * 4;
    Buffer stagingBuffer{*instance, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, bufferSize, 1};
    void* data = stagingBuffer.Map();
    memcpy(data, pixels, bufferSize);
    stagingBuffer.Unmap();
    stbi_image_free(pixels);

    VkImageCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    createInfo.imageType = VK_IMAGE_TYPE_2D;
    createInfo.extent.width = texWidth;
    createInfo.extent.height = texHeight;
    createInfo.extent.depth = 1;
    createInfo.mipLevels = 1;
    createInfo.arrayLayers = 1;
    createInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    createInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    createInfo.flags = 0;

    CP_VK_ASSERT(vkCreateImage(instance->GetDevice(), &createInfo, nullptr, &textureImage), "Failed to initialize texture image");

    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(instance->GetDevice(), textureImage, &memoryRequirements);

    VkMemoryAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize = memoryRequirements.size;
    allocateInfo.memoryTypeIndex = instance->FindMemoryType(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    CP_VK_ASSERT(vkAllocateMemory(instance->GetDevice(), &allocateInfo, nullptr, &textureImageMemory), "Failed to initiallizse texture image memory");

    vkBindImageMemory(instance->GetDevice(), textureImage, textureImageMemory, 0);

    TransitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    CopyBufferToImage(stagingBuffer, textureImage, texWidth, texHeight);
    TransitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
  }

  void InitializeTextureImageView()
  {
    textureImageView = CreateImageView();
  }

  void InitializeTextureSampler()
  {
    sampler = std::make_unique<Sampler>(*instance, textureImageView);
  }

  VkImageView CreateImageView()
  {
    VkImageView imageView;
    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = textureImage;
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;
    CP_VK_ASSERT(vkCreateImageView(instance->GetDevice(), &createInfo, nullptr, &imageView), "Failed to initialize ImageView");

    return imageView;
  }

  void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
  {
    VkCommandBuffer commandBuffer = BeginSingleUseCommandBuffer();

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = 0;

    VkPipelineStageFlags srcStage;
    VkPipelineStageFlags dstStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
        throw std::invalid_argument("Unsupported layout transition");
    }

    vkCmdPipelineBarrier(commandBuffer, srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    EndSinglelUseCommandBuffer(commandBuffer);
  }

  void CopyBufferToImage(const Buffer& buffer, VkImage image, uint32_t width, uint32_t height)
  {

    VkCommandBuffer commandBuffer = BeginSingleUseCommandBuffer();

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = {0, 0, 0};
    region.imageExtent = {width, height, 1};

    vkCmdCopyBufferToImage(commandBuffer, buffer.GetHandle(), image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    EndSinglelUseCommandBuffer(commandBuffer);
  }

  VkCommandBuffer BeginSingleUseCommandBuffer()
  {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = instance->GetCommandPool();
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(instance->GetDevice(), &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);
    return commandBuffer;
  }

  void EndSinglelUseCommandBuffer(VkCommandBuffer commandBuffer)
  {
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(instance->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(instance->GetGraphicsQueue());

    vkFreeCommandBuffers(instance->GetDevice(), instance->GetCommandPool(), 1, &commandBuffer);

  }

  void InitializeUniformBuffer()
  {
    shaderUniformBuffer = std::make_unique<UniformBuffer>(*instance, sizeof(ShaderUniform));
  }

  void InitializeDescriptorSets()
  {
    descriptorPool = std::make_unique<DescriptorPool>(*instance);

    uniformDescriptorSet = std::make_unique<DescriptorSet>(*instance, *descriptorPool, graphicsPipeline->GetDescriptorSetLayout(0));
    uniformDescriptorSet->AddUniform(*shaderUniformBuffer, 0);

    samplerDescriptorSet = std::make_unique<DescriptorSet>(*instance, *descriptorPool, graphicsPipeline->GetDescriptorSetLayout(1));
    samplerDescriptorSet->AddSampler(*sampler, 0);
  }

  void InitializeGraphicsPipeline() 
  {
    PipelineCreator creator{"res/shaders/vert.spv", "res/shaders/frag.spv"};
    creator.AddDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
    creator.AddDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
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
    shaderUniformBuffer->Bind(commandBuffer);

    std::vector<VkDescriptorSet> sets{uniformDescriptorSet->GetHandle(), samplerDescriptorSet->GetHandle()};
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->GetPipelineLayout(), 0, sets.size(), sets.data(), 0, nullptr);
    // Replace the two lines above with this somehow:
    // graphicsPipeline->BindDescriptorSets(uniformDescriptorSet, samplerDescriptorSet);

    indexBuffer->Draw(commandBuffer);

    vkCmdEndRenderPass(commandBuffer);
    CP_VK_ASSERT(vkEndCommandBuffer(commandBuffer), "Failed to end command buffer");
  }

  void UpdateUniformBuffer()
  {
		static Timer startTimer;

    float time = startTimer.Elapsed();
    ShaderUniform shaderUniform;
		shaderUniform.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		shaderUniform.projection = glm::perspective(glm::radians(45.0f), instance->GetSwapChain().GetExtent().width / (float) instance->GetSwapChain().GetExtent().height, 0.1f, 10.0f);
    shaderUniform.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    shaderUniform.projection[1][1] *= -1;

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