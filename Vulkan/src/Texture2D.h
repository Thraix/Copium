#pragma once

#include <cstring>
#include "CommandBufferScoped.h"
#include "Common.h"
#include "Image.h"
#include "Instance.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

class Texture2D
{
  CP_DELETE_COPY_AND_MOVE_CTOR(Texture2D);
private:
  Instance& instance;

  VkImage image;
  VkDeviceMemory imageMemory;
  VkImageView imageView;
  VkSampler sampler;
public:
  Texture2D(Instance& instance, const std::string& filename)
    : instance{instance}
  {
    InitializeTextureImage(filename);
    InitializeSampler();
  }

  ~Texture2D()
  {
    vkDestroyImage(instance.GetDevice(), image, nullptr);
    vkFreeMemory(instance.GetDevice(), imageMemory, nullptr);
    vkDestroyImageView(instance.GetDevice(), imageView, nullptr);
    vkDestroySampler(instance.GetDevice(), sampler, nullptr);
  }

  VkDescriptorImageInfo GetDescriptorImageInfo() const
  {
    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = imageView;
    imageInfo.sampler = sampler;

    return imageInfo;
  }
private:
  void InitializeTextureImage(const std::string& filename)
  {
    int texWidth;
    int texHeight;
    int texChannels;
    stbi_uc* pixels = stbi_load(filename.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

    CP_ASSERT(pixels, "Failed to load texture image");

    VkDeviceSize bufferSize = texWidth * texHeight * 4;
    Buffer stagingBuffer{instance, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, bufferSize, 1};
    void* data = stagingBuffer.Map();
    memcpy(data, pixels, bufferSize);
    stagingBuffer.Unmap();
    stbi_image_free(pixels);

    Image::InitializeImage(instance, texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &image, &imageMemory);
    Image::TransitionImageLayout(instance, image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    Image::CopyBufferToImage(instance, stagingBuffer, image, texWidth, texHeight);
    Image::TransitionImageLayout(instance, image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    imageView = Image::InitializeImageView(instance, image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
  }

  void InitializeSampler()
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

    CP_VK_ASSERT(vkCreateSampler(instance.GetDevice(), &createInfo, nullptr, &sampler), "Failed to initialize texture sampler");
  }

  VkImageView CreateImageView(VkImage image)
  {
    VkImageView imageView;
    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = image;
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;
    CP_VK_ASSERT(vkCreateImageView(instance.GetDevice(), &createInfo, nullptr, &imageView), "Failed to initialize ImageView");

    return imageView;
  }

};

