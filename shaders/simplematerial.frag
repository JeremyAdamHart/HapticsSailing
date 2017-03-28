#version 410

// first output is mapped to the framebuffer's colour index by default
out vec4 FragmentColour;

void main(void)
{
    // write colour output without modification
    FragmentColour = vec4(0.75, 0.5, 0.75, 1);
}
