#include "copium/buffer/Framebuffer.h"

#include "copium/asset/AssetManager.h"
#include "copium/buffer/CommandBuffer.h"
#include "copium/core/Vulkan.h"
#include "copium/sampler/Image.h"

namespace Copium
{
  Framebuffer::Framebuffer(const MetaFile& metaFile)
  {
    const MetaFileClass& metaClass = metaFile.GetMetaClass("Framebuffer");
    colorAttachment = AssetRef<ColorAttachment>(Uuid{metaClass.GetValue("rendertexture-uuid")});
    ColorAttachment& attachment = colorAttachment.GetAsset();

    width = attachment.GetWidth();
    height = attachment.GetHeight();
    CP_ASSERT(width > 0, "Width of framebuffer is less than 1: %d", width);
    CP_ASSERT(height > 0, "Height of framebuffer is less than 1: %d", height);

    InitializeDepthBuffer();
    InitializeRenderPass();
    InitializeFramebuffers();
  }

  Framebuffer::Framebuffer(int width, int height, const SamplerCreator& samplerCreator)
    : width{width}, height{height}
  {
    CP_ASSERT(width > 0, "Width of framebuffer is less than 1: %d", width);
    CP_ASSERT(height > 0, "Height of framebuffer is less than 1: %d", height);

    InitializeImage(samplerCreator);
    InitializeDepthBuffer();
    InitializeRenderPass();
    InitializeFramebuffers();
  }

  Framebuffer::~Framebuffer()
  {
    std::vector<VkFramebuffer> framebuffersCpy = framebuffers;
    VkRenderPass renderPassCpy = renderPass;
    Vulkan::GetDevice().QueueIdleCommand([framebuffersCpy, renderPassCpy]() {
      for (auto& framebuffer : framebuffersCpy)
        vkDestroyFramebuffer(Vulkan::GetDevice(), framebuffer, nullptr);
      vkDestroyRenderPass(Vulkan::GetDevice(), renderPassCpy, nullptr);
    });
  }

  void Framebuffer::Resize(int width, int height)
  {
    CP_ASSERT(width > 0, "Width of framebuffer is less than 1: %d", width);
    CP_ASSERT(height > 0, "Height of framebuffer is less than 1: %d", height);

    Vulkan::GetDevice().WaitIdle();
    this->width = width;
    this->height = height;
    for (auto&& framebuffer : framebuffers)
      vkDestroyFramebuffer(Vulkan::GetDevice(), framebuffer, nullptr);
    colorAttachment.GetAsset().Resize(width, height);
    depthAttachment->Resize(width, height);
    InitializeFramebuffers();
  }

  void Framebuffer::Bind(const CommandBuffer& commandBuffer)
  {
    std::vector<VkClearValue> clearValues{2};
    clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValues[1].depthStencil = {1.0f, 0};

    VkRenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = renderPass;
    renderPassBeginInfo.framebuffer = framebuffers[Vulkan::GetSwapChain().GetFlightIndex()];

    renderPassBeginInfo.renderArea.offset = {0, 0};
    renderPassBeginInfo.renderArea.extent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
    renderPassBeginInfo.clearValueCount = clearValues.size();
    renderPassBeginInfo.pClearValues = clearValues.data();
    vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = width;
    viewport.height = height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
  }

  void Framebuffer::Unbind(const CommandBuffer& commandBuffer)
  {
    vkCmdEndRenderPass(commandBuffer);
  }

  VkRenderPass Framebuffer::GetRenderPass() const
  {
    return renderPass;
  }

  VkFramebuffer Framebuffer::GetFramebuffer() const
  {
    return framebuffers[Vulkan::GetSwapChain().GetFlightIndex()];
  }

  const ColorAttachment& Framebuffer::GetColorAttachment() const
  {
    return colorAttachment.GetAsset();
  }

  int Framebuffer::GetWidth() const
  {
    return width;
  }

  int Framebuffer::GetHeight() const
  {
    return height;
  }

  void Framebuffer::InitializeImage(const SamplerCreator& samplerCreator)
  {
    colorAttachment = AssetRef(std::make_unique<ColorAttachment>(width, height, samplerCreator));
  }

  void Framebuffer::InitializeDepthBuffer()
  {
    depthAttachment = std::make_unique<DepthAttachment>(width, height, SamplerCreator{});
  }

  void Framebuffer::InitializeRenderPass()
  {
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = VK_FORMAT_R8G8B8A8_UNORM;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = Image::SelectDepthFormat();
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    std::vector<VkSubpassDependency> dependencies{2};
    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
    dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    dependencies[1].srcSubpass = 0;
    dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    std::vector<VkAttachmentDescription> attachments{colorAttachment, depthAttachment};
    VkRenderPassCreateInfo renderPassCreateInfo{};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.attachmentCount = attachments.size();
    renderPassCreateInfo.pAttachments = attachments.data();
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpass;
    renderPassCreateInfo.dependencyCount = dependencies.size();
    renderPassCreateInfo.pDependencies = dependencies.data();

    CP_VK_ASSERT(vkCreateRenderPass(Vulkan::GetDevice(), &renderPassCreateInfo, nullptr, &renderPass), "Failed to initialze render pass");
  }

  void Framebuffer::InitializeFramebuffers()
  {
    framebuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
    const ColorAttachment& attachment = colorAttachment.GetAsset();
    for (size_t i = 0; i < framebuffers.size(); ++i)
    {
      std::vector<VkImageView> attachments{attachment.GetImageView(i), depthAttachment->GetImageView()};

      VkFramebufferCreateInfo createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
      createInfo.renderPass = renderPass;
      createInfo.attachmentCount = attachments.size();
      createInfo.pAttachments = attachments.data();
      createInfo.width = width;
      createInfo.height = height;
      createInfo.layers = 1;

      CP_VK_ASSERT(vkCreateFramebuffer(Vulkan::GetDevice(), &createInfo, nullptr, &framebuffers[i]), "Failed to initialize framebuffer");
    }
  }
}
