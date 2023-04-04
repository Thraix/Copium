#include "copium/sampler/Texture2D.h"

#include "copium/core/Vulkan.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

namespace Copium
{
  Texture2D::Texture2D(const std::string& filename)
    : Sampler{}
  {
    CP_DEBUG("Texture2D : Loading texture file: %s", filename.c_str());
    InitializeTextureImageFromFile(filename);
  }

  Texture2D::Texture2D(const std::vector<uint8_t>& rgbaData, int width, int height)
    : Sampler{}
  {
    CP_ASSERT(rgbaData.size() == width * height * 4, "rgbaData has invalid size, should be equal to width * height * 4 (%d) actually is %d", width * height * 4, rgbaData.size());
    InitializeTextureImageFromData((void*)rgbaData.data(), width, height);
  }

  Texture2D::~Texture2D()
  {
    vkDestroyImage(Vulkan::GetDevice(), image, nullptr);
    vkFreeMemory(Vulkan::GetDevice(), imageMemory, nullptr);
    vkDestroyImageView(Vulkan::GetDevice(), imageView, nullptr);
  }

  VkDescriptorImageInfo Texture2D::GetDescriptorImageInfo(int index) const
  {
    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.sampler = sampler;
    imageInfo.imageView = imageView;

    return imageInfo;
  }

  void Texture2D::InitializeTextureImageFromFile(const std::string& filename)
  {
    int texWidth;
    int texHeight;
    int texChannels;
    stbi_uc* pixels = stbi_load(filename.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

    CP_ASSERT(pixels, "InitializeTextureImage : Failed to load texture image");

    InitializeTextureImageFromData((void*)pixels, texWidth, texHeight);

    stbi_image_free(pixels);
  }

  void Texture2D::InitializeTextureImageFromData(void* rgbaData, int width, int height)
  {
    VkDeviceSize bufferSize = width * height * 4;
    Buffer stagingBuffer{VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, bufferSize, 1};
    void* data = stagingBuffer.Map();
    memcpy(data, rgbaData, bufferSize);
    stagingBuffer.Unmap();

    Image::InitializeImage(width, height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &image, &imageMemory);
    Image::TransitionImageLayout(image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    Image::CopyBufferToImage(stagingBuffer, image, width, height);
    Image::TransitionImageLayout(image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    imageView = Image::InitializeImageView(image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
  }
}
