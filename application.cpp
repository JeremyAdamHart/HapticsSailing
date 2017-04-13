//==============================================================================
/*
    Jeremy Hart
*/
//==============================================================================

//------------------------------------------------------------------------------
#include "chai3d.h"
//------------------------------------------------------------------------------
//#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "ToonWater.h"
#include "Renderer.h"
#include "TrackballCamera.h"
#include "TrackerCamera.h"
#include "SimpleGeometry.h"
#include "SimpleMaterial.h"
#include "MeshInfoLoader.h"
#include "TorranceSparrow.h"
#include "ElementGeometry.h"
#include "glmSupport.h"
#include "Physics.h"
#include "PosTexture.h"
#include "WaterPhysics.h"
#include "MassSpring.h"
#include "Rudder.h"
#include "BoomPhysics.h"

#include <random>
#include <float.h>
//------------------------------------------------------------------------------
using namespace chai3d;
using namespace std;
using namespace glm;
//------------------------------------------------------------------------------

//unsigned int fp_control_state = _controlfp(_EM_INEXACT, _MCW_EM);

//------------------------------------------------------------------------------
// GENERAL SETTINGS
//------------------------------------------------------------------------------

// stereo Mode
/*
    C_STEREO_DISABLED:            Stereo is disabled 
    C_STEREO_ACTIVE:              Active stereo for OpenGL NVDIA QUADRO cards
    C_STEREO_PASSIVE_LEFT_RIGHT:  Passive stereo where L/R images are rendered next to each other
    C_STEREO_PASSIVE_TOP_BOTTOM:  Passive stereo where L/R images are rendered above each other
*/
cStereoMode stereoMode = C_STEREO_DISABLED;

// fullscreen mode
bool fullscreen = false;

// mirrored display
bool mirroredDisplay = false;


//------------------------------------------------------------------------------
// DECLARED VARIABLES
//------------------------------------------------------------------------------

// a world that contains all objects of the virtual environment
cWorld* world;

// a camera to render the world in the window display
cCamera* camera;

// a light source to illuminate the objects in the world
cDirectionalLight *light;

// a haptic device handler
cHapticDeviceHandler* handler;

// a pointer to the current haptic device
cGenericHapticDevicePtr hapticDevice;

cHapticDeviceHandler *bHandler;
cGenericHapticDevicePtr bHapticDevice;


// flag to indicate if the haptic simulation currently running
bool simulationRunning = false;

// flag to indicate if the haptic simulation has terminated
bool simulationFinished = false;

// a frequency counter to measure the simulation haptic rate
cFrequencyCounter freqCounterHaptics;

// haptic thread
cThread* hapticsThread;

// a handle to window display context
GLFWwindow* window = NULL;

// current width of window
int width  = 0;

// current height of window
int height = 0;

// swap interval for the display context (vertical synchronization)
int swapInterval = 1;


//------------------------------------------------------------------------------
// DECLARED FUNCTIONS
//------------------------------------------------------------------------------

// callback when the window display is resized
void windowSizeCallback(GLFWwindow* a_window, int a_width, int a_height);

// callback when an error GLFW occurs
void errorCallback(int error, const char* a_description);

// callback when a key is pressed
void keyCallback(GLFWwindow* a_window, int a_key, int a_scancode, int a_action, int a_mods);

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void mousePositionCallback(GLFWwindow *window, double xpos, double ypos);

// this function contains the main haptics simulation loop
void updateHaptics(void);

// this function closes the application
void close(void);

TrackballCamera* activeCamera = NULL;
bool leftMouseDown = false;
bool rightMouseDown = false;

float fov = 100.0;		//Degrees

//GLOBALS
RigidBody *physicsShip;
RudderPhysics *rudder;
Boom *boom;
MSSystem *sailSpring;
vector<WaveFunction> *waves;

Drawable *sail;

MeshInfoLoader shipMesh;
MeshInfoLoader shipCollisionMesh;

cFrequencyCounter graphicsRate;
cFrequencyCounter hapticsRate;

float timeElapsed = 0.f;

GLFWwindow *hapticsWindow = NULL;


static float rand01() { return float(rand()) / float(RAND_MAX); }


