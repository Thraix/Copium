#version 450

layout(set = 0, binding = 0) uniform sampler2D texSamplers[32];

layout(location = 0) in vec3 inColor;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in flat int inTexIndex;
layout(location = 3) in flat int inType;

layout(location = 0) out vec4 outColor;

vec2 GetTextureSize()
{
  if(inTexIndex == 0) return textureSize(texSamplers[0], 0);
  if(inTexIndex == 1) return textureSize(texSamplers[1], 0);
  if(inTexIndex == 2) return textureSize(texSamplers[2], 0);
  if(inTexIndex == 3) return textureSize(texSamplers[3], 0);
  if(inTexIndex == 4) return textureSize(texSamplers[4], 0);
  if(inTexIndex == 5) return textureSize(texSamplers[5], 0);
  if(inTexIndex == 6) return textureSize(texSamplers[6], 0);
  if(inTexIndex == 7) return textureSize(texSamplers[7], 0);
  if(inTexIndex == 8) return textureSize(texSamplers[8], 0);
  if(inTexIndex == 9) return textureSize(texSamplers[9], 0);
  if(inTexIndex == 10) return textureSize(texSamplers[10], 0);
  if(inTexIndex == 11) return textureSize(texSamplers[11], 0);
  if(inTexIndex == 12) return textureSize(texSamplers[12], 0);
  if(inTexIndex == 13) return textureSize(texSamplers[13], 0);
  if(inTexIndex == 14) return textureSize(texSamplers[14], 0);
  if(inTexIndex == 15) return textureSize(texSamplers[15], 0);
  if(inTexIndex == 16) return textureSize(texSamplers[16], 0);
  if(inTexIndex == 17) return textureSize(texSamplers[17], 0);
  if(inTexIndex == 18) return textureSize(texSamplers[18], 0);
  if(inTexIndex == 19) return textureSize(texSamplers[19], 0);
  if(inTexIndex == 20) return textureSize(texSamplers[20], 0);
  if(inTexIndex == 21) return textureSize(texSamplers[21], 0);
  if(inTexIndex == 22) return textureSize(texSamplers[22], 0);
  if(inTexIndex == 23) return textureSize(texSamplers[23], 0);
  if(inTexIndex == 24) return textureSize(texSamplers[24], 0);
  if(inTexIndex == 25) return textureSize(texSamplers[25], 0);
  if(inTexIndex == 26) return textureSize(texSamplers[26], 0);
  if(inTexIndex == 27) return textureSize(texSamplers[27], 0);
  if(inTexIndex == 28) return textureSize(texSamplers[28], 0);
  if(inTexIndex == 29) return textureSize(texSamplers[29], 0);
  if(inTexIndex == 30) return textureSize(texSamplers[30], 0);
  if(inTexIndex == 31) return textureSize(texSamplers[31], 0);
  return vec2(0);
}

vec4 TextureColor()
{
  if(inTexIndex == 0) return texture(texSamplers[0], inTexCoord);
  if(inTexIndex == 1) return texture(texSamplers[1], inTexCoord);
  if(inTexIndex == 2) return texture(texSamplers[2], inTexCoord);
  if(inTexIndex == 3) return texture(texSamplers[3], inTexCoord);
  if(inTexIndex == 4) return texture(texSamplers[4], inTexCoord);
  if(inTexIndex == 5) return texture(texSamplers[5], inTexCoord);
  if(inTexIndex == 6) return texture(texSamplers[6], inTexCoord);
  if(inTexIndex == 7) return texture(texSamplers[7], inTexCoord);
  if(inTexIndex == 8) return texture(texSamplers[8], inTexCoord);
  if(inTexIndex == 9) return texture(texSamplers[9], inTexCoord);
  if(inTexIndex == 10) return texture(texSamplers[10], inTexCoord);
  if(inTexIndex == 11) return texture(texSamplers[11], inTexCoord);
  if(inTexIndex == 12) return texture(texSamplers[12], inTexCoord);
  if(inTexIndex == 13) return texture(texSamplers[13], inTexCoord);
  if(inTexIndex == 14) return texture(texSamplers[14], inTexCoord);
  if(inTexIndex == 15) return texture(texSamplers[15], inTexCoord);
  if(inTexIndex == 16) return texture(texSamplers[16], inTexCoord);
  if(inTexIndex == 17) return texture(texSamplers[17], inTexCoord);
  if(inTexIndex == 18) return texture(texSamplers[18], inTexCoord);
  if(inTexIndex == 19) return texture(texSamplers[19], inTexCoord);
  if(inTexIndex == 20) return texture(texSamplers[20], inTexCoord);
  if(inTexIndex == 21) return texture(texSamplers[21], inTexCoord);
  if(inTexIndex == 22) return texture(texSamplers[22], inTexCoord);
  if(inTexIndex == 23) return texture(texSamplers[23], inTexCoord);
  if(inTexIndex == 24) return texture(texSamplers[24], inTexCoord);
  if(inTexIndex == 25) return texture(texSamplers[25], inTexCoord);
  if(inTexIndex == 26) return texture(texSamplers[26], inTexCoord);
  if(inTexIndex == 27) return texture(texSamplers[27], inTexCoord);
  if(inTexIndex == 28) return texture(texSamplers[28], inTexCoord);
  if(inTexIndex == 29) return texture(texSamplers[29], inTexCoord);
  if(inTexIndex == 30) return texture(texSamplers[30], inTexCoord);
  if(inTexIndex == 31) return texture(texSamplers[31], inTexCoord);
  return vec4(1, 1, 1, 1);
}

float median(float r, float g, float b)
{
  return max(min(r, g), min(max(r, g), b));
}

float screenPxRange()
{
  float pxRange = 2.0f;
  vec2 unitRange = vec2(pxRange) / vec2(GetTextureSize());
  vec2 screenTexSize = vec2(1.0f) / fwidth(inTexCoord);
  return max(0.5 * dot(unitRange, screenTexSize), 1.0);
}

void main()
{
  if(inType == 1)
  {
    vec3 msd = TextureColor().rgb;
    float sd = median(msd.r, msd.g, msd.b);
    float screenPxDistance = screenPxRange() * (sd - 0.5);
    float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);
    outColor = vec4(inColor, mix(0.0, 1.0, opacity));
  }
  else
  {
    outColor = vec4(inColor, 1.0) * TextureColor();
  }
}
