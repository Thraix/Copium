#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "copium/util/Enum.h"

#define CP_BINDING_TYPE_ENUMS Sampler2D, UniformBuffer
#define CP_SHADER_TYPE_ENUMS Vertex, Fragment
#define CP_UNIFORM_TYPE_ENUMS Mat3, Mat4, Vec2, Vec3, Vec4, Float, Int

CP_ENUM_CREATOR(Copium, BindingType, CP_BINDING_TYPE_ENUMS);
CP_ENUM_CREATOR(Copium, ShaderType, CP_SHADER_TYPE_ENUMS);
CP_ENUM_CREATOR(Copium, UniformType, CP_UNIFORM_TYPE_ENUMS);

namespace Copium
{
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

    uint32_t GetUniformOffset(const std::string& uniform) const;
    uint32_t GetUniformSize(const std::string& uniform) const;
    UniformType GetUniformType(const std::string& uniform) const;
    uint32_t GetUniformBufferSize() const;

  private:
    uint32_t GetUniformTypeSize(UniformType type) const;
    uint32_t GetUniformTypeOffset(UniformType type) const;
  };
}
