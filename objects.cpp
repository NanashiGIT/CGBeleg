// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>

// Include GLEW
#include <GL/glew.h>

#include <glm/glm.hpp>


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////    DrahtWuerfel-Objekt
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GLuint VertexArrayIDWireCube = 0;

static void createWireCube()
{
	// Vertexarrays kapseln ab OpenGL3 Eckpunkte, Texturen und Normalen
	glGenVertexArrays(1, &VertexArrayIDWireCube);
	glBindVertexArray(VertexArrayIDWireCube);

	// Our vertices. Tree consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
	// A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
	static const GLfloat g_vertex_buffer_data[] = {
		-1.0f,-1.0f,-1.0f, -1.0f,-1.0f, 1.0f, 
		-1.0f, 1.0f,-1.0f, -1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f,-1.0f,  1.0f,-1.0f, 1.0f,  
		 1.0f, 1.0f,-1.0f,  1.0f, 1.0f, 1.0f, 
		-1.0f,-1.0f,-1.0f, -1.0f, 1.0f,-1.0f, 
		-1.0f,-1.0f, 1.0f, -1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f,-1.0f,  1.0f, 1.0f,-1.0f, 
		 1.0f,-1.0f, 1.0f,  1.0f, 1.0f, 1.0f,  
		-1.0f,-1.0f,-1.0f,  1.0f,-1.0f,-1.0f,  
		-1.0f,-1.0f, 1.0f,  1.0f,-1.0f, 1.0f, 
		-1.0f, 1.0f,-1.0f,  1.0f, 1.0f,-1.0f, 
		-1.0f, 1.0f, 1.0f,  1.0f, 1.0f, 1.0f
	};

	// Vertexbuffer-Daten z.B. auf Grafikkarte kopieren
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	// Erklären wie die Vertex-Daten zu benutzen sind
	glEnableVertexAttribArray(0); // Kein Disable ausführen !
	glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
	);

	glBindVertexArray(0);
}

void drawWireCube()
{
	if (!VertexArrayIDWireCube)
	{
		createWireCube();
	}

	glBindVertexArray(VertexArrayIDWireCube);
	glDrawArrays(GL_LINES, 0, 24); // 12 Linien haben 24 Punkte
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////    Wuerfel-Objekt Bunt
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GLuint VertexArrayIDSolidCube = 0;

static void createCube()
{
	GLuint vertexbuffer;
	GLuint normalbuffer;
	GLuint uvbuffer;
	
	glGenVertexArrays(1, &VertexArrayIDSolidCube);
	glBindVertexArray(VertexArrayIDSolidCube);

	// Our vertices. Tree consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
	// A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
	static const GLfloat g_vertex_buffer_data[] = {
		-1.0f,-1.0f,-1.0f, -1.0f,-1.0f, 1.0f, -1.0f, 1.0f, 1.0f,  // Left Side
		-1.0f,-1.0f,-1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f,-1.0f,  // Left Side
		 1.0f, 1.0f,-1.0f, -1.0f,-1.0f,-1.0f, -1.0f, 1.0f,-1.0f,  // Back Side
		 1.0f,-1.0f, 1.0f, -1.0f,-1.0f,-1.0f,  1.0f,-1.0f,-1.0f,  // Bottom Side
		 1.0f, 1.0f,-1.0f,  1.0f,-1.0f,-1.0f, -1.0f,-1.0f,-1.0f,  // Back Side
		 1.0f,-1.0f, 1.0f, -1.0f,-1.0f, 1.0f, -1.0f,-1.0f,-1.0f,  // Bottom Side
		-1.0f, 1.0f, 1.0f, -1.0f,-1.0f, 1.0f,  1.0f,-1.0f, 1.0f,  // Front Side
		 1.0f, 1.0f, 1.0f,  1.0f,-1.0f,-1.0f,  1.0f, 1.0f,-1.0f,  // Right Side
		 1.0f,-1.0f,-1.0f,  1.0f, 1.0f, 1.0f,  1.0f,-1.0f, 1.0f,  // Right Side
		 1.0f, 1.0f, 1.0f,  1.0f, 1.0f,-1.0f, -1.0f, 1.0f,-1.0f,  // Top Side
		 1.0f, 1.0f, 1.0f, -1.0f, 1.0f,-1.0f, -1.0f, 1.0f, 1.0f,  // Top Side
		 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f,  1.0f,-1.0f, 1.0f   // Front Side
	}; 



	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	// Two UV coordinatesfor each vertex.
	static const GLfloat g_uv_buffer_data[] = {
		0.0f, 0.0f, 1.0, 0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f
	};
	

	
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, GL_STATIC_DRAW);
	
	static const GLfloat normals[] = { 
		-1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // Left Side
		-1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // Left Side
		 0.0f, 0.0f, -1.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f, -1.0f, // Back Side
		 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f,// Bottom Side
		 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, // Back Side
		 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f,// Bottom Side
		 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,// front Side
		 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,// right Side
		 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,// right Side
		 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,// top Side
		 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,// top Side
		 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f // front Side

	};

	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), &normals[0], GL_STATIC_DRAW);


	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
		);

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		2,                                // size : U+V => 2
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
		);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glVertexAttribPointer(
		2,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
		);
	
	glBindVertexArray(0);
}

