/*
 * Program 3 base code - includes modifications to shape and initGeom in preparation to load
 * multi shape objects 
 * CPE 471 Cal Poly Z. Wood + S. Sueda + I. Dunn
 */

/***********************
 SHADER MANAGER INSTRUCTIONS
 
 HOW TO ADD A SHADER:
 1) Create a #define in ShaderManager.h that will be used to identify your shader
 2) Add an init function in ShaderManager.cpp and put your initialization code there
 - be sure to add a prototype of this function in ShaderManager.h
 3) Call your init function from initShaders in ShaderManager.cpp and save it to the
 respective location in shaderMap. See example
 
 HOW TO USE A SHADER IN THE RENDER LOOP
 1) first, call shaderManager.setCurrentShader(int name) to set the current shader
 2) To retrieve the current shader, call shaderManager.getCurrentShader()
 3) Use the return value of getCurrentShader() to render
 ***********************/

/***********************
 SPLINE INSTRUCTIONS

 1) Create a spline object, or an array of splines (for a more complex path)
 2) Initialize the splines. I did this in initGeom in this example. There are 
	two constructors for it, for order 2 and order 3 splines. The first uses
	a beginning, intermediate control point, and ending. In the case of Bezier splines, 
	the path is influenced by, but does NOT necessarily touch, the control point. 
	There is a second constructor, for order 3 splines. These have two control points. 
	Use these to create S-curves. The constructor also takes a duration of time that the 
	path should take to be completed. This is in seconds. 
 3) Call update(frametime) with the time between the frames being rendered. 
	3a) Call isDone() and switch to the next part of the path if you are using multiple 
	    paths or something like that. 
 4) Call getPosition() to get the vec3 of where the current calculated position is. 
 ***********************/

#define VPLRESOLUTION 64

#include <chrono>
#include <iostream>
#include <glad/glad.h>

#include "GLSL.h"
#include "Program.h"
#include "Shape.h"
#include "MatrixStack.h"
#include "WindowManager.h"
#include "ShaderManager.h"
#include "GLTextureWriter.h"
#include "Spline.h"
#include "Camera.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader/tiny_obj_loader.h>

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

class Application : public EventCallbacks
{

public:
	// for debugging
	bool FirstTime = true;

	WindowManager *windowManager = nullptr;

	ShaderManager *shaderManager;

	Camera camera = Camera();

	// Shape to be used (from  file) - modify to support multiple
	shared_ptr<Shape> bunny;
	vec3 bunnyBaseColor = vec3(1,0,0);
	shared_ptr<Shape> cube;
	vec3 cubeBaseColor = vec3(0,0,1);
	vec3 lightPos = vec3(0, 0, 0);
	vec3 camPos = vec3(0,0,0);
	vec3 bunnyPos = vec3(4, -6, -4);

	// Two part path
	Spline splinepath[2];

	// Contains vertex information for OpenGL
	GLuint VertexArrayID;

	// Data necessary to give our triangle to OpenGL
	GLuint VertexBufferID;

	// Set up VPL buffer object
	GLuint VPLbuffer, depthBuf;
	GLuint VPLpositions, VPLcolors;

