#version 450

layout(set = 0, binding = 0) uniform sampler2D texSamplers[32];

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in flat int fragTexIndex;

layout(location = 0) out vec4 outColor;

vec4 TextureColor()
{
  if(fragTexIndex == 0) return texture(texSamplers[0], fragTexCoord);
  if(fragTexIndex == 1) return texture(texSamplers[1], fragTexCoord);
  if(fragTexIndex == 2) return texture(texSamplers[2], fragTexCoord);
  if(fragTexIndex == 3) return texture(texSamplers[3], fragTexCoord);
  if(fragTexIndex == 4) return texture(texSamplers[4], fragTexCoord);
  if(fragTexIndex == 5) return texture(texSamplers[5], fragTexCoord);
  if(fragTexIndex == 6) return texture(texSamplers[6], fragTexCoord);
  if(fragTexIndex == 7) return texture(texSamplers[7], fragTexCoord);
  if(fragTexIndex == 8) return texture(texSamplers[8], fragTexCoord);
  if(fragTexIndex == 9) return texture(texSamplers[9], fragTexCoord);
  if(fragTexIndex == 10) return texture(texSamplers[10], fragTexCoord);
  if(fragTexIndex == 11) return texture(texSamplers[11], fragTexCoord);
  if(fragTexIndex == 12) return texture(texSamplers[12], fragTexCoord);
  if(fragTexIndex == 13) return texture(texSamplers[13], fragTexCoord);
  if(fragTexIndex == 14) return texture(texSamplers[14], fragTexCoord);
  if(fragTexIndex == 15) return texture(texSamplers[15], fragTexCoord);
  if(fragTexIndex == 16) return texture(texSamplers[16], fragTexCoord);
  if(fragTexIndex == 17) return texture(texSamplers[17], fragTexCoord);
  if(fragTexIndex == 18) return texture(texSamplers[18], fragTexCoord);
  if(fragTexIndex == 19) return texture(texSamplers[19], fragTexCoord);
  if(fragTexIndex == 20) return texture(texSamplers[20], fragTexCoord);
  if(fragTexIndex == 21) return texture(texSamplers[21], fragTexCoord);
  if(fragTexIndex == 22) return texture(texSamplers[22], fragTexCoord);
  if(fragTexIndex == 23) return texture(texSamplers[23], fragTexCoord);
  if(fragTexIndex == 24) return texture(texSamplers[24], fragTexCoord);
  if(fragTexIndex == 25) return texture(texSamplers[25], fragTexCoord);
  if(fragTexIndex == 26) return texture(texSamplers[26], fragTexCoord);
  if(fragTexIndex == 27) return texture(texSamplers[27], fragTexCoord);
  if(fragTexIndex == 28) return texture(texSamplers[28], fragTexCoord);
  if(fragTexIndex == 29) return texture(texSamplers[29], fragTexCoord);
  if(fragTexIndex == 30) return texture(texSamplers[30], fragTexCoord);
  if(fragTexIndex == 31) return texture(texSamplers[31], fragTexCoord);
  return vec4(1, 1, 1, 1);
}


void main() {
  outColor = vec4(fragColor, 1.0) * TextureColor();
  // outColor = vec4(fragTexCoord, 0.0, 1.0);
}
