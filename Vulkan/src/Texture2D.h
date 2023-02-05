#pragma once

#include <cstring>
#include "CommandBufferScoped.h"
#include "Common.h"
#include "Image.h"
#include "Instance.h"

// TODO: Separate Texture2D and Framebuffer Attachments
class Texture2D
{
  CP_DELETE_COPY_AND_MOVE_CTOR(Texture2D);
public:
  enum class Type
  {
    Static, Dynamic 
  };

  enum class Format
  {
    Image, Color, Depth
  };
private:
  Instance& instance;

  std::vector<VkImage> images;
  std::vector<VkDeviceMemory> imageMemories;
  std::vector<VkImageView> imageViews;
  VkSampler sampler;
  Type type;
  Format format;
public:
  Texture2D(Instance& instance, const std::string& filename);
  Texture2D(Instance& instance, int width, int height, Type type, Format format);
  ~Texture2D();

  VkDescriptorImageInfo GetDescriptorImageInfo(int index) const;
  VkImageView GetImageView() const;
  VkImageView GetImageView(int index);
private:
  void InitializeTextureImage(const std::string& filename);
  void InitializeTexture(int width, int height);
  void InitializeSampler();
};

