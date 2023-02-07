#pragma once

#include "Common.h"
#include "Instance.h"
#include "ColorAttachment.h"
#include "DepthAttachment.h"

#include  <vulkan/vulkan.hpp>

namespace Copium
{
  class Framebuffer final
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(Framebuffer);
  private:
    Instance& instance;

    std::unique_ptr<ColorAttachment> colorAttachment;
    std::unique_ptr<DepthAttachment> depthAttachment;
    std::vector<VkFramebuffer> framebuffers;
    VkRenderPass renderPass;

    uint32_t width;
    uint32_t height;
  public:
    Framebuffer(Instance& instance, uint32_t width, uint32_t height);
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