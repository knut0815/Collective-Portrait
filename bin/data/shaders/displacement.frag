#version 330

uniform sampler2D tex0;
uniform float displacementAmount;
uniform float displacementSpeed;

out vec4 out_color;
in vec2 vs_texcoord;

void main() {
    vec2 res = textureSize(tex0, 0);
    vec2 noiseValues = texture(tex0, vs_texcoord).rg;
    vec2 displace = (noiseValues - vec2(0.5, 0.5)) / res.x;
    
    // Scale the displacement
    displace *= displacementAmount;
    float b = texture(tex0, vs_texcoord + displace).b;
    
    noiseValues = mix(noiseValues, vec2(0.5, 0.5), displacementSpeed); //try 0.01 too
    
    out_color = vec4(noiseValues.r, noiseValues.g, b, 1.0);
}