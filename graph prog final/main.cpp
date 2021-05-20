#define _CRT_SECURE_NO_DEPRECATE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "camera.h"
#include "texture.h"

#define WINDOW_WIDTH  900
#define WINDOW_HEIGHT 600

#define HALF_WIDTH (WINDOW_WIDTH >> 1)     
#define HALF_HEIGHT (WINDOW_HEIGHT >> 1)

// Perspective Camera Parameters
#define FOVY 70.0f
#define NEAR_PLANE 0.01f
#define FAR_PLANE  500.0f

#define  TIMER_PERIOD  16 
#define MOUSE_SENSIVITY 4.0f

/* Global Variables for Template File */
bool upKey = false, downKey = false, rightKey = false, leftKey = false;
bool wKey = false, sKey = false, aKey = false, dKey = false, spaceKey = false;
int  winWidth = WINDOW_WIDTH, winHeight = WINDOW_HEIGHT; // current Window width and height
bool mode = true;  // F1 key to toggle drawing mode between GL_LINE and GL_FILL
bool capture = true;  // Middle Mouse button set/unset mouse capture
int mouse_x = HALF_WIDTH;
int mouse_y = HALF_HEIGHT;

using namespace glm;
typedef struct {
	float Position[3];
	float Normal[3];
	float Texture[2];
} Vertex;

// User Global Variables
GLuint PyramidVaoId; // represents mesh or VAO
Shader simple;

mat4 proj, view;
Camera cam(0, 0, 2, 0, 0, 0.1);

Texture pyramid_diff, pyramid_alpha, pyramid_spec;

bool specularEnable = true;

float angles = 0;

void createPyramid() {
	GLuint VboId;
	Vertex Vertices[] =
	{
		// Front
		{ {0.0f, 0.8f, 0.0f }, { 0.0f, 0.5f, 0.8f}, {0.5f, 0.5f}},
		{ {-0.5f, 0.0f, 0.5f }, { 0.0f, 0.5f, 0.8f}, {0.0f, 0.0f}},
		{ { 0.5f, 0.0f, 0.5f }, { 0.0f, 0.5f, 0.8f}, {1.0f, 0.0f}},
		// Back
		{ { 0.0f, 0.8f, 0.0f }, { 0.0f, 0.5f, -0.8f}, {0.5f, 0.5f}},
		{ { -0.5f, 0.0f, -0.5f }, { 0.0f, 0.5f, -0.8f}, {0.0f, 1.0f}},
		{ {  0.5f, 0.0f, -0.5f }, { 0.0f, 0.5f, -0.8f}, {1.0f, 1.0f}},
		// Right
		{ { 0.0f, 0.8f, 0.0f }, { 0.8f, 0.5f, 0.0f}, {0.5f, 0.5f}},
		{ { 0.5f, 0.0f, 0.5f },{ 0.8f, 0.5f, 0.0f}, {1.0f, 0.0f}},
		{ { 0.5f, 0.0f, -0.5f },{ 0.8f, 0.5f, 0.0f}, {1.0f, 1.0f}},
		// Left
		{ { 0.0f, 0.8f, 0.0f }, { -0.8f, 0.5f, 0.0f}, {0.5f, 0.5f}},
		{ { -0.5f, 0.0f, -0.5f }, { -0.8f, 0.5f, 0.0f}, {0.0f, 1.0f}},
		{ { -0.5f, 0.0f,  0.5f }, { -0.8f, 0.5f, 0.0f}, {0.0f, 0.0f}}
	};

	const size_t BufferSize = sizeof(Vertices);
	const size_t VertexSize = sizeof(Vertex);
	const size_t NormalOffset = sizeof(Vertices[0].Position); // 3 * sizeof(float)
	const size_t TextureOffset = sizeof(Vertices[0].Position) + sizeof(Vertices[0].Normal); // 6 * sizeof(float)

	// get an empty resource id for a Vertex Array Object
	glGenVertexArrays(1, &PyramidVaoId);
	// create VAO which is empty
	glBindVertexArray(PyramidVaoId);

	// get an empty resouce id for a Vertex Buffer Object
	glGenBuffers(1, &VboId);
	// create VBO which is empty at the beginning
	glBindBuffer(GL_ARRAY_BUFFER, VboId);
	// upload Vertices array in System Memory to VboId object in VRAM
	glBufferData(GL_ARRAY_BUFFER, BufferSize, Vertices, GL_STATIC_DRAW);

	// Specify the configuration of VBO object.
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VertexSize, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)NormalOffset);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*) TextureOffset);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
}

void Reshape(int Width, int Height)
{
	winWidth = Width;
	winHeight = Height;
	glViewport(0, 0, winWidth, winHeight);
	proj = perspective(FOVY, winWidth * 1.0f / winHeight, NEAR_PLANE, FAR_PLANE);
}