//==============================================================================
/*
    TEMPLATE:    application.cpp

    Description of your application.
*/
//==============================================================================

int main(int argc, char* argv[])
{
    //--------------------------------------------------------------------------
    // INITIALIZATION
    //--------------------------------------------------------------------------

    cout << endl;
    cout << "-----------------------------------" << endl;
    cout << "CHAI3D" << endl;
    cout << "-----------------------------------" << endl << endl << endl;
    cout << "Keyboard Options:" << endl << endl;
    cout << "[f] - Enable/Disable full screen mode" << endl;
    cout << "[m] - Enable/Disable vertical mirroring" << endl;
    cout << "[q] - Exit application" << endl;
    cout << endl << endl;


    //--------------------------------------------------------------------------
    // OPENGL - WINDOW DISPLAY
    //--------------------------------------------------------------------------

	Renderer ris;

	GLFWwindow *window = ris.createWindow();

	if (window == NULL)
		return -1;

	hapticsWindow = ris.createWindowChild(window, 1, 1);

    // get width and height of window
    glfwGetWindowSize(window, &width, &height);

    // set key callback
    glfwSetKeyCallback(window, keyCallback);

    // set resize callback
    glfwSetWindowSizeCallback(window, windowSizeCallback);

	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetCursorPosCallback(window, mousePositionCallback);

    // set current display context
    glfwMakeContextCurrent(window);

    // sets the swap interval for the current display context
    glfwSwapInterval(swapInterval);

/*	if (!gladLoadGL())
	{
		cout << "Failed to initialize GLAD" << endl;
		return -1;
	}*/



    //--------------------------------------------------------------------------
    // HAPTIC DEVICE
    //--------------------------------------------------------------------------

    // create a haptic device handler
    handler = new cHapticDeviceHandler();

    // get a handle to the first haptic device
    handler->getDevice(hapticDevice, 0);

    // open a connection to haptic device
    hapticDevice->open();

    // calibrate device (if necessary)
	if (hapticDevice->calibrate())
		printf("First device calibrated\n");

    // retrieve information about the current haptic device
    cHapticDeviceInfo info = hapticDevice->getSpecifications();

	printf("Device num = %d\n", handler->getNumDevices());

    // if the device has a gripper, enable the gripper to simulate a user switch
    hapticDevice->setEnableGripperUserSwitch(true);

	//Create haptic device handler for boom
	if (handler->getDevice(bHapticDevice, 1))
		printf("Second device connected\n");

	if (bHapticDevice->open())
		printf("Second device opened\n");
	if (bHapticDevice->calibrate())
		printf("Second device calibrated\n");
	bHapticDevice->setEnableGripperUserSwitch(true);


    //--------------------------------------------------------------------------
    // WIDGETS
    //--------------------------------------------------------------------------


	//--------------------------------------------------------------------------
	// SETUP
	//--------------------------------------------------------------------------

	mat4 projectionMatrix = perspectiveFov(fov, (float)width, (float)height, 0.01f, 1000.f);
	TrackballCamera cam(
		vec3(0, 0, -1),		//Direction
		vec3(0, 0, 20),		//Position
		projectionMatrix);

	activeCamera = &cam;

	vector<vec3> points;
	points.push_back(vec3(-1, 0, 1));
	points.push_back(vec3(1, 0, 1));
	points.push_back(vec3(-1, 0, -1));
	points.push_back(vec3(1, 0, -1));

	glPatchParameteri(GL_PATCH_VERTICES, 4);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	//SHIP INITIALIZATION
	TorranceSparrow shipMat;
	shipMesh = MeshInfoLoader("models/ship.obj");
	ElementGeometry shipContainer(&shipMesh, GL_TRIANGLES);
	Drawable ship(mat4(), &shipMat, &shipContainer);

	//SAIL
	boom = new Boom("models/boom.obj", "models/rope.obj", "models/boomPivot.obj", "models/ropePoint.obj");
	boom->updateModelMatrix(mat4());
	MeshInfoLoader sailMesh ("models/sail.obj");
	sailSpring = new MSSystem();
	sailSpring->initializeTriangleMassSystem(
		boom->getBoomEndpointWorld(), sailMesh.vertices[1], sailMesh.vertices[0],
		20, 10.f, 2000.f);
	ElementGeometry sailGeometry;
//	sailGeometry.mode = GL_LINES;
	TorranceSparrow sailMat;
	sailSpring->loadToGeometryContainer(&sailGeometry);
	sail = new Drawable(mat4(), &sailMat, &sailGeometry);
	rudder = new RudderPhysics("models/rudder.obj", "models/handle.obj", "models/pivotPoint.obj");

	//WATER
	vector<WaveFunction> randomWaves = generateRandomWaves();
	waves = &randomWaves;

	//Make water
	for (int i = 0; i < points.size(); i++){
		points[i] = toMat3(scaleMatrix(20.f))*points[i];
	}
	SimpleGeometry waterGeometry(points.data(), points.size(), GL_PATCHES);
	ToonWater waterMat(waves, &timeElapsed);
	Drawable water(mat4(), &waterMat, &waterGeometry);

	checkGLErrors("Pre loop");

	//--------------------------------------------------------------------------
	// START SIMULATION
	//--------------------------------------------------------------------------

	// create a thread which starts the main haptics rendering loop
	hapticsThread = new cThread();
	hapticsThread->start(updateHaptics, CTHREAD_PRIORITY_HAPTICS);

	// setup callback when application exits
	atexit(close);

	cSleepMs(1000.0);

    //--------------------------------------------------------------------------
    // MAIN GRAPHIC LOOP
    //--------------------------------------------------------------------------

    // call window size callback at initialization
    windowSizeCallback(window, width, height);

	while (!glfwWindowShouldClose(window)){
		glClearColor(0.6f, 0.8f, 1.0f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		printf("Graphics rate = %.2f, Haptics rate = %.2f\n", graphicsRate.getFrequency(), hapticsRate.getFrequency());

		//Update matrices
		water.model_matrix = translateMatrix(vec3(physicsShip->p.x, 0.f, physicsShip->p.z));
		ship.model_matrix = physicsShip->matrix();
		rudder->updateModelMatrix(physicsShip->matrix());
		boom->updateModelMatrix(physicsShip->matrix());

		cam.center = toVec3(water.model_matrix*toVec4(vec3(0.f), 1.f));

		sailSpring->loadToGeometryContainer(&sailGeometry);

		ris.useDefaultFramebuffer();
	
		//Draw scene
		ris.draw(cam, &ship);

		ris.draw(cam, sail);

		ris.draw(cam, &water);
		
		ris.draw(cam, &rudder->rudderDrawable);
		ris.draw(cam, &rudder->handleDrawable);

		ris.draw(cam, &boom->boomDrawable);
		ris.draw(cam, &boom->sheetDrawable);
		ris.draw(cam, &boom->ropeDrawable);

		graphicsRate.signal(1);

		checkGLErrors("Finish draw");

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

    // close window
    glfwDestroyWindow(window);

    // terminate GLFW library
    glfwTerminate();

	simulationRunning = false;

    // exit
    return 0;
}
//vec3 toolPos = vec3(position.y(), position.z(), position.x());

cVector3d toCVector3d(vec3 v) { return cVector3d(v.z, v.x, v.y); }
vec3 toVec3(cVector3d v) { return vec3(v.y(), v.z(), v.x()); }

//--------------------------------------------------------------------------------

vector<vec3> generateBoundingBox(const vector<vec3> &vertices) {

	vec3 vMin (1000000.f, 1000000.f, 1000000.f);
	vec3 vMax (-1000000.f, -1000000.f, -1000000.f);


	for (int i = 0; i < vertices.size(); i++) {
		vec3 v = vertices[i];
		vMin.x = (v.x < vMin.x) ? v.x : vMin.x;
		vMax.x = (v.x > vMax.x) ? v.x : vMax.x;
		vMin.y = (v.y < vMin.y) ? v.y : vMin.y;
		vMax.y = (v.y > vMax.y) ? v.y : vMax.y;
		vMin.z = (v.z < vMin.z) ? v.z : vMin.z;
		vMax.z = (v.z > vMax.z) ? v.z : vMax.z;
	}

	vector<vec3> bounding;
	
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 2; k++) {
				float x = (i == 0) ? vMin.x : vMax.x;
				float y = (j == 0) ? vMin.y : vMax.y;
				float z = (k == 0) ? vMin.z : vMax.z;

				bounding.push_back(vec3(x, y, z));
			}

		}
	}

	return bounding;
}


