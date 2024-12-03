#version 460

layout (location = 0) in vec2 vtxUv;
layout (location = 1) in vec4 vtxColor;
layout (location = 2) in float time; // Add time uniform for animation

layout (binding = 0) uniform sampler2D tex;

layout (location = 0) out vec4 outColor;

// Convert HSV to RGB
vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
    // Sample the texture
    vec4 texColor = texture(tex, vtxUv);

    // Create rainbow effect by rotating through hue
    float hue = fract(time * 0.2 + vtxUv.x * 0.5 + vtxUv.y * 0.3);
    vec3 rainbow = hsv2rgb(vec3(hue, 1.0, 1.0));

    // Combine rainbow with original colors
    outColor = vec4(rainbow * texColor.rgb * vtxColor.rgb, texColor.a * vtxColor.a);
}