void Pyramid(mat4 model) {
	simple.uniform("model", model);
	simple.uniformTexture("material.diffuse", 0, pyramid_diff.getId());
	simple.uniformTexture("material.alpha", 1, pyramid_alpha.getId());
	simple.uniformTexture("material.specular", 2, pyramid_spec.getId());
	simple.uniform("maxOffset", 0.2f);
	simple.uniform("material.shiny", 80.0f);
	
	// Draw mesh/surface
	glBindVertexArray(PyramidVaoId); // sele
	glDrawArrays(GL_TRIANGLES, 0, 12);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	view = cam.LookAt();

	// Common shader properties (uniforms) 
	simple.use();
	//simple.uniform("drawingColor", vec3(1, 0, 0));
	simple.uniform("proj", proj);
	simple.uniform("view", view);
	// Lighting Properties
	simple.uniform("light.ambient", vec3(0.2f, 0.2f, 0.2f));
	simple.uniform("light.color", vec3(1.0f, 1.0f, 1.0f));
	simple.uniform("light.position", vec3(0.0f, 100.0f, 100.0f));
	simple.uniform("light.specular", vec3(1.0f, 1.0f, 1.0f));
	simple.uniform("cameraPos", cam.pos);
	simple.uniform("specularEnable", specularEnable);
	simple.uniform("angle", angles);

	// Draw Pyramid
	mat4 model = mat4(1.0f);
	model = translate(model, vec3(0.0f, -0.7f, 0.0f));
	model = rotate(model, radians(angles / 3), vec3(0.0f, 1.0f, 0.0f));
	Pyramid(model);
	
	
	glutSwapBuffers();
	angles++;
}


void onKeydown(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);

	switch (key) {
	case 'w':
	case 'W': wKey = true; break;
	case 's':
	case 'S': sKey = true; break;
	case 'a':
	case 'A': aKey = true; break;
	case 'd':
	case 'D': dKey = true; break;
	}
}

void onKeyup(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);

	switch (key) {
	case 'w':
	case 'W': wKey = false; break;
	case 's':
	case 'S': sKey = false; break;
	case 'a':
	case 'A': aKey = false; break;
	case 'd':
	case 'D': dKey = false; break;
	}

}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
//
void onSpecialKeydown(int key, int x, int y)
{
	// Write your codes here.
	switch (key) {
	case GLUT_KEY_UP: upKey = true;	break;
	case GLUT_KEY_DOWN: downKey = true; break;
	case GLUT_KEY_LEFT: leftKey = true; break;
	case GLUT_KEY_RIGHT: rightKey = true; break;
	}
}


//
// Special Key like GLUT_KEY_F1, F2, F3,...
//
void onSpecialKeyup(int key, int x, int y)
{
	// Write your codes here.
	switch (key) {
	case GLUT_KEY_UP: upKey = false;  break;
	case GLUT_KEY_DOWN: downKey = false; break;
	case GLUT_KEY_LEFT: leftKey = false; break;
	case GLUT_KEY_RIGHT: rightKey = false; break;
	case GLUT_KEY_F1: {
		mode = !mode;
		if (mode) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		else glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	} break;

	case GLUT_KEY_F2: specularEnable = !specularEnable; break;
	}
	// to refresh the window it calls display() function
	// glutPostRedisplay();
}

void onClick(int button, int status, int x, int y)
{
	// Write your codes here.
	if (button == GLUT_MIDDLE_BUTTON && status == GLUT_DOWN) {
		capture = !capture;
		if (capture) {
			glutSetCursor(GLUT_CURSOR_NONE);
			glutWarpPointer(HALF_WIDTH, HALF_HEIGHT);
			mouse_x = HALF_WIDTH;
			mouse_y = HALF_HEIGHT;
		}
		else {
			glutSetCursor(GLUT_CURSOR_INHERIT);
		}
	}
}


void init()
{
	GLenum GlewInitResult;

	glewExperimental = GL_TRUE;
	GlewInitResult = glewInit();

	if (GLEW_OK != GlewInitResult) {
		fprintf(stderr,"ERROR: %s\n",glewGetErrorString(GlewInitResult));
		exit(EXIT_FAILURE);
	}

	fprintf(stdout,"INFO: OpenGL Version: %s\n",glGetString(GL_VERSION));

	createPyramid();

	simple.init("shaders/vs.vert", "shaders/fs.frag");
	
	pyramid_diff.Load("textures/pyramid_diff.jpg");
	pyramid_alpha.Load("textures/pyramid_alpha.jpg");
	pyramid_spec.Load("textures/pyramid_spec.jpg");


	glutWarpPointer(HALF_WIDTH, HALF_HEIGHT);
	// hide cursor
	glutSetCursor(GLUT_CURSOR_NONE);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void onTimer(int v) {

	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
	// Write your codes here.

	
	if (wKey) {
		cam.Forward();
	}
	if (sKey) {
		cam.Backward();
	}
	if (aKey) {
		cam.Left();
	}
	if (dKey) {
		cam.Right();
	}

	if (capture) {
		cam.TurnRightLeft((HALF_WIDTH - mouse_x) / MOUSE_SENSIVITY);
		cam.TurnUpDown((HALF_HEIGHT - mouse_y) / MOUSE_SENSIVITY);
		glutWarpPointer(HALF_WIDTH, HALF_HEIGHT);
	}
	

	glEnable(GL_DEPTH_TEST);

	// to refresh the window it calls display() function
	glutPostRedisplay(); // display()

}

void onMove(int x, int y) {
	// Write your codes here.
	mouse_x = x;
	mouse_y = y;
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);

	glutInitContextVersion(4, 0);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutInitWindowSize(winWidth, winHeight);

	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE); // To specify props of Opengl Context

	glutCreateWindow("Pyramid");

	init();

	glutReshapeFunc(Reshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(onKeydown);
	glutSpecialFunc(onSpecialKeydown);

	glutKeyboardUpFunc(onKeyup);
	glutSpecialUpFunc(onSpecialKeyup);
	glutMouseFunc(onClick);
	glutPassiveMotionFunc(onMove);
	
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
	glutMainLoop();
}