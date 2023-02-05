#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>
#include "VertexDescriptor.h"

namespace Copium
{
  struct VertexPassthrough {
    glm::vec2 texCoord;

    static VertexDescriptor GetDescriptor()
    {
      VertexDescriptor descriptor{};
      descriptor.AddAttribute<VertexPassthrough>(0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(VertexPassthrough, texCoord));
      return descriptor;
    }
  };
}
