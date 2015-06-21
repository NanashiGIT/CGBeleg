// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>

// Include GLEW
#include <GL/glew.h>

#include "objloader.hpp"

std::vector<glm::vec3> vertices;
std::vector<glm::vec2> uvs;
std::vector<glm::vec3> normals; 
bool res = loadOBJ("teapot.obj", vertices, uvs, normals);

// Jedes Objekt eigenem VAO zuordnen, damit mehrere Objekte moeglich sind
// VAOs sind Container fuer mehrere Buffer, die zusammen gesetzt werden sollen.
GLuint VertexArrayIDTeapot;
glGenVertexArrays(1, &VertexArrayIDTeapot);
glBindVertexArray(VertexArrayIDTeapot);


static void createTeapot()
{
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
}