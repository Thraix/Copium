#pragma once

#include "copium/buffer/CommandBuffer.h"
#include "copium/core/Vulkan.h"
#include "copium/sampler/ColorAttachment.h"
#include "copium/sampler/DepthAttachment.h"
#include "copium/util/Common.h"

#include  <vulkan/vulkan.hpp>

namespace Copium
{
  class Framebuffer final
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(Framebuffer);
  private:
    Vulkan& vulkan;

    std::unique_ptr<ColorAttachment> colorAttachment;
    std::unique_ptr<DepthAttachment> depthAttachment;
    std::vector<VkFramebuffer> framebuffers;
    VkRenderPass renderPass;

    uint32_t width;
    uint32_t height;
  public:
    Framebuffer(Vulkan& vulkan, uint32_t width, uint32_t height);
    ~Framebuffer();

    void Resize(uint32_t width, uint32_t height);
    void Bind(const CommandBuffer& commandBuffer);
    void Unbind(const CommandBuffer& commandBuffer);

    VkRenderPass GetRenderPass() const;
    VkFramebuffer GetFramebuffer() const;
    const ColorAttachment& GetColorAttachment() const;
    uint32_t GetWidth() const;
    uint32_t GetHeight() const;

  private:
    void InitializeImage();
    void InitializeDepthBuffer();
    void InitializeRenderPass();
    void InitializeFramebuffers();
  };
}