#version 450

layout(location = 0) in vec2 inPosition;

layout(location = 0) out vec2 outTexCoord;

void main()
{
  gl_Position = vec4(inPosition, 0.0, 1.0);
  outTexCoord = inPosition * 0.5 + 0.5;
  outTexCoord.y = 1.0 - outTexCoord.y;
}