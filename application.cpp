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

#include <random>
//------------------------------------------------------------------------------
using namespace chai3d;
using namespace std;
using namespace glm;
//------------------------------------------------------------------------------

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

// this function renders the scene
void updateGraphics(void);

// this function contains the main haptics simulation loop
void updateHaptics(void);

// this function closes the application
void close(void);

TrackballCamera* activeCamera = NULL;
bool leftMouseDown = false;
bool rightMouseDown = false;

float rand01() { return float(rand()) / float(RAND_MAX); }

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
    hapticDevice->calibrate();

    // retrieve information about the current haptic device
    cHapticDeviceInfo info = hapticDevice->getSpecifications();


    // if the device has a gripper, enable the gripper to simulate a user switch
    hapticDevice->setEnableGripperUserSwitch(true);


    //--------------------------------------------------------------------------
    // WIDGETS
    //--------------------------------------------------------------------------

    //--------------------------------------------------------------------------
    // START SIMULATION
    //--------------------------------------------------------------------------

    // create a thread which starts the main haptics rendering loop
    hapticsThread = new cThread();
    hapticsThread->start(updateHaptics, CTHREAD_PRIORITY_HAPTICS);

    // setup callback when application exits
    atexit(close);

	//--------------------------------------------------------------------------
	// SETUP
	//--------------------------------------------------------------------------

	float fov = 100.0;		//Degrees
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

	//Import cube
	MeshInfoLoader cubeMesh;
	cubeMesh.loadModel("models/cube.obj");

	float cWidth = 3.f;
	float cHeight = 1.f;
	float cDepth = 3.f;
	mat4 cScale = scaleMatrix(vec3(cWidth*0.5f, cHeight*0.5f, cDepth*0.5f));
	mat3 cScale3 = toMat3(cScale); 

	for (int i = 0; i < cubeMesh.vertices.size(); i++){
		cubeMesh.vertices[i] = cScale3*cubeMesh.vertices[i];
	}

	ElementGeometry cubeContainer(cubeMesh.vertices.data(), cubeMesh.normals.data(), 
					cubeMesh.uvs.data(), cubeMesh.indices.data(), cubeMesh.vertices.size(), cubeMesh.indices.size(), GL_TRIANGLES);
	TorranceSparrow cubeMat;
	Drawable cube(translateMatrix(vec3(0, 5.f, 0)), &cubeMat, &cubeContainer);

	float mass = 10.f;
	mat3 I = mass/12.f*mat3(vec3(cHeight*cHeight + cDepth*cDepth, 0, 0),
		vec3(0, cWidth*cWidth + cDepth*cDepth, 0),
		vec3(0, 0, cWidth*cWidth + cHeight*cHeight));
	RigidBody physicsCube(mass, I);
	physicsCube.p = vec3(0, 5.f, 0);
	physicsCube.omega = vec3(1, 1, 0);
	physicsCube.v = vec3(2, 0, 0);


	vec3 cubePoints[] = { 
		cScale3*vec3(1, 1, 1),
		cScale3*vec3(1, 1, -1),
		cScale3*vec3(1, -1, 1),
		cScale3*vec3(1, -1, -1),
		cScale3*vec3(-1, 1, 1),
		cScale3*vec3(-1, 1, -1),
		cScale3*vec3(-1, -1, 1),
		cScale3*vec3(-1, -1, -1) };