void updateHaptics(void)
{
	// simulation in now running
	simulationRunning = true;
	simulationFinished = false;

	//CREATE OPENGL CONTEXT
	Renderer hapticsRenderer;

	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	GLFWwindow *window = hapticsRenderer.createWindow(800, 800);

	if (window == NULL)
		printf("Initialization failed\n");

	// set current display context
	glfwMakeContextCurrent(window);

	glPatchParameteri(GL_PATCH_VERTICES, 4);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	WaterPhysics waterBouyancy(&hapticsRenderer, 20, 20, &timeElapsed);
	waterBouyancy.waves = *waves;


	float mass = 12000;
	shipCollisionMesh = MeshInfoLoader("models/shipCollision.obj");
	physicsShip = new RigidBody(mass, calculateInertialTensor(&shipCollisionMesh, mass));
	physicsShip->p = vec3(0, 0.f, 0);

	vector<vec3> boundingShip = generateBoundingBox(shipCollisionMesh.vertices);

	ElementGeometry shipCollisionContainer(&shipCollisionMesh, GL_TRIANGLES);
	PosObject buoyMat;
	Drawable buoyShip(physicsShip->matrix(), &buoyMat, &shipCollisionContainer);

	cPrecisionClock timer;
	timer.start();

	vec3 prevForceA(0.f);
	vec3 prevForceB(0.f);

	const float MAX_FORCE_DIFF = 5.f;

	// main haptic simulation loop
	while (simulationRunning)
	{
		/////////////////////////////////////////////////////////////////////
		// READ HAPTIC DEVICE
		/////////////////////////////////////////////////////////////////////

		// read position 
		cVector3d position;
		hapticDevice->getPosition(position);

		// read orientation 
		cMatrix3d rotation;
		hapticDevice->getRotation(rotation);

		cVector3d bPosition;
		bHapticDevice->getPosition(bPosition);

		// read user-switch status (button 0)
		bool button = false;
		hapticDevice->getUserSwitch(0, button);

		vec3 toolPos = toVec3(position);	// vec3(position.y(), position.z(), position.x());
		vec3 bToolPos = toVec3(bPosition);	// vec3(bPosition.y(), bPosition.z(), bPosition.x());

		//////////////////////////////////////////////////////////////////////
		// RUN SIMULATION
		/////////////////////////////////////////////////////////////////////

		double frameTime = timer.getCurrentTimeSeconds();
//		frameTime = 0.0;
		timer.reset();

		sailSpring->transformFixedPoints(physicsShip->matrix());

		vec3 boomForce = boom->updateHandleAndGetForce(bToolPos, 0.05);
		vec3 sailForceOnBoom = sailSpring->getBoomForce();
		boom->addForceToBoom(toVec3(inverse(physicsShip->matrix())*vec4(sailForceOnBoom, 1.f)));
		boom->calculateBoomPosition(frameTime);
		sailSpring->setBoomEndPoint(boom->getBoomEndpointWorld());

		//Apply boom force
		float boomForceDiff = length(boomForce - prevForceB);
		if (boomForceDiff > MAX_FORCE_DIFF)
			boomForce = prevForceB + normalize(boomForce - prevForceB)*MAX_FORCE_DIFF;
		bHapticDevice->setForce(toCVector3d(boomForce));


		rudder->calculateRudderDirection(toolPos, 0.05);

		rudder->applyForce(physicsShip);

		sailSpring->applyWindForce(sail->model_matrix, vec3(0.f, 0.f, -20.f));
		sailSpring->solve(std::min(float(frameTime), 0.002f));
		sailSpring->calculateNormals();

		sailSpring->applyForcesToRigidBody(physicsShip);

		waterBouyancy.addForces(boundingShip.data(), boundingShip.size(),
			&buoyShip, physicsShip);

		physicsShip->addGravityForces();
		physicsShip->addDampingForces();

		physicsShip->resolveForces(std::min(float(frameTime), 0.002f));
		buoyShip.model_matrix = physicsShip->matrix();

		timeElapsed += float(std::min(float(frameTime), 0.002f));

		/////////////////////////////////////////////////////////////////////
		// COMPUTE FORCES
		/////////////////////////////////////////////////////////////////////

		cVector3d force(0, 0, 0);
		cVector3d torque(0, 0, 0);
		double gripperForce = 0.0;

//		boomForce = vec3(0.f);

		/////////////////////////////////////////////////////////////////////
		// APPLY FORCES
		/////////////////////////////////////////////////////////////////////

		// send computed force, torque, and gripper force to haptic device
		hapticDevice->setForce(force);

		// signal frequency counter
		hapticsRate.signal(1);
	}

	// exit haptics thread
	simulationFinished = true;

	glfwDestroyWindow(window);

}



