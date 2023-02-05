#pragma once

#include "Common.h"
#include "Image.h"
#include "Instance.h"
#include "Texture2D.h"

#include  <vulkan/vulkan.hpp>

namespace Copium
{
  // TODO: Add resizing (recreate image, depthImage, framebuffers)
  class Framebuffer
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(Framebuffer);
  private:
    Instance& instance;

    std::unique_ptr<Texture2D> image;
    std::unique_ptr<Texture2D> depthImage;
    std::vector<VkFramebuffer> framebuffers;
    VkRenderPass renderPass;

    uint32_t width;
    uint32_t height;
  public:
    Framebuffer(Instance& instance, uint32_t width, uint32_t height)
      : instance{instance}, width{width}, height{height}
    {
      InitializeImages();
      InitializeDepthBuffer();
      InitializeRenderPass();
      InitializeFramebuffers();
    }

    ~Framebuffer()
    {
      for (auto& framebuffer : framebuffers)
        vkDestroyFramebuffer(instance.GetDevice(), framebuffer, nullptr);
      vkDestroyRenderPass(instance.GetDevice(), renderPass, nullptr);
    }

    void Bind(const CommandBuffer& commandBuffer)
    {
      std::vector<VkClearValue> clearValues{2};
      clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
      clearValues[1].depthStencil = {1.0f, 0};

      VkRenderPassBeginInfo renderPassBeginInfo{};
      renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
      renderPassBeginInfo.renderPass = renderPass;
      renderPassBeginInfo.framebuffer = framebuffers[instance.GetFlightIndex()];
      renderPassBeginInfo.renderArea.offset = {0, 0};
      renderPassBeginInfo.renderArea.extent = {width, height};
      renderPassBeginInfo.clearValueCount = clearValues.size();
      renderPassBeginInfo.pClearValues = clearValues.data();
      vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

    void Unbind(const CommandBuffer& commandBuffer)
    {
      vkCmdEndRenderPass(commandBuffer);
    }

    VkRenderPass GetRenderPass() const
    {
      return renderPass;
    }

    VkFramebuffer GetFramebuffer() const
    {
      return framebuffers[instance.GetFlightIndex()];
    }

    const Texture2D& GetTexture2D() const
    {
      return *image;
    }

  private:

    void InitializeImages()
    {
      image = std::make_unique<Texture2D>(instance, width, height, Texture2D::Type::Dynamic, Texture2D::Format::Color);
    }

    void InitializeDepthBuffer()
    {
      depthImage = std::make_unique<Texture2D>(instance, width, height, Texture2D::Type::Static, Texture2D::Format::Depth);
    }

    void InitializeRenderPass()
    {
      VkAttachmentDescription colorAttachment{};
      colorAttachment.format = VK_FORMAT_R8G8B8A8_SRGB;
      colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
      colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
      colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
      colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
      colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
      colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

      VkAttachmentDescription depthAttachment{};
      depthAttachment.format = Image::SelectDepthFormat(instance);
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

      CP_VK_ASSERT(vkCreateRenderPass(instance.GetDevice(), &renderPassCreateInfo, nullptr, &renderPass), "InitializeRenderPass : Failed to initialze render pass");
    }

    void InitializeFramebuffers()
    {
      framebuffers.resize(instance.GetMaxFramesInFlight());

      for (size_t i = 0; i < instance.GetMaxFramesInFlight(); ++i)
      {
        std::vector<VkImageView> attachments{image->GetImageView(i), depthImage->GetImageView()};

        VkFramebufferCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        createInfo.renderPass = renderPass;
        createInfo.attachmentCount = attachments.size();
        createInfo.pAttachments = attachments.data();
        createInfo.width = width;
        createInfo.height = height;
        createInfo.layers = 1;

        CP_VK_ASSERT(vkCreateFramebuffer(instance.GetDevice(), &createInfo, nullptr, &framebuffers[i]), "InitializeFramebuffers : Failed to initialize swap chain framebuffer");
      }
    }
  };
}