//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	srand(time(0));

	float posRange = 20.f;
	float heightRange = 0.5f;
	float wavelengthRange = 20.f;
	float speedRange = 0.3f;

	vector<WaveFunction> waves;

	//Generate wind directions
	for (int i = 0; i < MAX_WAVE_NUMBER; i++){
		waves.push_back(WaveFunction(
			vec2(2.f*rand01() - 1.f, 2.f*rand01() - 1.f),
			vec2(rand01()*2.f - 1.f, rand01()*2.f - 1.f)*posRange,
			rand01()*wavelengthRange + 3.0f,
			rand01()*speedRange + 0.2f,
		//	0.f));	
			rand01()*heightRange));
	}

	float timeElapsed = 0.f;

	//Make water
	SimpleGeometry waterGeometry(points.data(), points.size(), GL_PATCHES);
	ToonWater waterMat(&waves, &timeElapsed);
	Drawable water(scaleMatrix(20.f), &waterMat, &waterGeometry);
	//SimpleMaterial mat;


	//RENDER WATER SURFACE
	const unsigned int buffWidth = 20;
	const unsigned int buffHeight = 20;
	Framebuffer fb = createPositionFramebuffer(buffWidth, buffHeight);
	TrackerCamera shipTrackingCam;
	
	PosWater bouyWaterMat(&waves, &timeElapsed);
	Drawable bouyWater (scaleMatrix(5.f), &bouyWaterMat, &waterGeometry);

	//Cube geometry for bouyancy
	PosObject bouyCubeMat;
	Drawable bouyCube(cube.model_matrix, &bouyCubeMat, &cubeContainer);

	//Array for postions
	vec3 renderPos[buffWidth*buffHeight];

	checkGLErrors("Pre loop");

    //--------------------------------------------------------------------------
    // MAIN GRAPHIC LOOP
    //--------------------------------------------------------------------------


    // call window size callback at initialization
    windowSizeCallback(window, width, height);


	while (!glfwWindowShouldClose(window)){
		glClearColor(0.6f, 0.8f, 1.0f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (int i = 0; i < 8; i++){
			vec4 p4 = cube.model_matrix*vec4(cubePoints[i], 1);
			vec3 point = vec3(p4.x, p4.y, p4.z);
			float height = 0.f;
			for (int j = 0; j < waves.size(); j++){
				height += waves[j].f(vec2(point.x, point.z), timeElapsed);
			}

			float k = 1000;
			float diff = std::min(height - point.y, 2.f); 
			if (diff > 0)
				physicsCube.addForce(vec3(0, k*diff, 0), point);
		}

		physicsCube.force += -physicsCube.v*DAMPING_LINEAR + physicsCube.mass*GRAVITY;
		physicsCube.torque -= physicsCube.omega*DAMPING_ANGULAR;

		physicsCube.resolveForces(1.f / 60.f);
		bouyCube.model_matrix = cube.model_matrix = physicsCube.matrix();

		shipTrackingCam.trackGeometryXZ(cubePoints, 8, cube.model_matrix);

	//	ris.draw(cam, &water);
	//	ris.draw(cam, &cube);

		ris.draw(shipTrackingCam, &bouyWater);
		ris.draw(shipTrackingCam, &bouyCube);

		fb.useFramebuffer();
		glClearColor(0.0, 0.0, 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ris.draw(shipTrackingCam, &bouyWater);
		ris.draw(shipTrackingCam, &bouyCube);

		//Get values from framebuffer
		glBindTexture(GL_TEXTURE_2D, fb.texID);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, renderPos);

		ris.useDefaultFramebuffer();

		timeElapsed += 1.f / 60.f;

		checkGLErrors("Finish draw");

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

    // close window
    glfwDestroyWindow(window);

    // terminate GLFW library
    glfwTerminate();

    // exit
    return 0;
}

//------------------------------------------------------------------------------

void windowSizeCallback(GLFWwindow* a_window, int a_width, int a_height)
{
    // update window size
    width  = a_width;
    height = a_height;
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

    // delete resources
    delete hapticsThread;
    delete handler;
}

void updateHaptics(void)
{
    // simulation in now running
    simulationRunning  = true;
    simulationFinished = false;

    // main haptic simulation loop
    while(simulationRunning)
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

        // read user-switch status (button 0)
        bool button = false;
        hapticDevice->getUserSwitch(0, button);

        /////////////////////////////////////////////////////////////////////
        // COMPUTE FORCES
        /////////////////////////////////////////////////////////////////////

        cVector3d force(0, 0, 0);
        cVector3d torque(0, 0, 0);
        double gripperForce = 0.0;


        /////////////////////////////////////////////////////////////////////
        // APPLY FORCES
        /////////////////////////////////////////////////////////////////////

        // send computed force, torque, and gripper force to haptic device
        hapticDevice->setForceAndTorqueAndGripperForce(force, torque, gripperForce);

        // signal frequency counter
        freqCounterHaptics.signal(1);
    }
    
    // exit haptics thread
    simulationFinished = true;
}

//------------------------------------------------------------------------------
