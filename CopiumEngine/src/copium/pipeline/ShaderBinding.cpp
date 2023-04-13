#include "copium/pipeline/ShaderBinding.h"

#include "copium/util/Common.h"

namespace Copium
{
  bool ShaderBinding::operator<(const ShaderBinding& rhs) const
  {
    if (set != rhs.set)
      return set < rhs.set;
    return binding < rhs.binding;
  }

  uint32_t ShaderBinding::GetUniformOffset(const std::string& uniform) const
  {
    // TODO: Caching?
    uint32_t offset = 0;
    for (auto& uniformElem : uniforms)
    {
      if (uniformElem.second == uniform)
        return offset;
      offset += GetUniformTypeOffset(uniformElem.first);
    }
    CP_ABORT("Uniform not found=%s", uniform);
  }

  uint32_t ShaderBinding::GetUniformSize(const std::string& uniform) const
  {
    for (auto& uniformElem : uniforms)
    {
      if (uniformElem.second == uniform)
        return GetUniformTypeSize(uniformElem.first);
    }
    CP_ABORT("Uniform not found=%s", uniform);
  }

  UniformType ShaderBinding::GetUniformType(const std::string& uniform) const
  {
    for (auto& uniformElem : uniforms)
    {
      if (uniformElem.second == uniform)
        return uniformElem.first;
    }
    CP_ABORT("Uniform not found=%s", uniform);
  }

  uint32_t ShaderBinding::GetUniformBufferSize() const
  {
    CP_ASSERT(bindingType == BindingType::UniformBuffer, "BindingType is not UniformBuffer");

    uint32_t size = 0;
    for (auto& uniform : uniforms)
    {
      size += GetUniformTypeOffset(uniform.first);
    }

    // alignas(64)
    if (size % 64 != 0)
      size += 64 - (size % 64);
    return size;
  }

  uint32_t ShaderBinding::GetUniformTypeSize(UniformType type) const
  {
    switch (type)
    {
    case UniformType::Mat3:
      return 4 * 9; // glm::mat3
    case UniformType::Mat4:
      return 4 * 16; // glm::mat4
    case UniformType::Vec2:
      return 4 * 2; // glm::vec2
    case UniformType::Vec3:
      return 4 * 3; // glm::vec3
    case UniformType::Vec4:
      return 4 * 4; // glm::vec4
    case UniformType::Int:
      return 4; // int
    case UniformType::Float:
      return 4; // float
    default:
      CP_ABORT("Unhandled switch case");
    }
  }

  uint32_t ShaderBinding::GetUniformTypeOffset(UniformType type) const
  {
    switch (type)
    {
    case UniformType::Mat3:
      return 64; // alignas(16) glm::mat3
    case UniformType::Mat4:
      return 64; // alignas(16) glm::mat4
    case UniformType::Vec2:
      return 16; // alignas(16) glm::vec2
    case UniformType::Vec3:
      return 16; // alignas(16) glm::vec2
    case UniformType::Vec4:
      return 16; // alignas(16) glm::vec2
    case UniformType::Int:
      return 16; // alignas(16) glm::vec2
    case UniformType::Float:
      return 16; // alignas(16) glm::vec2
    default:
      CP_ABORT("Unhandled switch case");
    }
  }
}
