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

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

// Achtung, die OpenGL-Tutorials nutzen glfw 2.7, glfw kommt mit einem ver�nderten API schon in der Version 3 

// Befindet sich bei den OpenGL-Tutorials unter "common"
#include "shader.hpp"

// Wuerfel und Kugel
#include "objects.hpp"

#include "objloader.hpp"

#include "texture.hpp"



// Erkl�ren ObenGL-Statemachine, lowlevel
// Version 1: seit 1992, elegantes API f�r die Plattformunabh�gige 3D-Programmierung 
// Version 2: seit 2002, Erg�nzung durch Shader-Programme, die auf Grafikkarte laufen
// Version 3: seit 2008, Fixedfunction-Pipeline nur noch als Compability-Extension
// Version 4: seit 2010 noch kaum unterst�tzt
// Wir setzen nun auf Version 3, schwieriger zu erlernen als Version 1
// glm-Bibliothek f�r Matrix-Operationen (fehlen in Version3), glew-Bibliothek, um API-Funktionen > 1.1 nutzen zu k�nnen
// glfw-Bibliothek, um OpenGL plattformunabh�ngig nutzen zu k�nnen (z. B. Fenster �ffnen)
#define UEBUNG1 /* feste Rotation */
// Pflichtteil: Ergebnis zeigen ...
// Kode zeilenweise erkl�ren. (Orientiert sich an http://www.opengl-tutorial.org/, Evtl. diese Tutorials dort ausf�hren, um zu vertiefen.)
// Dort wird glfw in der Version 2 genutzt, wir verwenden die aktuelle Version 3 http://www.glfw.org/
// Schwarze Linien wg. Shader. Kurz erklaeren, was die Shader machen...
// Vorgehensweise erklaeren, Jeweils alte cpp-Datei nach CGTutorial.cpp.n kopieren, damit jede Aenderung nachvollziehbar bleibt.
// (Voraussetzung fuer Support)
// Aufgabe Rotation: glm::rotate    (http://glm.g-truc.net/glm.pdf)
#define UEBUNG2 /* variable Rotation ueber R-Taste */
// Eventloop, kann man muss man aber nicht in glfw (Alternativ Glut, dort muss man)
// Aufgabe: Hinweis globale Variable, Taste...
#define UEBUNG3 /* Kamerasteuerung */
// Aufgabe drei Unabh�ngige Rotationen, wird zu erstem Teil des Pflichtteils
// Hinweis auf andere Kaperasteuerungen ->  http://www.opengl-tutorial.org
#define UEBUNG4 /* Ausgemaltes Objekt und Z-Buffer */
// OpenGL-Befehle: http://wiki.delphigl.com/index.php/Hauptseite auf Deutsch!
// GLEW http://glew.sourceforge.net/
// Wireframe vs. Solid thematisieren, Z-Buffer wird noetig, um Verdeckungsproblem zu l�sen
// http://www.opengl-tutorial.org/beginners-tutorials/tutorial-4-a-colored-cube/
///////////////////////////////////////////////////////
#define UEBUNG5 /* Einlesen eines Objekts */
// http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/
// Bessere OBJ-Loader vorhanden, basieren aber auf OpenGL1.1
// Andere Formate m�glich, z. B. 3ds, s. a. Link am Ende der Seite
// Wir nehmen Teapot, Anekdote zu Teapot:  von Newell 1975 (hat ber�hmtes Lehrbuch geschrieben) glut, 3dsmax, Toystory, Monster AG, ...
//
// Vertex Buffer Objects (VBO) (ab 1.5) enthalten alle Daten zu Oberflaechen eines Objekts (Eckpunktkoordinaten, Normalen, Texturkoordinaten -> Uebung 6)
// koennen in separatem Speicher einer Grafikkarte gehalten werden (haengt vom Modell ab)
// koennen ueber API-Aufrufe veraendert werden --> glBufferData, glMapBuffer (GL_STATIC_DRAW, um mitzuteilen, dass sich nichts aendern wird)
// Vertex Array Objects (VAO) (ab 3) kapseln mehrere VBOs eines Objects zwecks Optimierung und einfachrer Benutzung:
// Beim Zeichnen nur 2 Aufrufe: glBindVertexArray und glDrawArrays
//#define UEBUNG5TEDDY  /* Nach der Teddy-�bung fuer Uebung6 wieder ausschalten */
// Alternativ: Teddy wenn die Zeit reicht: google nach teddy chameleon, teapot.obj mit xxx.obj austauschen....
// Modellieren kann also auch einfach sein, Freies Tool Blender (open Source), Professionelle Werkzeuge Maya, 3dsmax, etc. (nicht gegenstand der LV)
#define UEBUNG6 /* Beleuchten, neuer Shader */
// Teddy-Modell hat keine Normalen, passt nicht zu Shadern, wieder zur Teekanne zurueck.
// Shader anschauen, Alter Shader "ColourFragmentShader" setzt Farbe direkt, wird interpoliert ("Gouraud-Shading")
// "TransformVertexShader" gibt Farben weiter, legt layout der Eingabe-Daten fest, verwendet MVP um Eckpunkte zu transvormieren ("Eine Matrix reicht")
// Neue Shader koennen mehr (Immer noch nicht, was die Fixed-Function-Pipeline konnte) 
// Normalen stehen senkrecht auf Oberflaechen, wichtig fuer die Beleuchtung
// Normalen muessen anders behandelt werden als Eckpunkte, deshalb Trennung von MVP in Model View Projection
// -> Shader ver�ndern in Uebung 15
#define UEBUNG7 /* Texturieren */
// Farbtexturen sind digitale Rasterbilder (andere Texturen in Kombination moeglich "Multi-Texturing" nicht Gegenstand der Uebung -> VL Textur)
// Imageloader fuer png und jpg nicht in den Bibliotheken enthalten -> SOIL
// DDS-Format kompatibel zu Texturkompression der Hardware. Wir nehmen aber BMP !
// s. a.:  http://www.opengl-tutorial.org/beginners-tutorials/tutorial-5-a-textured-cube/, Achtung glfwLoadTexture2D gibt' nicht mehr in glfw-version 3
// Granularitaet: Textur pro Objekt. kann also gewechselt werden ! --> Uebung 8
// Texturierte Teekanne ist 2. Teil des Pflichtteils 
///////////////////////////////////////////////////////
#define UEBUNG8 /* Mehrere Objekte */
// Polygone bilden Objekt, Objekte haben eigene Transformationen ("Model"-Matrix) eigene Texturen, etc.
// VAOs abwechselnd binden, MVP-Matrizen abwechselnd setzen, ggf Texturen abwechselnd binden, OpenGL verwendet jeweils positive Zahlen als
// Namen, die man sich generieren lassen muss.
// Model-Matrix fuer jedes Objekt anders, Szenen haben meist hierarchische Struktur, Multiplikationskette von der Wurzel zum Blatt
// Roboter-Beispiel, um Ketten zu lernen
#define UEBUNG9 /* Koordinatenachse */
// Notwendigkeit Koordinatensysteme anzeigen zu lassen... -> drawCS-Funktion mit drei Balkenen
// Wie erhaelt man langen duennen Balken mit bekannten Geometrien ?
// Aufgabe: Balken (leider mit M, V, P, ... als Parameter) Hinweis ginge auch als Singleton...
// Skalierungsparameter ruhig ausprobieren.
// sieht man den Balken im Bauch der Teekanne ?
#define UEBUNG10 /* Koordinatenkreuz */
// Aufgabe: Drei Balken, spaeter entsteht die Notwendigkeit Matrizen zu sichern (evtl. Mechanismus von OpenGL1 erwaehnen)
#define UEBUNG11 /* Ein Robotersegment */
// Teekanne ausblenden, Kugel zeichnen, Transformationsreihenfolge nochmal thematisieren
// Aufgabe: -Funktion mit Parameter!
#define UEBUNG12 /* Drei Robotersegmente uebereinander */
// Aufgabe: statischer Roboter, Unterschied lokales vs. globales Translate
#define UEBUNG13 /* Rotationen in den Gelenken */
// Aufgabe Roboter mit Tastensteuerung, Reihenfolge der Transformationen von oben nach unten und umgekehrt mit Stiften erl�utern
#define UEBUNG14 /* Lampe am Ende */
// Uebung 15 im StandardShading pixelshader
// Hier mal exemplarisch den Pixelshader aendern, um die Beleuchtung ansprechender zu machen
// Erwaehnen, dass man Parameter wie "MVP" kontrollieren koennte.
// Hier beginnt die Wellt der Shader-programmierung, die nicht Gegenstand der Uebung ist.
// Lampe am Ende eines steuerbaren Roboters ist dritter Teil des Pflichtteils
// (5 Punkte: 3 Rotationen, Teekanne, texturiert, Roboter, Licht am Ende) 

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
				
				Model = glm::translate(Model, glm::vec3(groesse*j, groesse, groesse*i));
				Model = glm::scale(Model, glm::vec3(groesse, groesse, groesse));
				sendMVP();
				drawCube();
				Model = save;
				
			}
			Model = glm::mat4(1.0f);
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
	GLFWwindow* window = glfwCreateWindow(1024, // Breite
		768,  // Hoehe
		"CG - Tutorial", // Ueberschrift
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
	// GLEW erm�glicht Zugriff auf OpenGL-API > 1.1
	glewExperimental = true; // Needed for core profile

	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	// Auf Keyboard-Events reagieren
	glfwSetKeyCallback(window, key_callback);

	// Dark blue background
	glClearColor(100.0f, 100.0f, 100.0f, 0.2f);

	// Create and compile our GLSL program from the shaders
	programID = LoadShaders("StandardShading.vertexshader", "StandardShading.fragmentshader");
	//programID = LoadShaders("TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader");

	// Shader auch benutzen !
	glUseProgram(programID);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);


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
	// Buffer zugreifbar f�r die Shader machen
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	// Erst nach glEnableVertexAttribArray kann DrawArrays auf die Daten zugreifen...
	glEnableVertexAttribArray(0); // siehe layout im vertex shader: location = 0 
	glVertexAttribPointer(0,  // location = 0 
		3,  // Datenformat vec3: 3 floats fuer xyz 
		GL_FLOAT, 
		GL_FALSE, // Fixedpoint data normalisieren ?
		0, // Eckpunkte direkt hintereinander gespeichert
		(void*) 0); // abweichender Datenanfang ? 

	GLuint normalbuffer; // Hier alles analog f�r Normalen in location == 2
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2); // siehe layout im vertex shader 
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	GLuint uvbuffer; // Hier alles analog f�r Texturkoordinaten in location == 1 (2 floats u und v!)
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
		
		// Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
		Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);

		// Camera matrix
		View = glm::lookAt(glm::vec3(10,20,5), // Camera is at (0,0,-5), in World Space
			glm::vec3(5,5,5),  // and looks at the origin
			glm::vec3(0,1,0)); // Head is up (set to 0,-1,0 to look upside-down)

		/*glm::vec3 lightPos = glm::vec3(4,4,-4);
		glUniform3f(glGetUniformLocation(programID, "LightPosition_worldspace"), lightPos.x, lightPos.y, lightPos.z);*/

		// Model matrix : an identity matrix (model will be at the origin)
		Model = glm::mat4(1.0f);
		//Objekt drehen
		Model = glm::rotate(Model, x, glm::vec3(1,0,0));
		Model = glm::rotate(Model, y, glm::vec3(0,1,0));
		Model = glm::rotate(Model, z, glm::vec3(0,0,1));
		
		
		glm::vec4 lightPos = Model * glm::vec4(10.0,10.0,10.0,1.0);
		glUniform3f(glGetUniformLocation(programID, "LightPosition_worldspace"), lightPos.x, lightPos.y, lightPos.z);
		//drawWireCube();	
		//drawCube();		

		
		drawLevel();
		// Bind our texture in Texture Unit 0
		

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

