#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>
#include "VertexDescriptor.h"

struct Vertex {
	glm::vec2 pos;
	glm::vec3 color;

  static VertexDescriptor GetDescriptor()
  {
    VertexDescriptor descriptor{};
    descriptor.AddAttribute<Vertex>(0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, pos));
    descriptor.AddAttribute<Vertex>(0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color));
    return descriptor;
  }
  
  static VkVertexInputBindingDescription GetBindingDescription()
  {
    VkVertexInputBindingDescription description{};

    description.binding = 0;
    description.stride = sizeof(Vertex);
    description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return description;
  }

  static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions()
  {
    std::vector<VkVertexInputAttributeDescription> descriptions{2};

    descriptions[0].binding = 0;
    descriptions[0].location = 0;
    descriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    descriptions[0].offset = offsetof(Vertex, pos);

    descriptions[1].binding = 0;
    descriptions[1].location = 1;
    descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    descriptions[1].offset = offsetof(Vertex, color);

    return descriptions;
  }
};

