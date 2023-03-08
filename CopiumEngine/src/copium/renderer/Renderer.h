#pragma once

#include "copium/buffer/CommandBuffer.h"
#include "copium/buffer/IndexBuffer.h"
#include "copium/buffer/VertexBuffer.h"
#include "copium/core/Vulkan.h"
#include "copium/pipeline/Pipeline.h"
#include "copium/pipeline/PipelineCreator.h"
#include "copium/sampler/Texture2D.h"
#include "copium/util/Common.h"

#include <glm/glm.hpp>
#include <vector>

namespace Copium
{
  class Renderer
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(Renderer);

    struct Vertex
    {
      glm::vec2 position;
      glm::vec3 color;
      glm::vec2 texCoord;
      int8_t texIndex;

      static VertexDescriptor GetDescriptor()
      {
        VertexDescriptor descriptor{};
        descriptor.AddAttribute(0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, position), sizeof(Vertex));
        descriptor.AddAttribute(0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color), sizeof(Vertex));
        descriptor.AddAttribute(0, 2, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, texCoord), sizeof(Vertex));
        descriptor.AddAttribute(0, 3, VK_FORMAT_R8_SINT, offsetof(Vertex, texIndex), sizeof(Vertex));
        return descriptor;
      }
    };

  private:
    Vulkan& vulkan;

    DescriptorPool descriptorPool;
    IndexBuffer ibo;
    Texture2D emptyTexture;
    std::unique_ptr<Pipeline> graphicsPipeline;
    std::vector<std::unique_ptr<Buffer>> vbos;
    std::vector<std::unique_ptr<DescriptorSet>> descriptorSets;

    // Temporary data during a render
    CommandBuffer* currentCommandBuffer;
    Buffer* currentVertexBuffer;
    DescriptorSet* currentDescriptorSet;
    std::vector<const Sampler*> samplers;
    int vboIndex;
    int vertexCount;
    int indexCount;
    void* mappedVertexBuffer;
    int texturesUsed;
  public:
    Renderer(Vulkan& vulkan, VkRenderPass renderPass, DescriptorPool& descriptorPool);

    void Quad(const glm::vec2& from, const glm::vec2& to, const glm::vec3& color = glm::vec3{1, 1, 1});
    void Quad(const glm::vec2& from, const glm::vec2& to, const Sampler& sampler, const glm::vec2& texCoord1 = glm::vec2{0, 0}, const glm::vec2& texCoord2 = glm::vec2{1, 1});

    void Begin(CommandBuffer& commandBuffer);
    void End();
  private:

    int AllocateSampler(const Sampler& sampler);
    void AllocateQuad();
    void Flush();
    void NextVertexBuffer();

    void AddVertex(const glm::vec2& position, const glm::vec3& color, int texindex, const glm::vec2& texCoord);
  };
}
