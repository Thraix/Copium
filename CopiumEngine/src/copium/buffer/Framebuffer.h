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

    int width;
    int height;
  public:
    Framebuffer(const MetaFile& metaFile);
    Framebuffer(int width, int height, const SamplerCreator& samplerCreator);
    ~Framebuffer();

    void Resize(int width, int height);
    void Bind(const CommandBuffer& commandBuffer);
    void Unbind(const CommandBuffer& commandBuffer);

    VkRenderPass GetRenderPass() const;
    VkFramebuffer GetFramebuffer() const;
    const ColorAttachment& GetColorAttachment() const;
    int GetWidth() const;
    int GetHeight() const;

  private:
    void InitializeImage(const SamplerCreator& samplerCreator);
    void InitializeDepthBuffer();
    void InitializeRenderPass();
    void InitializeFramebuffers();
  };
}
