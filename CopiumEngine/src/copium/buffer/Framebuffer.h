#pragma once

#include "copium/asset/Asset.h"
#include "copium/asset/AssetRef.h"
#include "copium/buffer/CommandBuffer.h"
#include "copium/sampler/ColorAttachment.h"
#include "copium/sampler/DepthAttachment.h"
#include "copium/util/Common.h"

#include  <vulkan/vulkan.hpp>

namespace Copium
{
  class Framebuffer final : public Asset
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(Framebuffer);
  private:
    AssetRef<ColorAttachment> colorAttachment;
    std::unique_ptr<DepthAttachment> depthAttachment;
    std::vector<VkFramebuffer> framebuffers;
    VkRenderPass renderPass;

    uint32_t width;
    uint32_t height;
  public:
    Framebuffer(const MetaFile& metaFile);
    Framebuffer(uint32_t width, uint32_t height);
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
