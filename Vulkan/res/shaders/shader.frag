#version 450

layout(set = 0, binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragPosition;
layout(location = 3) in vec3 fragLightPos;

layout(location = 0) out vec4 outColor;

void main() {
    float scale = 0.45 +  max(dot(vec3(0, 1, 0), normalize(fragLightPos - fragPosition)), 0.0);
    outColor = vec4(fragColor, 1.0) * texture(texSampler, fragTexCoord) * scale;
}
