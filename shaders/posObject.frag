#version 410

// first output is mapped to the framebuffer's colour index by default
out vec3 PragmentPosition;

in vec4 ModelPosition;

void main(void)
{
    // write colour output without modification
    FragmentPosition = ModelPosition.xyz;
}