void drawCube()
{
	if (!VertexArrayIDSolidCube)
	{
		createCube();
	}

	// Draw the triangles !
	glBindVertexArray(VertexArrayIDSolidCube);
	glDrawArrays(GL_TRIANGLES, 0, 12*3); // 12*3 indices starting at 0 -> 12 triangles
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////    Kugel-Objekt
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GLuint VertexArrayIDSphere = 0;
GLuint lats;
GLuint longs;


// Dieser Code  basiert auf http://ozark.hendrix.edu/~burch/cs/490/sched/feb8/
static void createSphere()
{
	glGenVertexArrays(1, &VertexArrayIDSphere);
	glBindVertexArray(VertexArrayIDSphere);

	static GLuint sphereVertexBuffer = 0;
	static GLfloat* sphereVertexBufferData = NULL;
	static GLfloat* sphereNormalBufferData = NULL;

	sphereVertexBufferData = new GLfloat [6 * (lats + 1) * (longs + 1)];
	sphereNormalBufferData = new GLfloat [6 * (lats + 1) * (longs + 1)];
	int index = 0;

    for (int i = 0; i <= lats; i++) 
	{
          GLfloat lat0 = (GLfloat) M_PI * ((GLfloat) -0.5 + (GLfloat) (i - 1) / (GLfloat) lats);
          GLfloat z0  = sin(lat0);
          GLfloat zr0 =  cos(lat0);
    
          GLfloat lat1 = (GLfloat) M_PI * ((GLfloat) -0.5 + (GLfloat) i / (GLfloat) lats);
          GLfloat z1 = sin(lat1);
          GLfloat zr1 = cos(lat1);
    
		  for (int j = 0; j <= longs; j++)
		  {
              GLfloat lng = (GLfloat) 2 * (GLfloat) M_PI * (GLfloat) (j - 1) / (GLfloat) longs;
              GLfloat x = cos(lng);
              GLfloat y = sin(lng);

			  sphereNormalBufferData[index] = x * zr0;
			  sphereVertexBufferData[index++] = x * zr0;
			  sphereNormalBufferData[index] = y * zr0;
			  sphereVertexBufferData[index++] = y * zr0;
			  sphereNormalBufferData[index] = z0;
			  sphereVertexBufferData[index++] = z0;
			  sphereNormalBufferData[index] = x * zr1;
			  sphereVertexBufferData[index++] = x * zr1;
			  sphereNormalBufferData[index] = y * zr1;
			  sphereVertexBufferData[index++] = y * zr1;
			  sphereNormalBufferData[index] = z1;
			  sphereVertexBufferData[index++] = z1;
          }
     }

	GLuint vertexbuffer;
	GLuint normalbuffer;
	
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * (lats + 1) * (longs + 1), sphereVertexBufferData, GL_STATIC_DRAW);

	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * (lats + 1) * (longs + 1), sphereNormalBufferData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); // Kein Disable ausführen !
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
	);

	// 2nd attribute buffer : normals
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glEnableVertexAttribArray(2); // Kein Disable ausführen !
	glVertexAttribPointer(
			2,                                // attribute. No particular reason for 2, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
	);

	glBindVertexArray(0);
}

