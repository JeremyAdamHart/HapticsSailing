#include "WaterPhysics.h"
#include <random>
#include <ctime>

using namespace glm;

static float rand01() { return float(rand()) / float(RAND_MAX); }

WaterPhysics::WaterPhysics(Renderer *r, unsigned int width, unsigned int height, 
	float *timeElapsed) : width(width), height(height), waterMat(&waves, timeElapsed),
	water(mat4(1.f), &waterMat, &waterGeom), r(r)
{
	srand(time(0));

	mat3 scale = toMat3(scaleMatrix(5.f));

	//Push back quad
	waterPoints.push_back(scale*vec3(-1, 0, 1));
	waterPoints.push_back(scale*vec3(1, 0, 1));
	waterPoints.push_back(scale*vec3(-1, 0, -1));
	waterPoints.push_back(scale*vec3(1, 0, -1));

	waterGeom = SimpleGeometry(waterPoints.data(), waterPoints.size(), GL_PATCHES);

	fb = createPositionFramebuffer(width, height);
	bouyancyLines.resize(width*height*2);

	//Texture copy buffers
	topObject.resize(width*height, vec3(0.f));
	bottomObject.resize(width*height, vec3(0.f));
	waterSurface.resize(width*height, vec3(0.f));

	waves = randomWaveSet(MAX_WAVE_NUMBER);
}

vector<WaveFunction> generateRandomWaves() {
	float posRange = 20.f;
	float heightRange = 0.2f;
	float wavelengthRange = 20.f;
	float speedRange = 0.3f;

	vector<WaveFunction> waves;

	srand(time(0));

	//Generate wind directions
	for (int i = 0; i < MAX_WAVE_NUMBER; i++) {
		waves.push_back(WaveFunction(
			vec2(2.f*rand01() - 1.f, 2.f*rand01() - 1.f),
			vec2(rand01()*2.f - 1.f, rand01()*2.f - 1.f)*posRange,
			rand01()*wavelengthRange + 3.0f,
			rand01()*speedRange + 0.2f,
		//		0.f));	
			rand01()*heightRange));
	}

	return waves;
}

void WaterPhysics::addForces(glm::vec3 *vertices, size_t numVertices, Drawable *dObject,
	RigidBody *pObject) {

	//Center camera
	cam.trackGeometryXZ(vertices, numVertices, dObject->model_matrix);

	vec3 clearColor(0.f, 0.f, 0.f);

	fb.useFramebuffer();
	glClearColor(clearColor.x, clearColor.y, clearColor.z, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Draw object top
	r->draw(cam, dObject);
	glBindTexture(GL_TEXTURE_2D, fb.texID);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, topObject.data());

	//Draw object bottom
	glClearDepth(0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Draw object bottom
	glDepthFunc(GL_GEQUAL);
	r->draw(cam, dObject);
	glBindTexture(GL_TEXTURE_2D, fb.texID);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, bottomObject.data());

	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Draw water surface
	glDepthFunc(GL_LEQUAL);
	water.model_matrix = translateMatrix(vec3(cam.pos.x, 0.f, cam.pos.z));
	r->draw(cam, &water);
	glBindTexture(GL_TEXTURE_2D, fb.texID);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, waterSurface.data());
	
	//Return state
	r->useDefaultFramebuffer();

	//Apply forces
	const float WATER_DENSITY = 998.2;

	float xWidth = (2.f / cam.projection[0][0]) / float(fb.width - 1);
	float zWidth = (2.f / cam.projection[1][1]) / float(fb.height - 1);

	for (int i = 0; i < fb.width*fb.height; i++)
	{
		if (topObject[i] != clearColor) {
			float depth = clamp(waterSurface[i].y - bottomObject[i].y,
				0.f, topObject[i].y - bottomObject[i].y);

			//Addforces
			if (depth >= 0.00001f)
			{
				pObject->addForce(-GRAVITY*depth*xWidth*zWidth*WATER_DENSITY, bottomObject[i]);	
				bouyancyLines[2 * i] = bottomObject[i];
				bouyancyLines[2 * i + 1] = bottomObject[i] + vec3(0, depth, 0)*1000.f;
			}
			else {
				bouyancyLines[2 * i] = vec3(0.f);
				bouyancyLines[2 * i + 1] = vec3(0.f);
			}
		}
		else {
			bouyancyLines[2 * i] = vec3(0.f);
			bouyancyLines[2 * i + 1] = vec3(0.f);
		}
	}

}