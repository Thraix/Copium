#include "Texture2D.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

namespace Copium
{
  Texture2D::Texture2D(Instance& instance, const std::string& filename)
    : instance{instance}, type{Type::Static}, format{Format::Image}
  {
    InitializeTextureImage(filename);
    InitializeSampler();
  }

  Texture2D::Texture2D(Instance& instance, int width, int height, Type type, Format format)
    : instance{instance}, type{type}, format{format}
  {
    InitializeTexture(width, height);
    InitializeSampler();
  }

  Texture2D::~Texture2D()
  {
    for (auto&& image : images)
      vkDestroyImage(instance.GetDevice(), image, nullptr);
    for (auto&& imageMemory : imageMemories)
      vkFreeMemory(instance.GetDevice(), imageMemory, nullptr);
    for (auto&& imageView : imageViews)
      vkDestroyImageView(instance.GetDevice(), imageView, nullptr);
    vkDestroySampler(instance.GetDevice(), sampler, nullptr);
  }

  VkDescriptorImageInfo Texture2D::GetDescriptorImageInfo(int index) const
  {
    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.sampler = sampler;

    switch (type) {
    case Type::Static:
      imageInfo.imageView = imageViews.front();
      break;
    case Type::Dynamic:
      CP_ASSERT(index >= 0 && index < imageViews.size(), "GetDescriptorImageInfo : index out of bound for dynamic texture");
      imageInfo.imageView = imageViews[index];
      break;
    default:
      CP_ABORT("GetDescriptorImageInfo : Unreachable switch case");
    }

    return imageInfo;
  }

  VkImageView Texture2D::GetImageView() const
  {
    CP_ASSERT(type == Type::Static, "GetImageView : Texture2D is not static");
    return imageViews.front();
  }

  VkImageView Texture2D::GetImageView(int index)
  {
    CP_ASSERT(type == Type::Dynamic && index >= 0 && index < imageViews.size(), "GetImageView : Texture2D is not dynamic or index out of bound for SystemTexture");
    return imageViews[index];
  }

  void Texture2D::InitializeTextureImage(const std::string& filename)
  {
    int texWidth;
    int texHeight;
    int texChannels;
    stbi_uc* pixels = stbi_load(filename.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

    CP_ASSERT(pixels, "InitializeTextureImage : Failed to load texture image");

    VkDeviceSize bufferSize = texWidth * texHeight * 4;
    Buffer stagingBuffer{instance, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, bufferSize, 1};
    void* data = stagingBuffer.Map();
    memcpy(data, pixels, bufferSize);
    stagingBuffer.Unmap();
    stbi_image_free(pixels);

    images.resize(1);
    imageMemories.resize(1);
    imageViews.resize(1);
    Image::InitializeImage(instance, texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &images.front(), &imageMemories.front());
    Image::TransitionImageLayout(instance, images.front(), VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    Image::CopyBufferToImage(instance, stagingBuffer, images.front(), texWidth, texHeight);
    Image::TransitionImageLayout(instance, images.front(), VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    imageViews[0] = Image::InitializeImageView(instance, images.front(), VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
  }

  void Texture2D::InitializeTexture(int width, int height)
  {
    int count = 1;
    if (type == Type::Dynamic)
      count = instance.GetMaxFramesInFlight();
    images.resize(count);
    imageMemories.resize(count);
    imageViews.resize(count);
    for (size_t i = 0; i < images.size(); i++)
    {
      switch (format)
      {
      case Format::Color:
      {
        Image::InitializeImage(instance, width, height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &images[i], &imageMemories[i]);
        // Image::TransitionImageLayout(instance, images[i], VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        imageViews[i] = Image::InitializeImageView(instance, images[i], VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
        break;
      }
      case Format::Depth:
      {
        VkFormat depthFormat = Image::SelectDepthFormat(instance);
        Image::InitializeImage(instance, width, height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &images[i], &imageMemories[i]);
        // Image::TransitionImageLayout(instance, images[i], depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
        imageViews[i] = Image::InitializeImageView(instance, images[i], depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
        break;
      }
      case Format::Image:
      {
        CP_ABORT("InitializeTexture : Image format currently not supported");
      }
      default:
        CP_ABORT("InitializeTexture : Unreachable switch case");
      }
    }
  }

  void Texture2D::InitializeSampler()
  {
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(instance.GetPhysicalDevice(), &properties);

    VkSamplerCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    createInfo.magFilter = VK_FILTER_LINEAR;
    createInfo.minFilter = VK_FILTER_LINEAR;
    createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    createInfo.anisotropyEnable = VK_TRUE;
    createInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    createInfo.unnormalizedCoordinates = VK_FALSE;
    createInfo.compareEnable = VK_FALSE;
    createInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    createInfo.mipLodBias = 0.0f;
    createInfo.minLod = 0.0f;
    createInfo.maxLod = 0.0f;

    CP_VK_ASSERT(vkCreateSampler(instance.GetDevice(), &createInfo, nullptr, &sampler), "InitializeSampler : Failed to initialize texture sampler");
  }
}