// Nur die Angabe bei der ersten Kugel ist relevant
void drawSphere(GLuint slats, GLuint slongs)
{
	if (!VertexArrayIDSphere)
	{
		lats = slats;
		longs = slongs;
		createSphere();
	}

	glBindVertexArray(VertexArrayIDSphere);
	// Draw the triangles !
	glDrawArrays(GL_QUAD_STRIP, 0, 2 * (lats + 1) * (longs + 1)); 
}

/*
* http://www.joelcontrol.com/Graphics/cylinder.cpp
*/

//one radius cylinder
void drawCylinder_1(float radius, float height, float theta)
{

	//M_PI = 3.14159265358979323846;
	float radian, r, h, t, t_normal, radian_normal;

	/* set the cylinder to be drawn using lines (filled) */
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	/* draw the upper circle */
	r = radius; h = height; t = theta;
	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0.0, 1.0, 0.0);
	glVertex3f(0.0, h, 0.0);
	glVertex3f(r, h, 0.0);
	while (t <= 360) {
		radian = M_PI * t / 180.0;
		glVertex3f(r * cos(radian), h, r * sin(radian));
		t = t + theta;
	}
	glEnd();

	/* draw the lower circle */
	r = radius; h = height; t = theta;
	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0.0, -1.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(r, 0.0, 0.0);
	while (t <= 360) {
		radian = M_PI * t / 180.0;
		glVertex3f(r * cos(radian), 0.0, r * sin(radian));
		t = t + theta;
	}
	glEnd();

	/* draw the body */
	r = radius; h = height; t = theta;
	glBegin(GL_QUAD_STRIP);

	glVertex3f(r, 0.0, 0.0);
	glVertex3f(r, h, 0.0);
	while (t <= 360) {
		t_normal = t - theta / 2;
		radian = M_PI * t / 180.0;
		radian_normal = M_PI * t_normal / 180.0;
		glNormal3f(cos(radian_normal), 0.0, sin(radian_normal));

		radian = M_PI * t / 180.0;
		glVertex3f(r * cos(radian), 0.0, r * sin(radian));
		glVertex3f(r * cos(radian), h, r * sin(radian));
		t = t + theta;
	}
	glEnd();
}

//two radius cylinder
void drawCylinder_2(float radiusBottom, float radiusTop, float height, float theta)
{

	float radian, r, h, t, t_normal, radian_normal;

	/* set the cylinder to be drawn filled */
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	/* draw the upper circle */
	r = radiusTop; h = height; t = theta;
	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0.0, 1.0, 0.0);
	glVertex3f(0.0, h, 0.0);
	glVertex3f(radiusTop, h, 0.0);
	do {
		radian = M_PI * t / 180.0;
		glVertex3f(radiusTop * cos(radian), h, radiusTop * sin(radian));
		t = t + theta;
	} while (t <= 360);
	glEnd();

	/* draw the lower circle */
	r = radiusBottom; h = height; t = theta;
	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0.0, -1.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(radiusBottom, 0.0, 0.0);
	do {
		radian = M_PI * t / 180.0;
		glVertex3f(radiusBottom * cos(radian), 0.0, radiusBottom * sin(radian));
		t = t + theta;
	} while (t <= 360);
	glEnd();

	/* draw the body */
	h = height; t = theta;
	glBegin(GL_QUAD_STRIP);
	glVertex3f(radiusBottom, 0.0, 0.0);
	glVertex3f(radiusTop, h, 0.0);
	do {
		t_normal = t - theta / 2;
		radian = M_PI * t / 180.0;
		radian_normal = M_PI * t_normal / 180.0;
		glNormal3f(cos(radian_normal), 0.0, sin(radian_normal));
		glVertex3f(radiusBottom * cos(radian), 0.0, radiusBottom * sin(radian));
		glVertex3f(radiusTop * cos(radian), h, radiusTop * sin(radian));
		t = t + theta;
	} while (t <= 360);
	glEnd();
}
