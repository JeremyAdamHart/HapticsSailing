#version 410

// first output is mapped to the framebuffer's colour index by default
out vec3 FragmentPosition;

in vec3 ModelPosition;

void main(void)
{
 	FragmentPosition = ModelPosition;
}
