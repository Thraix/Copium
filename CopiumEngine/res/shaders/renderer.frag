#version 450

layout(set = 0, binding = 0) uniform sampler2D texSamplers[32];

layout(location = 0) in vec3 inColor;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in flat int inTexIndex;

layout(location = 0) out vec4 outColor;

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


void main()
{
  outColor = vec4(inColor, 1.0) * TextureColor();
}
