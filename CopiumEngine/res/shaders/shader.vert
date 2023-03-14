#version 450

layout(set = 0, binding = 0) uniform SceneUniformBufferObject
{
  mat4 projection;
  mat4 view;
  mat4 model;
  vec3 lightPos;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 outColor;
layout(location = 1) out vec2 outTexCoord;
layout(location = 2) out vec3 outPosition;
layout(location = 3) out vec3 outLightPos;

void main()
{
  gl_Position = ubo.projection * ubo.view * ubo.model * vec4(inPosition, 1.0);
  outColor = inColor;
  outTexCoord = inTexCoord;
  outPosition =  vec3(ubo.model * vec4(inPosition, 1.0));
  outLightPos = ubo.lightPos;
}