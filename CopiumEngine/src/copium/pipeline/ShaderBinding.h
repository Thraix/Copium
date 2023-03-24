#pragma once

#include <string>
#include <vector>

namespace Copium
{
  enum class BindingType
  {
    Sampler2D, UniformBuffer
  };

  enum class ShaderType
  {
    Vertex, Fragment
  };

  enum class UniformType
  {
    Mat3, Mat4, Vec2, Vec3, Vec4, Float, Int
  };

  struct ShaderBinding
  {
    std::string name;
    uint32_t set;
    uint32_t binding;
    uint32_t arraySize;
    BindingType bindingType;
    ShaderType shaderType;
    std::vector<std::pair<UniformType, std::string>> uniforms;

    // TODO: Maybe store a cache mapping the std::string to the offset in the uniform buffer?

    bool operator<(const ShaderBinding& rhs) const;

    uint32_t GetUniformOffset (const std::string& uniform) const;
    uint32_t GetUniformSize(const std::string& uniform) const;
    UniformType GetUniformType(const std::string& uniform) const;
    uint32_t GetUniformBufferSize() const;
  private:
    uint32_t GetUniformTypeSize(UniformType type) const;
    uint32_t GetUniformTypeOffset(UniformType type) const;
  };
}
