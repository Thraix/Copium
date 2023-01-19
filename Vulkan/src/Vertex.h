#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>
#include "VertexDescriptor.h"

struct Vertex {
	glm::vec2 pos;
	glm::vec3 color;
	glm::vec2 texCoord;

  static VertexDescriptor GetDescriptor()
  {
    VertexDescriptor descriptor{};
    descriptor.AddAttribute<Vertex>(0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, pos));
    descriptor.AddAttribute<Vertex>(0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color));
    descriptor.AddAttribute<Vertex>(0, 2, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, texCoord));
    return descriptor;
  }
};

