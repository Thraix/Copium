#version 450

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in int inTexIndex;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out int fragTexIndex;

layout(set = 1, binding = 0) uniform SceneUniformBufferObject
{
  mat4 projection;
  mat4 view;
} ubo;

void main()
{
  gl_Position = ubo.projection * ubo.view * vec4(inPosition, 0.0, 1.0);
  fragColor = inColor;
  fragTexCoord = inTexCoord;
  fragTexIndex = inTexIndex;
}
