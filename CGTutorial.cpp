// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>   

// Include GLEW
#include <GL/glew.h>
#include <stdlib.h>

#include <GL/glut.h>
// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;
#include "controls.hpp"

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

// Achtung, die OpenGL-Tutorials nutzen glfw 2.7, glfw kommt mit einem veränderten API schon in der Version 3 

// Befindet sich bei den OpenGL-Tutorials unter "common"
#include "shader.hpp"

// Wuerfel und Kugel
#include "objects.hpp"

#include "objloader.hpp"

#include "texture.hpp"

#define groesse 1.0f

using namespace std;

void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}

// Diese Drei Matrizen global (Singleton-Muster), damit sie jederzeit modifiziert und
// an die Grafikkarte geschickt werden koennen
glm::mat4 Projection;
glm::mat4 View;
glm::mat4 Model;
GLuint programID;
GLuint textures[4];
glm::vec3 position;
bool free_cam = 0;
glm::vec2 currentBlock;


float x = 0.0f, y = 0.0f, z=0.0f;
int a=1,b=0,c=0;

float x_1 = 0.0f, x_2 = 0.0f, x_3 = 0.0f, x_4 = 0.0f;


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	switch (key)
	{
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window, GL_TRUE);
		break;
	case GLFW_KEY_X:
		x += 5.0f;
		break;
	case GLFW_KEY_Y:
		y += 5.0f;
		break;
	case GLFW_KEY_Z:
		z += 5.0f;
		break;
	case GLFW_KEY_F1:
		free_cam = true;
		break;
	case GLFW_KEY_F2:
		free_cam = false;
		break;
	default:
		break;
	}
}

int level[12][12];

