// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

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

#define groesse 1.0

using namespace std;

void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}

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
	
	default:
		break;
	}
}

	int level[12][12];



// Diese Drei Matrizen global (Singleton-Muster), damit sie jederzeit modifiziert und
// an die Grafikkarte geschickt werden koennen
glm::mat4 Projection;
glm::mat4 View;
glm::mat4 Model;
GLuint programID;


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

	Model = glm::scale(Model, glm::vec3(10000.0, 0.01, 0.01));
	sendMVP();
	drawCube();
	Model = Save;


	Model = glm::scale(Model, glm::vec3(0.01, 0.01, 10000.0));
	sendMVP();
	drawCube();
	Model = Save;

	Model = glm::scale(Model, glm::vec3(0.01, 10000.0, 0.01));
	sendMVP();
	drawCube();
	Model = Save;
}

static void drawLevel(){
	glm::mat4 save = Model;
	
	
	drawCS();
	for (int i = 0; i < 12; i++){
		for (int j = 0; j < 12; j++){
			if (level[i][j] == 1){
					Model = glm::translate(Model, glm::vec3(groesse*j - 0.5, groesse*0.5, groesse*i - 0.5));
				Model = glm::scale(Model, glm::vec3(groesse*0.5, groesse*0.5, groesse*0.5));
				sendMVP();
				drawCube();
				Model = save;
				
			}
			//Model = glm::mat4(1.0f);
			cout << level[i][j];
			
		}
		cout << endl;
	}
}




void drawSeg(float h){
	glm::mat4 Save = Model;

	Model = glm::translate(Model,glm::vec3(0,h/2,0));
	Model = glm::scale(Model,glm::vec3(h/6,h/2,h/6));
	sendMVP();
	drawSphere(100,100);
	Model = Save;
}

int main(void)
{
	readLevel();

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
	


	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals; 
	bool res = loadOBJ("teapot.obj", vertices, uvs, normals);

	// Jedes Objekt eigenem VAO zuordnen, damit mehrere Objekte moeglich sind
	// VAOs sind Container fuer mehrere Buffer, die zusammen gesetzt werden sollen.
	GLuint VertexArrayIDTeapot;
	glGenVertexArrays(1, &VertexArrayIDTeapot);
	glBindVertexArray(VertexArrayIDTeapot);

	// Ein ArrayBuffer speichert Daten zu Eckpunkten (hier xyz bzw. Position)
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer); // Kennung erhalten
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer); // Daten zur Kennung definieren
	// Buffer zugreifbar für die Shader machen
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	// Erst nach glEnableVertexAttribArray kann DrawArrays auf die Daten zugreifen...
	glEnableVertexAttribArray(0); // siehe layout im vertex shader: location = 0 
	glVertexAttribPointer(0,  // location = 0 
		3,  // Datenformat vec3: 3 floats fuer xyz 
		GL_FLOAT, 
		GL_FALSE, // Fixedpoint data normalisieren ?
		0, // Eckpunkte direkt hintereinander gespeichert
		(void*) 0); // abweichender Datenanfang ? 

	GLuint normalbuffer; // Hier alles analog für Normalen in location == 2
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2); // siehe layout im vertex shader 
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	GLuint uvbuffer; // Hier alles analog für Texturkoordinaten in location == 1 (2 floats u und v!)
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1); // siehe layout im vertex shader 
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);  

	GLuint Texture = loadBMP_custom("stones.bmp");

	glm::mat4 Save = Model;
	Model = glm::translate(Model, glm::vec3(0.0, 0.0, 0.0));
	// Eventloop
	while (!glfwWindowShouldClose(window))
	{
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);	
		
		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs();
		Projection = getProjectionMatrix();
		View = getViewMatrix();
		glm::mat4 MVP = Projection * View * Model;

		// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
		//Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);

		// Camera matrix
		//View = glm::lookAt(glm::vec3(0, 0, -5), // Camera is at (0,0,-5), in World Space
		//	glm::vec3(0, 0, 0),  // and looks at the origin
		//	glm::vec3(0, 1, 0)); // Head is up (set to 0,-1,0 to look upside-down)

		/*glm::vec3 lightPos = glm::vec3(4,4,-4);
		glUniform3f(glGetUniformLocation(programID, "LightPosition_worldspace"), lightPos.x, lightPos.y, lightPos.z);*/

		// Model matrix : an identity matrix (model will be at the origin)
		Model = Save;
		//Objekt drehen
		Model = glm::rotate(Model, x, glm::vec3(1, 0, 0));
		Model = glm::rotate(Model, y, glm::vec3(0, 1, 0));
		Model = glm::rotate(Model, z, glm::vec3(0, 0, 1));

		glBindVertexArray(VertexArrayIDTeapot);

		glm::mat4 Save = Model;
		Model = glm::translate(Model, glm::vec3(10.5, 10.0, 10.0));
		Model = glm::scale(Model, glm::vec3(1.0 / 1000.0, 1.0 / 1000.0, 1.0 / 1000.0));
		sendMVP();
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		Model = Save;

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		drawLevel();

		Model = Save;
		Model = glm::translate(Model, glm::vec3(5.0, 5.0, 5.0));
		sendMVP();
		glm::vec4 lightPos = Model * glm::vec4(10.0, 10.0, 7.0, 0.0);
		glUniform3f(glGetUniformLocation(programID, "LightPosition_worldspace"), lightPos.x, lightPos.y, lightPos.z);
		//drawCS();
		cout << lightPos.x << "x" << endl;
		cout << lightPos.y << "y" << endl;
		cout << lightPos.z << "z" << endl;
		
		Model = Save;
		// Set our "myTextureSampler" sampler to user Texture Unit 0
		glUniform1i(glGetUniformLocation(programID, "myTextureSampler"), 0);

		// Swap buffers
		glfwSwapBuffers(window);

		// Poll for and process events 
		glfwPollEvents();
	} 

	glDeleteBuffers(1, &uvbuffer);
	glDeleteTextures(1, &Texture);
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &normalbuffer);
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();
	return 0;
}

