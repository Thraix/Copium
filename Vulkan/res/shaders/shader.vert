#version 450

layout(set = 0, binding = 0) uniform SceneUniformBufferObject
{
	mat4 projection;
	mat4 view;
	mat4 model;
	vec3 lightPos;
} ubo;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = ubo.projection * ubo.view * ubo.model * vec4(inPosition.x, 0.0, inPosition.y, 1.0);
    fragColor = inColor;
}