void sendMVP()
{
	// Our ModelViewProjection : multiplication of our 3 matrices
	glm::mat4 MVP = Projection * View * Model; 
	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform, konstant fuer alle Eckpunkte
	glUniformMatrix4fv(glGetUniformLocation(programID, "MVP"), 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(programID, "M"), 1, GL_FALSE, &Model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(programID, "V"), 1, GL_FALSE, &View[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(programID, "P"), 1, GL_FALSE, &Projection[0][0]);
}

static void readLevel(){
	string line;
	ifstream myfile("Muster.txt");
	if (myfile.is_open())
	{
		int i = 0;
		while (getline(myfile, line))
		{
			for (int j = 0; j<13; j++)
			{
				level[i][j] = line[j] - '0';
				cout << level[i][j];
			}
			cout << "\n";
			i++;
		}
		myfile.close();
	}

	else cout << "Unable to open file";
}

void drawCS(){
	glm::mat4 Save = Model;
	Model = glm::mat4(1.0f);
	Model = glm::scale(Model, glm::vec3(10000.0, 0.01, 0.01));
	sendMVP();
	drawCube();
	Model = Save;


	Model = glm::mat4(1.0f);
	Model = glm::scale(Model, glm::vec3(0.01, 0.01, 10000.0));
	sendMVP();
	drawCube();
	Model = Save;


	Model = glm::mat4(1.0f);
	Model = glm::scale(Model, glm::vec3(0.01, 10000.0, 0.01));
	sendMVP();
	drawCube();
	Model = Save;
}

static void drawLevel(){
	glm::mat4 save = Model;

	drawCS();

	for (int i = 1; i < 11; i++){
		for (int j = 1; j < 11; j++){
			if (level[i][j] == 1 || level[i][j] == 0){
				glBindTexture(GL_TEXTURE_2D, textures[0]);
				Model = glm::translate(Model, glm::vec3(groesse*j, (-1)*groesse*0.5, groesse*i));
				Model = glm::scale(Model, glm::vec3(groesse*0.5, groesse*0.5, groesse*0.5));
				sendMVP();
				drawCube();
				Model = save;

			}else{
				glBindTexture(GL_TEXTURE_2D, textures[level[i][j]-1]);
				Model = glm::translate(Model, glm::vec3(groesse*j, (-1)*groesse*0.5, groesse*i));
				Model = glm::scale(Model, glm::vec3(groesse*0.5, groesse*0.5, groesse*0.5));
				sendMVP();
				drawCube();
				Model = save;
			}
		}
	}


	for (int i = 0; i < 12; i++){
		for (int j = 0; j < 12; j++){
			if (level[i][j] == 1){
				Model = glm::translate(Model, glm::vec3(groesse*j, groesse*0.5, groesse*i));
				Model = glm::scale(Model, glm::vec3(groesse*0.5, groesse*0.5, groesse*0.5));
				sendMVP();
				drawCube();
				Model = save;
			}
			if (level[i][j] == 3){
				x = groesse*j;
				y = groesse*0.5;
				z = groesse*i;
			}
		}
	}

	for (int i = 1; i < 11; i++){
		for (int j = 1; j < 11; j++){
				Model = glm::translate(Model, glm::vec3(groesse*j, groesse*1.5, groesse*i));
				Model = glm::scale(Model, glm::vec3(groesse*0.5, groesse*0.5, groesse*0.5));
				sendMVP();
				drawCube();
				Model = save;
		}
	}
}

void drawSeg(glm::vec3 v1, glm::vec3 v2){
	glm::mat4 Save = Model;
	glm::mat4 viewMatrix = getViewMatrix();
	glm::mat4 inversed;
	for (int i = 0; i < 3; i++){
		for (int j = 0; j < 3; j++){
			inversed[i][j] = viewMatrix[j][i];
		}
	}
	inversed[3][0] = 0;
	inversed[3][1] = 0;
	inversed[3][2] = 0;
	inversed[3][3] = 1;
	inversed[0][3] = 0;
	inversed[1][3] = 0;
	inversed[2][3] = 0;
	glBindTexture(GL_TEXTURE_2D, textures[2]);
	Model = glm::translate(Model, v1);
	Model = Model * inversed;
	Model = glm::scale(Model, glm::vec3(0.02f, 0.1f, 0.02f));
	sendMVP();
	drawCube();
	Model = Save;
}


void triggerTrap(){
	cout << "TRAP !" << endl;
}

void triggerFinish(){
	cout << "FINISH !" << endl;
}



int main(int argc, char *argv[])
{
	readLevel();
	readLevelControls();
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		exit(EXIT_FAILURE);
	}

	// Fehler werden auf stderr ausgegeben, s. o.
	glfwSetErrorCallback(error_callback);

	// Open a window and create its OpenGL context
	// glfwWindowHint vorher aufrufen, um erforderliche Resourcen festzulegen
	window = glfwCreateWindow(1024, // Breite
		768,  // Hoehe
		"CG Beleg", // Ueberschrift
		NULL,  // windowed mode
		NULL); // shared window

	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// Make the window's context current (wird nicht automatisch gemacht)
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	// GLEW ermöglicht Zugriff auf OpenGL-API > 1.1
	glewExperimental = true; // Needed for core profile

	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	// Auf Keyboard-Events reagieren
	glfwSetKeyCallback(window, key_callback);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetCursorPos(window, 1024 / 2, 768 / 2);
	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
	// Dark blue background
	glClearColor(100.0f, 100.0f, 100.0f, 0.2f);
	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);
	// Create and compile our GLSL program from the shaders
	programID = LoadShaders("StandardShading.vertexshader", "StandardShading.fragmentshader");
	//programID = LoadShaders("TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader");

	// Shader auch benutzen !
	glUseProgram(programID);

	textures[0] = loadBMP_custom("stones.bmp");
	textures[1] = loadBMP_custom("trap.bmp");
	textures[2] = loadBMP_custom("start.bmp");
	textures[3] = loadBMP_custom("finish.bmp");
	
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(programID, "myTextureSampler"), 0);

	Model = glm::mat4(1.0f);
	glm::mat4 Save = Model;

	drawLevel();
	position = glm::vec3(x, y, z);
	setPosition(position);

	// Eventloop
	while (!glfwWindowShouldClose(window))
	{
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs(free_cam);
		Projection = getProjectionMatrix();
		View = getViewMatrix();
		glm::mat4 MVP = Projection * View * Model;

		// Model matrix : an identity matrix (model will be at the origin)
		//Objekt drehen
		/*Model = glm::rotate(Model, x, glm::vec3(1, 0, 0));
		Model = glm::rotate(Model, y, glm::vec3(0, 1, 0));
		Model = glm::rotate(Model, z, glm::vec3(0, 0, 1));
		*/
		Model = Save;
		// Bind our texture in Texture Unit 0
		drawLevel();

		drawSeg(getPositionWithDirection(), getPosition());
		//drawSeg(getPositionTest());
	
		glm::vec4 lightPos = glm::vec4(getPosition(), 1);
		glUniform3f(glGetUniformLocation(programID, "LightPosition_worldspace"), lightPos.x, lightPos.y, lightPos.z);

		Model = Save;

		currentBlock = findPosition();
		int x = currentBlock.x;
		int z = currentBlock.y;
		if (level[z][x] == 2){
			triggerTrap();
		}
		else if (level[z][x] == 4){
			triggerFinish();
		}

		// Swap buffers
		glfwSwapBuffers(window);

		// Poll for and process events 
		glfwPollEvents();
	} 

	glDeleteTextures(1, &textures[0]);
	glDeleteTextures(1, &textures[1]);
	glDeleteTextures(1, &textures[2]);
	glDeleteTextures(1, &textures[3]);
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();
	return 0;
}

