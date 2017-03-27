#version 410
layout(quads) in;

void main()
{
	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;

	vec3 py0 = (1-u)*gl_in[0].gl_Position + u*gl_in[1].gl_Position;
	vec3 py1 = (1-u)*gl_in[2].gl_Position + u*gl_in[3].gl_Position;

	gl_Position = (1-v)py0 + v*py1 + vec3(0, u*v, 0);
}
