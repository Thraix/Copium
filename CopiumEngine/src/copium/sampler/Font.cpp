#include "copium/sampler/Font.h"

#include <msdf-atlas-gen/msdf-atlas-gen.h>

#include "copium/buffer/Buffer.h"
#include "copium/core/Vulkan.h"
#include "copium/sampler/Image.h"

namespace Copium
{
  Font::Font(const MetaFile& metaFile)
    : Sampler{SamplerCreator{metaFile.GetMetaClass("Font")}}
  {
    msdfgen::FreetypeHandle* ft = msdfgen::initializeFreetype();
    CP_ASSERT(ft, "Failed to initialize FreeType");  // TODO: Move to Vulkan singleton class?
    std::string fontPath = metaFile.GetMetaClass("Font").GetValue("filepath");
    msdfgen::FontHandle* font = msdfgen::loadFont(ft, fontPath.c_str());
    CP_ASSERT(font, "Failed to initialize font: %s", fontPath.c_str());

    std::vector<msdf_atlas::GlyphGeometry> glyphs;
    msdf_atlas::FontGeometry fontGeometry(&glyphs);
    fontGeometry.loadCharset(font, 1.0, msdf_atlas::Charset::ASCII);
    const double maxCornerAngle = 3.0;
    for (msdf_atlas::GlyphGeometry& glyph : glyphs)
    {
      glyph.edgeColoring(&msdfgen::edgeColoringInkTrap, maxCornerAngle, 0);
    }
    msdf_atlas::TightAtlasPacker packer;
    packer.setDimensionsConstraint(msdf_atlas::TightAtlasPacker::DimensionsConstraint::SQUARE);
    packer.setMinimumScale(64.0);
    packer.setPixelRange(2.0);
    packer.setMiterLimit(1.0);
    packer.setPadding(2);
    packer.pack(glyphs.data(), glyphs.size());

    int width = 0, height = 0;
    packer.getDimensions(width, height);

    msdf_atlas::
      ImmediateAtlasGenerator<float, 3, msdf_atlas::msdfGenerator, msdf_atlas::BitmapAtlasStorage<msdf_atlas::byte, 3>>
        generator(width, height);
    msdf_atlas::GeneratorAttributes attributes;
    generator.setAttributes(attributes);
    generator.setThreadCount(4);
    generator.generate(glyphs.data(), glyphs.size());

    msdf_atlas::BitmapAtlasStorage<msdf_atlas::byte, 3> pixels = generator.atlasStorage();
    std::vector<uint8_t> data;
    data.resize(width * height * 4);
    msdfgen::Bitmap<msdf_atlas::byte, 3> bitmap = (msdfgen::Bitmap<msdf_atlas::byte, 3>)pixels;
    for (int i = 0; i < width * height; i++)
    {
      data[i * 4] = bitmap[i * 3];
      data[i * 4 + 1] = bitmap[i * 3 + 1];
      data[i * 4 + 2] = bitmap[i * 3 + 2];
      data[i * 4 + 3] = 255;
    }
    InitializeTextureImageFromData(data.data(), width, height);

    for (msdf_atlas::GlyphGeometry& glyphGeom : glyphs)
    {
      Glyph glyph;
      glyph.advance = glyphGeom.getAdvance();
      double l, b, r, t;
      glyphGeom.getQuadPlaneBounds(l, b, r, t);
      glyph.boundingBox = BoundingBox{(float)l, (float)b, (float)r, (float)t};
      glyphGeom.getQuadAtlasBounds(l, b, r, t);
      glyph.texCoordBoundingBox = BoundingBox{(float)l / width, (float)b / height, (float)r / width, (float)t / height};
      this->glyphs.emplace((char)glyphGeom.getCodepoint(), glyph);
    }
    lineHeight = fontGeometry.getMetrics().lineHeight;
    baseHeight = fontGeometry.getMetrics().ascenderY;

    msdfgen::destroyFont(font);
    msdfgen::deinitializeFreetype(ft);
  }

  Font::~Font()
  {
    VkImage imageCpy = image;
    VkDeviceMemory imageMemoryCpy = imageMemory;
    VkImageView imageViewCpy = imageView;
    Vulkan::GetDevice().QueueIdleCommand(
      [imageCpy, imageMemoryCpy, imageViewCpy]()
      {
        vkDestroyImage(Vulkan::GetDevice(), imageCpy, nullptr);
        vkFreeMemory(Vulkan::GetDevice(), imageMemoryCpy, nullptr);
        vkDestroyImageView(Vulkan::GetDevice(), imageViewCpy, nullptr);
      });
  }

  VkDescriptorImageInfo Font::GetDescriptorImageInfo(int index) const
  {
    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.sampler = sampler;
    imageInfo.imageView = imageView;

    return imageInfo;
  }

  const Glyph& Font::GetGlyph(char c) const
  {
    CP_ASSERT(glyphs.find(c) != glyphs.end(), "Glyph not found: %c", c);
    return glyphs.find(c)->second;
  }

  float Font::GetLineHeight() const
  {
    return lineHeight;
  }

  float Font::GetBaseHeight() const
  {
    return baseHeight;
  }

  BoundingBox Font::GetTextBoundingBox(const std::string& str, float size) const
  {
    BoundingBox boundingBox{0.0f};
    glm::vec2 offset{0.0f};
    for (auto c : str)
    {
      if (c == ' ')
      {
        const Glyph& glyph = GetGlyph(c);
        offset.x += glyph.advance;
        continue;
      }
      else if (c == '\t')
      {
        const Glyph& glyph = GetGlyph(' ');
        offset.x += glyph.advance * 4;
        continue;
      }
      else if (c == '\n')
      {
        boundingBox.r = std::max(boundingBox.r, offset.x);
        offset.y -= GetLineHeight();
        offset.x = 0.0f;
        continue;
      }
      const Glyph& glyph = GetGlyph(c);
      boundingBox.l = std::min(boundingBox.l, offset.x + glyph.boundingBox.l);
      boundingBox.t = std::max(boundingBox.t, offset.y + glyph.boundingBox.t);
      offset.x += glyph.advance;
      boundingBox.b = std::min(boundingBox.b, offset.y + glyph.boundingBox.b);
    }
    boundingBox.r = std::max(boundingBox.r, offset.x);
    boundingBox.l *= size;
    boundingBox.b *= size;
    boundingBox.r *= size;
    boundingBox.t *= size;
    return boundingBox;
  }

  void Font::InitializeTextureImageFromData(const uint8_t* rgbaData, int width, int height)
  {
    VkDeviceSize bufferSize = width * height * 4;
    Buffer stagingBuffer{VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                         bufferSize,
                         1};
    void* data = stagingBuffer.Map();
    memcpy(data, rgbaData, bufferSize);
    stagingBuffer.Unmap();

    Image::InitializeImage(width,
                           height,
                           VK_FORMAT_R8G8B8A8_UNORM,
                           VK_IMAGE_TILING_OPTIMAL,
                           VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                           VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                           &image,
                           &imageMemory);
    Image::TransitionImageLayout(
      image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    Image::CopyBufferToImage(stagingBuffer, image, width, height);
    Image::TransitionImageLayout(
      image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    imageView = Image::InitializeImageView(image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
  }
}
