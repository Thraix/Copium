#include "copium/renderer/Renderer.h"

#include "copium/core/Vulkan.h"
#include "copium/pipeline/PipelineCreator.h"
#include "copium/renderer/RendererVertex.h"

namespace Copium
{
  static constexpr int MAX_NUM_QUADS = 10000;
  static constexpr int MAX_NUM_VERTICES = 4 * MAX_NUM_QUADS;
  static constexpr int MAX_NUM_INDICES = 6 * MAX_NUM_QUADS;
  static constexpr int MAX_NUM_TEXTURES = 32;

  Renderer::Renderer(const AssetRef<Pipeline>& pipeline)
    : ibo{MAX_NUM_INDICES},
      pipeline{pipeline},
      samplers{MAX_NUM_TEXTURES, &Vulkan::GetEmptyTexture2D().GetAsset()}
  {
    InitializeIndexBuffer();
  }

  void Renderer::Quad(const glm::vec2& pos, const glm::vec2& size, const glm::vec3& color)
  {
    AllocateQuad();
    AddVertex(pos, color, -1, glm::vec2{0, 0}, RendererVertex::TYPE_QUAD);
    AddVertex(glm::vec2{pos.x, pos.y + size.y}, color, -1, glm::vec2{0, 0}, RendererVertex::TYPE_QUAD);
    AddVertex(pos + size, color, -1, glm::vec2{0, 0}, RendererVertex::TYPE_QUAD);
    AddVertex(glm::vec2{pos.x + size.x, pos.y}, color, -1, glm::vec2{0, 0}, RendererVertex::TYPE_QUAD);
  }

  void Renderer::Quad(const glm::vec2& pos,
                      const glm::vec2& size,
                      const Sampler& sampler,
                      const glm::vec2& texCoord1,
                      const glm::vec2& texCoord2)
  {
    AllocateQuad();
    int texIndex = AllocateSampler(sampler);
    AddVertex(pos, glm::vec3{1, 1, 1}, texIndex, texCoord1, RendererVertex::TYPE_QUAD);
    AddVertex(glm::vec2{pos.x, pos.y + size.y},
              glm::vec3{1, 1, 1},
              texIndex,
              glm::vec2{texCoord1.x, texCoord2.y},
              RendererVertex::TYPE_QUAD);
    AddVertex(pos + size, glm::vec3{1, 1, 1}, texIndex, texCoord2, RendererVertex::TYPE_QUAD);
    AddVertex(glm::vec2{pos.x + size.x, pos.y},
              glm::vec3{1, 1, 1},
              texIndex,
              glm::vec2{texCoord2.x, texCoord1.y},
              RendererVertex::TYPE_QUAD);
  }

  glm::vec2 Renderer::Text(
    const std::string& str, const glm::vec2& position, const Font& font, float size, const glm::vec3& color)
  {
    glm::vec2 offset = position;
    for (char c : str)
    {
      if (c == ' ')
      {
        const Glyph& glyph = font.GetGlyph(c);
        offset.x += glyph.advance * size;
        continue;
      }
      else if (c == '\t')
      {
        const Glyph& glyph = font.GetGlyph(' ');
        offset.x += glyph.advance * size * 4;
        continue;
      }
      else if (c == '\n')
      {
        offset.y -= font.GetLineHeight() * size;
        offset.x = position.x;
        continue;
      }
      const Glyph& glyph = font.GetGlyph(c);
      AllocateQuad();
      int texIndex = AllocateSampler(font);
      AddVertex(offset + glm::vec2{glyph.boundingBox.l * size, glyph.boundingBox.b * size},
                color,
                texIndex,
                glyph.texCoordBoundingBox.AsLb(),
                RendererVertex::TYPE_TEXT);
      AddVertex(offset + glm::vec2{glyph.boundingBox.l * size, glyph.boundingBox.t * size},
                color,
                texIndex,
                glm::vec2{glyph.texCoordBoundingBox.l, glyph.texCoordBoundingBox.t},
                RendererVertex::TYPE_TEXT);
      AddVertex(offset + glm::vec2{glyph.boundingBox.r * size, glyph.boundingBox.t * size},
                color,
                texIndex,
                glyph.texCoordBoundingBox.AsRt(),
                RendererVertex::TYPE_TEXT);
      AddVertex(offset + glm::vec2{glyph.boundingBox.r * size, glyph.boundingBox.b * size},
                color,
                texIndex,
                glm::vec2{glyph.texCoordBoundingBox.r, glyph.texCoordBoundingBox.b},
                RendererVertex::TYPE_TEXT);
      offset.x += glyph.advance * size;
    }
    return offset;
  }

