#ifndef OBJECTS_HPP
#define OBJECTS_HPP

void drawWireCube(); // Wuerfel mit Kantenlaenge 2 im Drahtmodell
void drawCube();     // Bunter Wuerfel mit Kantenlaenge 2
void drawSphere(GLuint slices, GLuint stacks); // Kugel mit radius 1 bzw. Durchmesser 2
void drawCylinder_2(float radiusBottom, float radiusTop, float height, float theta); //Cylinder with 2 different radiuses for top & bottom
void drawCylinder_1(float radius, float height, float theta); //Cylinder with the same radius for top & bottom
#endif
