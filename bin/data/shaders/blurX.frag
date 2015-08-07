#version 150

uniform sampler2D tex0;
uniform float blurAmount;

in vec2 vs_texcoord;
out vec4 outputColor;

void main()
{
  vec4 color;

  // Convolution Kernel: http://en.wikipedia.org/wiki/Kernel_(image_processing)#Convolution
  color += 1.0 * texture(tex0, vs_texcoord + vec2(blurAmount * -4.0, 0.0));
  color += 2.0 * texture(tex0, vs_texcoord + vec2(blurAmount * -3.0, 0.0));
  color += 3.0 * texture(tex0, vs_texcoord + vec2(blurAmount * -2.0, 0.0));
  color += 4.0 * texture(tex0, vs_texcoord + vec2(blurAmount * -1.0, 0.0));

  color += 5.0 * texture(tex0, vs_texcoord + vec2(blurAmount, 0.0));

  color += 4.0 * texture(tex0, vs_texcoord + vec2(blurAmount * 1.0, 0.0));
  color += 3.0 * texture(tex0, vs_texcoord + vec2(blurAmount * 2.0, 0.0));
  color += 2.0 * texture(tex0, vs_texcoord + vec2(blurAmount * 3.0, 0.0));
  color += 1.0 * texture(tex0, vs_texcoord + vec2(blurAmount * 4.0, 0.0));

  color /= 25.0;

  outputColor = color;
}
