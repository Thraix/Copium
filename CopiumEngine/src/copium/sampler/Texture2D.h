#pragma once

#include "copium/sampler/Sampler.h"
#include "copium/util/Common.h"

namespace Copium
{
  class Texture2D final : public Sampler
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(Texture2D);
  private:
    VkImage image;
    VkDeviceMemory imageMemory;
    VkImageView imageView;

    int width;
    int height;
  public:
    Texture2D(const MetaFile& metaFile);
    Texture2D(const std::vector<uint8_t>& rgbaData, int width, int height, const SamplerCreator& samplerCreator);
    ~Texture2D() override;

    VkDescriptorImageInfo GetDescriptorImageInfo(int index) const override;

    int GetWidth() const;
    int GetHeight() const;

  private:
    void InitializeTextureImageFromFile(const std::string& filename);
    void InitializeTextureImageFromData(const uint8_t* rgbaData, int width, int height);
  };
}
