#version 450

layout(set = 0, binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 inColor;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inPosition;
layout(location = 3) in vec3 inLightPos;

layout(location = 0) out vec4 outColor;

void main()
{
  float scale = 0.45 +  max(dot(vec3(0, 1, 0), normalize(inLightPos - inPosition)), 0.0);
  outColor = vec4(inColor, 1.0) * texture(texSampler, inTexCoord) * scale;
}