  glm::vec2 Renderer::TextUi(
    const std::string& str, const glm::vec2& position, const Font& font, float size, const glm::vec3& color)
  {
    glm::vec2 offset = position;
    for (char c : str)
    {
      if (c == ' ')
      {
        const Glyph& glyph = font.GetGlyph(c);
        offset.x += glyph.advance * size;
        continue;
      }
      else if (c == '\t')
      {
        const Glyph& glyph = font.GetGlyph(' ');
        offset.x += glyph.advance * size * 4;
        continue;
      }
      else if (c == '\n')
      {
        offset.y += font.GetLineHeight() * size;
        offset.x = position.x;
        continue;
      }
      const Glyph& glyph = font.GetGlyph(c);
      AllocateQuad();
      int texIndex = AllocateSampler(font);
      AddVertex(offset + glm::vec2{glyph.boundingBox.l * size, -glyph.boundingBox.t * size},
                color,
                texIndex,
                glm::vec2{glyph.texCoordBoundingBox.l, glyph.texCoordBoundingBox.t},
                RendererVertex::TYPE_TEXT);
      AddVertex(offset + glm::vec2{glyph.boundingBox.l * size, -glyph.boundingBox.b * size},
                color,
                texIndex,
                glm::vec2{glyph.texCoordBoundingBox.l, glyph.texCoordBoundingBox.b},
                RendererVertex::TYPE_TEXT);
      AddVertex(offset + glm::vec2{glyph.boundingBox.r * size, -glyph.boundingBox.b * size},
                color,
                texIndex,
                glm::vec2{glyph.texCoordBoundingBox.r, glyph.texCoordBoundingBox.b},
                RendererVertex::TYPE_TEXT);
      AddVertex(offset + glm::vec2{glyph.boundingBox.r * size, -glyph.boundingBox.t * size},
                color,
                texIndex,
                glm::vec2{glyph.texCoordBoundingBox.r, glyph.texCoordBoundingBox.t},
                RendererVertex::TYPE_TEXT);
      offset.x += glyph.advance * size;
    }
    return offset;
  }

  void Renderer::AddVertex(
    const glm::vec2& position, const glm::vec3& color, int texindex, const glm::vec2& texCoord, int type)
  {
    RendererVertex* vertex = (RendererVertex*)mappedVertexBuffer;
    vertex->position = position;
    vertex->color = color;
    vertex->texCoord = texCoord;
    vertex->texIndex = texindex;
    vertex->type = type;
    mappedVertexBuffer = (RendererVertex*)mappedVertexBuffer + 1;
  }

  void Renderer::Begin(CommandBuffer& commandBuffer)
  {
    pipeline.GetAsset().Bind(commandBuffer);
    ibo.Bind(commandBuffer);
    batchIndex = -1;
    NextBatch();
    currentCommandBuffer = &commandBuffer;
  }

  void Renderer::End()
  {
    Flush();
  }

  Pipeline& Renderer::GetGraphicsPipeline()
  {
    return pipeline.GetAsset();
  }

  void Renderer::SetDescriptorSet(const DescriptorSet& descriptorSet)
  {
    pipeline.GetAsset().SetDescriptorSet(descriptorSet);
  }

  void Renderer::InitializeIndexBuffer()
  {
    CP_ASSERT(MAX_NUM_INDICES < std::numeric_limits<uint16_t>::max(), "Maximum number of indices too big");

    std::vector<uint16_t> indices;
    indices.resize(MAX_NUM_INDICES);
    for (int i = 0; i < MAX_NUM_QUADS; i++)
    {
      indices[i * 6] = i * 4;
      indices[i * 6 + 1] = i * 4 + 1;
      indices[i * 6 + 2] = i * 4 + 2;
      indices[i * 6 + 3] = i * 4;
      indices[i * 6 + 4] = i * 4 + 2;
      indices[i * 6 + 5] = i * 4 + 3;
    }
    ibo.UpdateStaging(indices.data());
  }

  int Renderer::AllocateSampler(const Sampler& sampler)
  {
    for (size_t i = 0; i < textureCount; i++)
    {
      if (*samplers[i] == (VkSampler)sampler)
      {
        return i;
      }
    }
    if (textureCount == MAX_NUM_TEXTURES)
    {
      Flush();
      NextBatch();
    }
    batches[batchIndex]->GetDescriptorSet().SetSamplerDynamic(sampler, 0, textureCount);

    samplers[textureCount] = &sampler;
    textureCount++;
    return textureCount - 1;
  }

  void Renderer::AllocateQuad()
  {
    if (quadCount + 1 > MAX_NUM_QUADS)
    {
      Flush();
      NextBatch();
    }
    quadCount++;
  }

  void Renderer::Flush()
  {
    batches[batchIndex]->GetVertexBuffer().Unmap();
    batches[batchIndex]->GetVertexBuffer().Bind(*currentCommandBuffer);
    Pipeline& p = pipeline.GetAsset();
    p.SetDescriptorSet(batches[batchIndex]->GetDescriptorSet());
    p.BindDescriptorSets(*currentCommandBuffer);
    ibo.Draw(*currentCommandBuffer, quadCount * 6);
  }

  void Renderer::NextBatch()
  {
    batchIndex++;
    std::fill(samplers.begin(), samplers.end(), &Vulkan::GetEmptyTexture2D().GetAsset());
    if (batchIndex >= batches.size())
    {
      batches.emplace_back(std::make_unique<Batch>(pipeline, MAX_NUM_VERTICES, samplers));
    }
    batches[batchIndex]->GetDescriptorSet().SetSamplersDynamic(samplers, 0);
    mappedVertexBuffer = (char*)batches[batchIndex]->GetVertexBuffer().Map() +
                         batches[batchIndex]->GetVertexBuffer().GetPosition(Vulkan::GetSwapChain().GetFlightIndex());
    quadCount = 0;
    textureCount = 0;
  }
}
