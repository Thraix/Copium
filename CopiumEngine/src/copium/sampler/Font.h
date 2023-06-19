#pragma once

#include "copium/sampler/Sampler.h"
#include "copium/sampler/Glyph.h"
#include "copium/util/BoundingBox.h"

namespace Copium
{
  class Font : public Sampler
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(Font);
  private:
    VkImage image;
    VkDeviceMemory imageMemory;
    VkImageView imageView;

    std::map<char, Glyph> glyphs;
    float lineHeight;
    float baseHeight;
  public:
    Font(const MetaFile& metaFile);
    ~Font() override;

    VkDescriptorImageInfo GetDescriptorImageInfo(int index) const override;

    const Glyph& GetGlyph(char c) const;
    float GetLineHeight() const;
    float GetBaseHeight() const;

    BoundingBox GetTextBoundingBox(const std::string& str, float size) const;
  private:
    void InitializeTextureImageFromData(const uint8_t* rgbaData, int width, int height);
  };
}
