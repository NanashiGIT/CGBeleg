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
GLuint textures[9];
glm::vec3 position;
int levelCount = 1;
bool dead = 0;
bool finished = 0;
bool restart = 0;
bool init = false;
glm::vec2 currentBlock;
vector< vector<int> > level;
int dimension = 0;
void loop_game();
void loop_menu();
int main();
float x = 0.0f, y = 0.0f, z=0.0f;
int a=1,b=0,c=0;
int textureSelector = 0;
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
	case GLFW_KEY_ENTER:
		if (init == false){
			finished = 0;
			init = true;
			loop_game();
		}
		break;
	case GLFW_KEY_SPACE:
		if (init == true){
			dead = 0;
			finished = 0;
			loop_game();
		}
		break;
	default:
		break;
	}
}



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
	level.clear();
	string line;
	string filename = "level" + to_string(levelCount) + ".txt";
	ifstream myfile(filename);
	if (myfile.is_open())
	{
		int i = 0;
		while (getline(myfile, line))
		{
			vector<int> row;
			level.push_back(row);
			if (i == 0){
				dimension = line.size();
			}
			for (int j = 0; j < dimension + 1; j++)
			{
				level[i].push_back(line[j] - '0');
			}
			i++;
		}
		myfile.close();
	}

	else cout << "Level konnte nicht gefunden werden!";
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

	for (int i = 1; i < dimension - 1; i++){
		for (int j = 1; j < dimension - 1; j++){
			if (level[i][j] == 1 || level[i][j] == 0 || level[i][j] == 3){
				glBindTexture(GL_TEXTURE_2D, textures[0]);
				Model = glm::translate(Model, glm::vec3(groesse*j, (-1)*groesse*0.5, groesse*i));
				Model = glm::scale(Model, glm::vec3(groesse*0.5, groesse*0.5, groesse*0.5));
				sendMVP();
				drawCube();
				Model = save;

			}else if (level[i][j] == 2){
				if (textureSelector == 0){
					glBindTexture(GL_TEXTURE_2D, textures[1]);
					textureSelector = 1;
				}
				else{
					glBindTexture(GL_TEXTURE_2D, textures[4]);
					textureSelector = 0;
				}
				Model = glm::translate(Model, glm::vec3(groesse*j, (-1)*groesse*0.5, groesse*i));
				Model = glm::scale(Model, glm::vec3(groesse*0.5, groesse*0.5, groesse*0.5));
				sendMVP();
				drawCube();
				Model = save;
			}
			else{
				glBindTexture(GL_TEXTURE_2D, textures[level[i][j] - 1]);
				Model = glm::translate(Model, glm::vec3(groesse*j, (-1)*groesse*0.5, groesse*i));
				Model = glm::scale(Model, glm::vec3(groesse*0.5, groesse*0.5, groesse*0.5));
				sendMVP();
				drawCube();
				Model = save;
			}
		}
	}


	for (int i = 0; i < dimension; i++){
		for (int j = 0; j < dimension; j++){
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

	for (int i = 1; i < dimension - 1; i++){
		for (int j = 1; j < dimension - 1; j++){
			if (level[i][j] == 3)
				glBindTexture(GL_TEXTURE_2D, textures[2]);
			else
				glBindTexture(GL_TEXTURE_2D, textures[0]);
				Model = glm::translate(Model, glm::vec3(groesse*j, groesse*1.5, groesse*i));
				Model = glm::scale(Model, glm::vec3(groesse*0.5, groesse*0.5, groesse*0.5));
				sendMVP();
				drawCube();
				Model = save;
		}
	}
	textureSelector = 0;
}

void drawSeg(glm::vec3 v1){
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

	Model = glm::translate(Model, v1);
	Model = Model * inversed;
	Model = glm::scale(Model, glm::vec3(0.02f, 0.02f, 0.02f));
	sendMVP();
	drawSphere(15, 15);
	Model = Save;

	Model = glm::translate(Model, glm::vec3(v1.x, v1.y-0.12f, v1.z));
	Model = Model * inversed;
	Model = glm::scale(Model, glm::vec3(0.02f, 0.1f, 0.02f));
	glBindTexture(GL_TEXTURE_2D, textures[8]);
	sendMVP();
	drawCube();
	Model = Save;

}


void triggerTrap(){
	cout << "TRAP !" << endl;
	dead = 1;
	loop_menu();
}

void triggerFinish(){
	cout << "FINISH !" << endl;
	if (levelCount < 2){
		levelCount++;
		loop_game();
	}
	else{
		finished = 1;
		restart = 1;
		loop_menu();
	}
	
}

void loop_menu(){
	programID = LoadShaders("StandardShading.vertexshader", "StandardShading-menu.fragmentshader");

	// Shader auch benutzen !
	glUseProgram(programID);

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(programID, "myTextureSampler"), 0);

	glm::mat4 save = Model;
	glm::vec4 lightPos = glm::vec4(-4,0,0,0);
	glUniform3f(glGetUniformLocation(programID, "LightPosition_worldspace"), lightPos.x, lightPos.y, lightPos.z);
	
	while (!glfwWindowShouldClose(window)){
		Model = save;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Compute the MVP matrix from keyboard and mouse input
		View = glm::lookAt(
			glm::vec3(0.0f, 0.0f, 0.0f),           // Camera is here
			glm::vec3(1.0f, 0.0f, 0.0f), // and looks here : at the same position, plus "direction"
			glm::vec3(0.0f, 1.0f, 0.0f)          // Head is up (set to 0,-1,0 to look upside-down)
			);
		Projection = glm::perspective(75.0f, 4.0f / 3.0f, 0.1f, 100.0f);
		if (finished == 1){
			glBindTexture(GL_TEXTURE_2D, textures[7]);
			init = false;
			levelCount = 1;
			level.clear();
		}
		else if (dead == 1)
			glBindTexture(GL_TEXTURE_2D, textures[6]);
		else if (dead == 0)
			glBindTexture(GL_TEXTURE_2D, textures[5]);
		Model = glm::translate(Model, glm::vec3(0.98f, 0.0f, 0.0f));
		Model = glm::scale(Model, glm::vec3(groesse*0.5, groesse*0.5, groesse*0.5));
		sendMVP();
		drawCube();
		Model = save;
		// Swap buffers
		glfwSwapBuffers(window);

		// Poll for and process events 
		glfwPollEvents();
	}
}

