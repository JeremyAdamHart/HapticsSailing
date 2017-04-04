#version 410

// location indices for these attributes correspond to those specified in the
// InitializeGeometry() function of the main program
layout(location = 0) in vec3 VertexPosition;

uniform mat4 modelview_projection;
uniform mat4 modelview;

out vec4 ModelPosition;

void main()
{
    // assign vertex position without modification
    ModelPosition = modelview*vec4(VertexPosition, 1.0);
    gl_Position = modelview_projection*modelview*vec4(VertexPosition, 1.0);
}