	//example data that might be useful when trying to compute bounds on multi-shape
	vec3 gMin;

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		if (key == GLFW_KEY_Z && action == GLFW_PRESS)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		if (key == GLFW_KEY_Z && action == GLFW_RELEASE)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		if (key == GLFW_KEY_W && action == GLFW_PRESS)
		{
			camera.w = true;
		}
		if (key == GLFW_KEY_W && action == GLFW_RELEASE)
		{
			camera.w = false;
		}
		if (key == GLFW_KEY_S && action == GLFW_PRESS)
		{
			camera.s = true;
		}
		if (key == GLFW_KEY_S && action == GLFW_RELEASE)
		{
			camera.s = false;
		}
		if (key == GLFW_KEY_A && action == GLFW_PRESS)
		{
			camera.a = true;
		}
		if (key == GLFW_KEY_A && action == GLFW_RELEASE)
		{
			camera.a = false;
		}
		if (key == GLFW_KEY_D && action == GLFW_PRESS)
		{
			camera.d = true;
		}
		if (key == GLFW_KEY_D && action == GLFW_RELEASE)
		{
			camera.d = false;
		}
	}

	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;

		if (action == GLFW_PRESS)
		{
			glfwGetCursorPos(window, &posX, &posY);
			cout << "Pos X " << posX << " Pos Y " << posY << endl;
		}
	}

	void resizeCallback(GLFWwindow *window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	void init(const std::string &resourceDirectory)
	{
		GLSL::checkVersion();

		// Set background color.
		glClearColor(0, 0, 0, 1);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		// create the Instance of ShaderManager which will initialize all shaders in its constructor
		shaderManager = new ShaderManager(resourceDirectory);
	}

	void loadMultiPartObject(const std::string &resource, vector<shared_ptr<Shape>> *object)
	{
		// Initialize mesh
		// Load geometry
		// Some obj files contain material information.We'll ignore them for this assignment.
		vector<tinyobj::shape_t> TOshapes;
		vector<tinyobj::material_t> objMaterials;
		string errStr;
		shared_ptr<Shape> s;
		//load in the mesh and make the shape(s)
		bool rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr, (resource).c_str());
		if (!rc)
		{
			cerr << errStr << endl;
		}
		else
		{
			for (int i = 0; i < TOshapes.size(); i++)
			{
				s = make_shared<Shape>();
				s->createShape(TOshapes[i]);
				s->measure();
				s->init();
				object->push_back(s);
			}
		}
	}

	void drawMultiPartObject(vector<shared_ptr<Shape>> *object, shared_ptr<Program> *program)
	{
		for (int i = 0; i < object->size(); i++)
			(*object)[i]->draw(*program);
	}

	void initVPLBuffer()
	{
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);

		// choose the smaller of width/height and defined resolution to create VPL size
		width = width > VPLRESOLUTION ? VPLRESOLUTION : width;
		height = height > VPLRESOLUTION ? VPLRESOLUTION : height;

		//initialize the buffers -- from learnopengl.com
		glGenFramebuffers(1, &VPLbuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, VPLbuffer);

		// - position color buffer
		glGenTextures(1, &VPLpositions);
		glBindTexture(GL_TEXTURE_2D, VPLpositions);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, VPLpositions, 0);

		// - color buffer
		glGenTextures(1, &VPLcolors);
		glBindTexture(GL_TEXTURE_2D, VPLcolors);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, VPLcolors, 0);
	
		glGenRenderbuffers(1, &depthBuf);
		//set up depth necessary as rendering a mesh that needs depth test
		glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf);

		//more FBO set up
		GLenum DrawBuffers[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
		glDrawBuffers(2, DrawBuffers);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void initGeom(const std::string &resourceDirectory)
	{
		//EXAMPLE new set up to read one shape from one obj file - convert to read several
		// Initialize mesh
		// Load geometry
		// Some obj files contain material information.We'll ignore them for this assignment.
		vector<tinyobj::shape_t> TOshapes;
		vector<tinyobj::material_t> objMaterials;
		string errStr;
		//load in the mesh and make the shape(s)
		bool rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr, (resourceDirectory + "/models/bunny.obj").c_str());
		if (!rc)
		{
			cerr << errStr << endl;
		}
		else
		{
			bunny = make_shared<Shape>();
			bunny->createShape(TOshapes[0]);
			bunny->measure();
			bunny->init();
		}
		//read out information stored in the shape about its size - something like this...
		//then do something with that information.....
		gMin.x = bunny->min.x;
		gMin.y = bunny->min.y;

		rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr, (resourceDirectory + "/models/cube.obj").c_str());
		if (!rc)
		{
			cerr << errStr << endl;
		}
		else
		{
			cube = make_shared<Shape>();
			cube->createShape(TOshapes[0]);
			cube->measure();
			cube->init();
		}
		//read out information stored in the shape about its size - something like this...
		//then do something with that information.....
		gMin.x = cube->min.x;
		gMin.y = cube->min.y;
	}

	mat4 SetProjectionMatrix(shared_ptr<Program> curShader)
	{
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		float aspect = width / (float)height;
		mat4 Projection = perspective(radians(50.0f), aspect, 0.1f, 100.0f);
		glUniformMatrix4fv(curShader->getUniform("P"), 1, GL_FALSE, value_ptr(Projection));
		return Projection;
	}

	void SetViewMatrix(shared_ptr<Program> curShader, vec3 position, vec3 lookat)
	{
		auto View = make_shared<MatrixStack>();
		View->pushMatrix();
		View->lookAt(position, lookat, vec3(0, 1, 0));
		glUniformMatrix4fv(curShader->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));
		View->popMatrix();
	}

	void render(float frametime)
	{
		VPLpass(frametime);
		RenderPass(frametime);
	}

	void VPLpass(float frametime)
	{

		glBindFramebuffer(GL_FRAMEBUFFER, VPLbuffer);

		// choose the smaller of width/height and defined resolution to create VPL size
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		width = width > VPLRESOLUTION ? VPLRESOLUTION : width;
		height = height > VPLRESOLUTION ? VPLRESOLUTION : height;
		glViewport(0, 0, VPLRESOLUTION, VPLRESOLUTION);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderManager->setCurrentShader(VPLPROG);
		shared_ptr<Program> VPLshader = shaderManager->getCurrentShader();

		auto Model = make_shared<MatrixStack>();

		VPLshader->bind();
		// Apply perspective projection.
		SetProjectionMatrix(VPLshader);
		SetViewMatrix(VPLshader, lightPos, bunnyPos);

		glUniform3f(VPLshader->getUniform("lightPos"), lightPos.x, lightPos.y, lightPos.z);

		// draw mesh
		Model->pushMatrix();
		Model->loadIdentity();
		//"global" translate
		Model->translate(bunnyPos);
		// draw bunny
		Model->pushMatrix();
		Model->scale(vec3(2));
		glUniformMatrix4fv(VPLshader->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
		glUniform3f(VPLshader->getUniform("baseColor"), bunnyBaseColor.x, bunnyBaseColor.y, bunnyBaseColor.z);
		bunny->draw(VPLshader);
		Model->popMatrix();

		// draw cubes around the bunny
		Model->pushMatrix();
		Model->translate(vec3(0, -2, 0));
		Model->scale(vec3(4));
		glUniformMatrix4fv(VPLshader->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
		glUniform3f(VPLshader->getUniform("baseColor"), cubeBaseColor.x, cubeBaseColor.y, cubeBaseColor.z);
		cube->draw(VPLshader);
		Model->popMatrix();

		Model->pushMatrix();
		Model->translate(vec3(0, 2, -4));
		Model->scale(vec3(4));
		glUniformMatrix4fv(VPLshader->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
		glUniform3f(VPLshader->getUniform("baseColor"), cubeBaseColor.x, cubeBaseColor.y, cubeBaseColor.z);
		cube->draw(VPLshader);
		Model->popMatrix();

		Model->pushMatrix();
		Model->translate(vec3(4, 2, 0));
		Model->scale(vec3(4));
		glUniformMatrix4fv(VPLshader->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
		glUniform3f(VPLshader->getUniform("baseColor"), cubeBaseColor.x, cubeBaseColor.y, cubeBaseColor.z);
		cube->draw(VPLshader);
		Model->popMatrix();

		Model->pushMatrix();
		Model->translate(vec3(0, -2, -4));
		Model->scale(vec3(4));
		glUniformMatrix4fv(VPLshader->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
		glUniform3f(VPLshader->getUniform("baseColor"), cubeBaseColor.x, cubeBaseColor.y, cubeBaseColor.z);
		cube->draw(VPLshader);
		Model->popMatrix();

		Model->pushMatrix();
		Model->translate(vec3(4, -2, 0));
		Model->scale(vec3(4));
		glUniform3f(VPLshader->getUniform("baseColor"), cubeBaseColor.x, cubeBaseColor.y, cubeBaseColor.z);
		glUniformMatrix4fv(VPLshader->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
		cube->draw(VPLshader);
		Model->popMatrix();

		Model->popMatrix();

		VPLshader->unbind();
	}

	void RenderPass(float frametime)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);		
		
		// resize
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		glViewport(0, 0, width, height);
		int vplres = width > VPLRESOLUTION ? VPLRESOLUTION : width;
		vplres = height > VPLRESOLUTION ? VPLRESOLUTION : height;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, VPLpositions);
		glActiveTexture(GL_TEXTURE0+1);
		glBindTexture(GL_TEXTURE_2D, VPLcolors);

		shaderManager->setCurrentShader(RENDERPROG);
		shared_ptr<Program> renderShader = shaderManager->getCurrentShader();

		auto Model = make_shared<MatrixStack>();

		renderShader->bind();		
		
		// get the resolution of the vpl buffer so we know how many lights to 
		// loop through
		glUniform1i(renderShader->getUniform("VPLresolution"), vplres);
		// Apply perspective projection.
		SetProjectionMatrix(renderShader);
		SetViewMatrix(renderShader, camPos, bunnyPos);

		// draw mesh
		Model->pushMatrix();
		Model->loadIdentity();
		//"global" translate
		Model->translate(bunnyPos);
		// draw bunny
		Model->pushMatrix();
		Model->scale(vec3(2));
		glUniformMatrix4fv(renderShader->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
		glUniform3f(renderShader->getUniform("baseColor"), bunnyBaseColor.x, bunnyBaseColor.y, bunnyBaseColor.z);
		bunny->draw(renderShader);
		Model->popMatrix();

		// draw cubes around the bunny
		Model->pushMatrix();
		Model->translate(vec3(0, -2, 0));
		Model->scale(vec3(4));
		glUniformMatrix4fv(renderShader->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
		glUniform3f(renderShader->getUniform("baseColor"), cubeBaseColor.x, cubeBaseColor.y, cubeBaseColor.z);
		cube->draw(renderShader);
		Model->popMatrix();

		Model->pushMatrix();
		Model->translate(vec3(0, 2, -4));
		Model->scale(vec3(4));
		glUniformMatrix4fv(renderShader->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
		glUniform3f(renderShader->getUniform("baseColor"), cubeBaseColor.x, cubeBaseColor.y, cubeBaseColor.z);
		cube->draw(renderShader);
		Model->popMatrix();

		Model->pushMatrix();
		Model->translate(vec3(4, 2, 0));
		Model->scale(vec3(4));
		glUniformMatrix4fv(renderShader->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
		glUniform3f(renderShader->getUniform("baseColor"), cubeBaseColor.x, cubeBaseColor.y, cubeBaseColor.z);
		cube->draw(renderShader);
		Model->popMatrix();

		Model->pushMatrix();
		Model->translate(vec3(0, -2, -4));
		Model->scale(vec3(4));
		glUniformMatrix4fv(renderShader->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
		glUniform3f(renderShader->getUniform("baseColor"), cubeBaseColor.x, cubeBaseColor.y, cubeBaseColor.z);
		cube->draw(renderShader);
		Model->popMatrix();

		Model->pushMatrix();
		Model->translate(vec3(4, -2, 0));
		Model->scale(vec3(4));
		glUniformMatrix4fv(renderShader->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
		glUniform3f(renderShader->getUniform("baseColor"), cubeBaseColor.x, cubeBaseColor.y, cubeBaseColor.z);
		cube->draw(renderShader);
		Model->popMatrix();

		Model->popMatrix();

		if (FirstTime)
		{
			assert(GLTextureWriter::WriteImage(VPLbuffer, "vplBuf.png"));
			assert(GLTextureWriter::WriteImage(VPLpositions, "vplPos.png"));
			assert(GLTextureWriter::WriteImage(VPLcolors, "vplColors.png"));
			FirstTime = false;
		}

		renderShader->unbind();
	}
};

int main(int argc, char *argv[])
{
	// Where the resources are loaded from
	std::string resourceDir = "../resources";

	if (argc >= 2)
	{
		resourceDir = argv[1];
	}

	Application *application = new Application();

	// Your main will always include a similar set up to establish your window
	// and GL context, etc.

	WindowManager *windowManager = new WindowManager();
	windowManager->init(640, 480);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	// This is the code that will likely change program to program as you
	// may need to initialize or set up different data and state

	application->init(resourceDir);
	application->initGeom(resourceDir);
	application->initVPLBuffer();

	auto lastTime = chrono::high_resolution_clock::now();

	// Loop until the user closes the window.
	while (!glfwWindowShouldClose(windowManager->getHandle()))
	{

		// save current time for next frame
		auto nextLastTime = chrono::high_resolution_clock::now();

		// get time since last frame
		float deltaTime =
			chrono::duration_cast<std::chrono::microseconds>(
				chrono::high_resolution_clock::now() - lastTime)
				.count();

		// convert microseconds (weird) to seconds (less weird)
		deltaTime *= 0.000001;

		// reset lastTime so that we can calculate the deltaTime
		// on the next frame
		lastTime = nextLastTime;
		// Render scene.
		application->render(deltaTime);

		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}