void loop_game(){
	readLevel();
	readLevelControls(levelCount);
	programID = LoadShaders("StandardShading.vertexshader", "StandardShading.fragmentshader");

	// Shader auch benutzen !
	glUseProgram(programID);

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
		computeMatricesFromInputs(restart);
		if (restart == true)
			restart = false;
		Projection = getProjectionMatrix();
		View = getViewMatrix();
		glm::mat4 MVP = Projection * View * Model;

		Model = Save;
		drawLevel();

		drawSeg(getPositionWithDirection());
		//drawSeg(getPositionTest());

		glm::vec4 lightPos = glm::vec4(getPositionWithDirection(), 1);
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
}

int main()
{
	
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
	programID = LoadShaders("StandardShading.vertexshader", "StandardShading-menu.fragmentshader");

	// Shader auch benutzen !
	glUseProgram(programID);

	textures[0] = loadBMP_custom("stones.bmp");
	textures[1] = loadBMP_custom("trap_beartrap.bmp");
	textures[2] = loadBMP_custom("start.bmp");
	textures[3] = loadBMP_custom("finish.bmp");
	textures[4] = loadBMP_custom("trap_hole.bmp");
	textures[5] = loadBMP_custom("menu.bmp");
	textures[6] = loadBMP_custom("death-screen.bmp");
	textures[7] = loadBMP_custom("finish-screen.bmp");
	textures[8] = loadBMP_custom("wood.bmp");

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(programID, "myTextureSampler"), 0);

	Model = glm::mat4(1.0f);
	glm::mat4 Save = Model;
	loop_menu();

	glDeleteTextures(1, &textures[0]);
	glDeleteTextures(1, &textures[1]);
	glDeleteTextures(1, &textures[2]);
	glDeleteTextures(1, &textures[3]);
	glDeleteTextures(1, &textures[4]);
	glDeleteTextures(1, &textures[5]);
	glDeleteTextures(1, &textures[6]);

	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();
	return 0;
}