//------------------------------------------------------------------------------

void windowSizeCallback(GLFWwindow* a_window, int a_width, int a_height)
{
    // update window size
    width  = a_width;
    height = a_height;


	activeCamera->projection = perspectiveFov(fov, (float)width, (float)height, 0.01f, 1000.f);
}

//------------------------------------------------------------------------------

void errorCallback(int a_error, const char* a_description)
{
    cout << "Error: " << a_description << endl;
}

//------------------------------------------------------------------------------

void keyCallback(GLFWwindow* a_window, int a_key, int a_scancode, int a_action, int a_mods)
{
    // filter calls that only include a key press
    if (a_action != GLFW_PRESS)
    {
        return;
    }

    // option - exit
    else if ((a_key == GLFW_KEY_ESCAPE) || (a_key == GLFW_KEY_Q))
    {
        glfwSetWindowShouldClose(a_window, GLFW_TRUE);
    }

    // option - toggle fullscreen
    else if (a_key == GLFW_KEY_F)
    {
        // toggle state variable
        fullscreen = !fullscreen;

        // get handle to monitor
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();

        // get information about monitor
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        // set fullscreen or window mode
        if (fullscreen)
        {
            glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
            glfwSwapInterval(swapInterval);
        }
        else
        {
            int w = 0.8 * mode->height;
            int h = 0.5 * mode->height;
            int x = 0.5 * (mode->width - w);
            int y = 0.5 * (mode->height - h);
            glfwSetWindowMonitor(window, NULL, x, y, w, h, mode->refreshRate);
            glfwSwapInterval(swapInterval);
        }
    }

    // option - toggle vertical mirroring
    else if (a_key == GLFW_KEY_M)
    {
        mirroredDisplay = !mirroredDisplay;
        camera->setMirrorVertical(mirroredDisplay);
    }
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT){
		if (action == GLFW_RELEASE)
			leftMouseDown = false;
		else
			leftMouseDown = true;
		
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		if (action == GLFW_RELEASE)
			rightMouseDown = false;
		else
			rightMouseDown = true;

	}

}

void mousePositionCallback(GLFWwindow *window, double xpos, double ypos)
{
	static vec2 mousePos;

	vec2 newPos = vec2(xpos / double(width), -ypos / double(height))*2.f - vec2(1, -1);

	vec2 diff = newPos - mousePos;

	if (leftMouseDown && activeCamera != NULL){
		activeCamera->trackballRight(diff.x);
		activeCamera->trackballUp(diff.y);
	}

	if (rightMouseDown && activeCamera != NULL) {
		float zoomFactor = 0.5f*pow(2.f, diff.y + 1.f);
		activeCamera->zoom(zoomFactor);
	}

	mousePos = newPos;
}

//------------------------------------------------------------------------------

void close(void)
{
    // stop the simulation
    simulationRunning = false;

    // wait for graphics and haptics loops to terminate
    while (!simulationFinished) { cSleepMs(100); }

    // close haptic device
    hapticDevice->close();

	delete sail;
	delete rudder;
	delete sailSpring;
	delete physicsShip;
	delete boom;

    // delete resources
    delete hapticsThread;
    delete handler;
}


//------------------------------------------------------------------------------
