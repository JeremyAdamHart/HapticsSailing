#version 410
#define M_PI 3.1415926535897932384626433832795

// first output is mapped to the framebuffer's colour index by default
out vec4 FragmentColour;

in vec3 TessNormal;
in vec3 ModelPosition;


uniform vec3 camera_position;

vec3 lightPos = vec3(10.0, 10.0, 10.0);
float ks = 0.5;
float kd = 0.4;
float alpha = 5.0;
float ka = 0.6;
const vec3 COLOR = vec3(0.1, 0.7, 1.0);

const float WIDTH = 0.5;


float torranceSparrowLighting(vec3 normal, vec3 position, vec3 viewPosition)
{
	vec3 viewer = normalize(viewPosition - position);
	vec3 light = normalize(lightPos - position);
	vec3 h = normalize(viewer + light);

	return ks*(alpha+2.0)*(0.5/M_PI) * clamp(pow(dot(normal, h), alpha), 0.0, 1.0)
			+ kd*clamp(dot(normal, light), 0.0, 1.0) + ka;
}

void main(void)
{
    // write colour output without modification
 //   FragmentColour = vec4(TessNormal, 1.0);
 	vec3 color = torranceSparrowLighting(TessNormal, ModelPosition, camera_position)*COLOR;

 	//Draw grid lines
 	if(((ModelPosition.x/WIDTH) - floor(ModelPosition.x/WIDTH) < WIDTH/4.f) ||
 		((ModelPosition.z/WIDTH) - floor(ModelPosition.z/WIDTH) < WIDTH/4.f))
 		color = vec3(0.1, 0.1, 0.1);
// 	color = normalize(ModelPosition - camera_position);
 	FragmentColour = vec4(color, 1);
}
