#pragma once

#include "copium/sampler/Sampler.h"
#include "copium/sampler/Glyph.h"

namespace Copium
{
  class Font : public Sampler
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(Font);
    struct GlyphData
    {

    };
  private:
    VkImage image;
    VkDeviceMemory imageMemory;
    VkImageView imageView;

    std::map<char, Glyph> glyphs;
    float lineHeight;
  public:
    Font(const MetaFile& metaFile);
    ~Font() override;

    VkDescriptorImageInfo GetDescriptorImageInfo(int index) const override;

    const Glyph& GetGlyph(char c) const;
    float GetLineHeight() const;
  private:
    void InitializeTextureImageFromFile(const std::string& filename);
    void InitializeTextureImageFromData(const uint8_t* rgbaData, int width, int height);
  };
}
