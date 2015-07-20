// Include GLFW
#include <GLFW/glfw3.h>
extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" declared in tutorialXXX.cpp. This is a hack to keep the tutorials simple. Please avoid this.

#define groesse 1.0

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace glm;
#include <vector>
#include "controls.hpp"

using namespace std;

glm::mat4 ViewMatrix;					// ViewMatrix der Kamera.
glm::mat4 inversedView;					// Inverse ViewMatrix
glm::mat4 ViewMatrixObj;				// ViewMatrix der Fackel
glm::mat4 ProjectionMatrix;				// ProjektionsMatrix
int fps = 0;	

// ##### Erhalte Viewmatrix #####
glm::mat4 getViewMatrix(){
	return ViewMatrix;
}

// ##### Erhalte Projektionsmatrix #####
glm::mat4 getProjectionMatrix(){
	return ProjectionMatrix;
}

// ##### Erhalte inverse Matrix #####
glm::mat4 getInversedView(){
	return inversedView;
}

// ##### Erhalte fps #####
int getFPS(){
	return fps;
}

// Initial position : on +Z
glm::vec3 position2;								// Position des Spielers
glm::vec3 position3;								// Position der Fackel.
glm::vec3* bodyPositions = new glm::vec3[4];		// [0] = Linker Arm, [1] = Rechter Arm, [2] = Linkes Bein, [3] = Rechtes Bein
float horizontalAngle = 3.14f;
float verticalAngle = 0.0f;

float initialFoV = 75.0f;
float verticalAngle_LowerLimit = -1.0f;
float verticalAngle_UpperLimit = 1.3f;
float speed = 1.0f; 
float mouseSpeed = 0.005f;
bool check[2];
vector< vector<int> > levelControls;
int dimensionControls = 0;							// Breite/Höhe des gesamten Levels.
float fpsTime = 0;									// Aufaddierte Delta-Zeit zum berechnen der fps.
int frameCount = 0;									// Zähler für die Frames, zum berechnen der fps.

// ##### Lese das Level aus der Datei ein #####
void readLevelControls(int lvlCount){
	levelControls.clear();
	string line;
	string filename = "level" + to_string(lvlCount) + ".txt";
	ifstream myfile(filename);
	if (myfile.is_open())
	{
		int i = 0;
		while (getline(myfile, line))
		{
			vector<int> row;
			levelControls.push_back(row);
			if (i == 0){
				dimensionControls = line.size();
			}
			for (int j = 0; j< dimensionControls + 1; j++)
			{
				levelControls[i].push_back(line[j] - '0');
			}
			i++;
		}
		myfile.close();
	}

	else cout << "Unable to open file";
}

void setPosition(glm::vec3 position){
	position2 = position;
}

// ##### Finde Position des Spielers. #####
glm::vec2 findPosition(){
	glm::vec2 currentBlock;
	float bottomLeftX, bottomLeftZ, topRightX, topRightZ;
	float x = position2.x;
	float z = position2.z;
	bool inX;
	bool inZ;

	for (int i = 0; i < dimensionControls; i++){
		for (int j = 0; j < dimensionControls; j++){
			inX = false;
			inZ = false;
			bottomLeftX = groesse*j -0.5;
			bottomLeftZ = groesse*i + 0.5;
			topRightX = groesse*j + 0.5;
			topRightZ = groesse*i - 0.5;

			if (x >= bottomLeftX && x <= topRightX)
				inX = true;

			if (z <= bottomLeftZ && z >= topRightZ)
				inZ = true;

			if (inX == true && inZ == true){
				currentBlock = glm::vec2(j, i);
				return currentBlock;
			}
		}
	}
	return currentBlock;
}


// ##### Kollisionserkennung #####
int checkBoundary(glm::vec2 cBlock){
	if (cBlock.x == NULL)
		return 0;
	glm::vec2 currentBlock = cBlock;
	int j = cBlock.x;		// Position des Blockes, auf dem der Spieler sich befindet.
	int i = cBlock.y;		// Position des Blockes, auf dem der Spieler sich befindet.
	float distance = 0;
	float posx, posz;		// im Folgenden die Position der Mittelpunkte der angrenzenden Blöcke.
	
	// ##### oben #####
	if (i < 1 || j < 1 || i == dimensionControls || j == dimensionControls)
		return 0;
	if (levelControls[i-1][j] == 1){
		posx = groesse*j;
		posz = groesse*(i-1);
		if (position2.z < posz + 0.78) {
			check[1] = false;
		}
	}

	// ##### unten #####
	if (levelControls[i + 1][j] == 1){
		posx = groesse*j;
		posz = groesse*(i + 1);

		if (position2.z > posz - 0.78) {
			check[1] = false;
		}
	}

	// ##### rechts #####
	if (levelControls[i][j + 1] == 1){
		posx = groesse*(j + 1);
		posz = groesse*i;

		if (position2.x > posx - 0.78) {
			check[0] = false;
		}
	}

	// ##### links #####
	if (levelControls[i][j - 1] == 1){
		posx = groesse*(j - 1);
		posz = groesse*i;

		if (position2.x < posx + 0.78) {
			check[0] = false;
		}
	}

	// ##### obren-rechts #####
	if (levelControls[i-1][j + 1] == 1){
		posx = groesse*(j + 1);
		posz = groesse*(i-1);

		distance = sqrt((posx - position2.x)*(posx - position2.x) + (posz - position2.z)*(posz - position2.z));
		if (distance < sqrt(0.5) + 0.211) {
			check[0] = false;
			check[1] = false;
		}
	}

	// ##### unten-rechts #####
	if (levelControls[i + 1][j + 1] == 1){
		posx = groesse*(j + 1);
		posz = groesse*(i + 1);

		distance = sqrt((posx - position2.x)*(posx - position2.x) + (posz - position2.z)*(posz - position2.z));
		if (distance < sqrt(0.5) + 0.211) {
			check[0] = false;
			check[1] = false;
		}
	}

	// ##### unten-links #####
	if (levelControls[i + 1][j - 1] == 1){
		posx = groesse*(j - 1);
		posz = groesse*(i + 1);

		distance = sqrt((posx - position2.x)*(posx - position2.x) + (posz - position2.z)*(posz - position2.z));
		if (distance < sqrt(0.5) + 0.211) {
			check[0] = false;
			check[1] = false;
		}
	}

	// ##### oben-links #####
	if (levelControls[i - 1][j - 1] == 1){
		posx = groesse*(j - 1);
		posz = groesse*(i - 1);

		distance = sqrt((posx - position2.x)*(posx - position2.x) + (posz - position2.z)*(posz - position2.z));
		if (distance < sqrt(0.5) + 0.211) {
			check[0] = false;
			check[1] = false;
		}
	}

	return true;
}

void computeMatricesFromInputs(bool restartMerker){
	
	static double lastTime = glfwGetTime();
	
	glm::vec3 position_old = position2;
	glm::vec2 currentBlock;
	
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);
	fpsTime = fpsTime + deltaTime;
	frameCount++;

	if (fpsTime > 1){
		fps = frameCount / fpsTime;
		frameCount = 0;
		fpsTime = 0;
	}
	if (restartMerker == true)
		deltaTime = 0;
	cout << deltaTime << endl;
	//##### Erhalte Mausposition ##### 
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);


	// ##### Setzte Mausposition für den nächsten Frame zurück. #####
	glfwSetCursorPos(window, 1024/2, 768/2);

	// ##### Berechne neue Orientierung. #####
	horizontalAngle += mouseSpeed * float(1024/2 - xpos );
	verticalAngle   += mouseSpeed * float( 768/2 - ypos );
	if (verticalAngle < verticalAngle_LowerLimit)
		verticalAngle = verticalAngle_LowerLimit;
	else if (verticalAngle > verticalAngle_UpperLimit)
		verticalAngle = verticalAngle_UpperLimit;

	// ##### Berechnung der Spherischen Koordinaten in das Kathesische Koordinatensystem. #####
	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle), 
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);

	//##### Berechnung der Orientierungen der Gliedmaßen/der Fackel #####
	glm::vec3 objectDirection(
		sin(horizontalAngle -0.8) /4,
		sin(verticalAngle) /4,
		cos(horizontalAngle - 0.8) / 4
		);

	glm::vec3 leftArmDirection(
		sin(horizontalAngle + 1.6) / 4,
		sin(verticalAngle) / 4,
		cos(horizontalAngle + 1.6) / 4
		);

	glm::vec3 rightArmDirection(
		sin(horizontalAngle - 1.2) / 6,
		sin(verticalAngle) / 4,
		cos(horizontalAngle - 1.2) / 6
		);

	glm::vec3 leftLegDirection(
		sin(horizontalAngle + 2.3) / 4,
		sin(verticalAngle) / 4,
		cos(horizontalAngle + 2.3) / 4
		);

	glm::vec3 rightLegDirection(
		sin(horizontalAngle - 2.3) / 4,
		sin(verticalAngle) / 4,
		cos(horizontalAngle -2.3) / 4
		);

	// ##### Right vector #####
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f/2.0f), 
		0,
		cos(horizontalAngle - 3.14f/2.0f)
	);
	
	// ##### Up-Vector #####
	glm::vec3 up = glm::cross( right, direction );


	// ##### Vorwärts bewegen #####
	if (glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS){
		position2 += (glm::vec3(1.0f, 0.0f, 1.0f)* direction) * deltaTime * speed;
	}
	// ##### Rückwärs bewegen #####
	if (glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS){
		position2 -= (glm::vec3(1.0f,0.0f,1.0f)* direction) * deltaTime * speed;
	}
	// ##### Nach rechts bewegen #####
	if (glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS){
		position2 += right * deltaTime * speed;
	}
	// ##### Nach links bewegen #####
	if (glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS){
		position2 -= right * deltaTime * speed;
	}

	// ##### Derzeitigen Standort ermitteln #####
	currentBlock = findPosition();
	// ##### Kollisionserkennung #####
	checkBoundary(currentBlock);
	
	// ##### Wenn Kollision in x-Richtung #####
	if (check[0] == false){
		position2.x = position_old.x;
	}
	// ##### Wenn Kollision in y-Richtung #####
	if (check[1] == false){
		position2.z = position_old.z;
	}
	check[0] = true;
	check[1] = true;

	float FoV = initialFoV;

	ProjectionMatrix = glm::perspective(FoV, 4.0f / 3.0f, 0.1f, 100.0f);

	// ##### ViewMatrix der Kamera #####
	ViewMatrix       = glm::lookAt(
								position2,							
								position2+direction, 
								up                  
						   );


	direction.y = 0;

	// ##### ViewMatrix des Objektes #####
	ViewMatrixObj	= glm::lookAt(
								position2,								
								position2 + direction,					
								glm::vec3(0.0,1.0,0.0)                 
							);

	// ##### Berechnung der Inversen #####
	for (int i = 0; i < 3; i++){
		for (int j = 0; j < 3; j++){
			inversedView[i][j] = ViewMatrixObj[j][i];
		}
	}

	inversedView[3][0] = 0;
	inversedView[3][1] = 0;
	inversedView[3][2] = 0;
	inversedView[3][3] = 1;
	inversedView[0][3] = 0;
	inversedView[1][3] = 0;
	inversedView[2][3] = 0;


	// ##### Für den nächsten Frame wird das jetztige zum alten gesetzt. #####
	lastTime = currentTime;

	// ##### Diverse Orientierungsberechnungen für die Fackel/Gliedmaßen. #####
	objectDirection.y = 0;
	leftArmDirection.y = 0;
	rightArmDirection.y = 0;
	leftLegDirection.y = 0;
	rightLegDirection.y = 0;
	position3.x = position2.x + objectDirection.x / 1.4;
	position3.z = position2.z + objectDirection.z / 1.4;
	position3.y = (position2.y - 0.05) + objectDirection.y;
	bodyPositions[0].x = position2.x + (leftArmDirection.x / 1.4);
	bodyPositions[0].z = position2.z + (leftArmDirection.z / 1.4);
	bodyPositions[0].y = (position2.y - 0.05) + leftArmDirection.y;
	bodyPositions[1] = position2 + rightArmDirection;
	bodyPositions[1].y = (position2.y - 0.05) + rightArmDirection.y;
	bodyPositions[2].x = position2.x + (leftLegDirection.x / 2.7);
	bodyPositions[2].z = position2.z + (leftLegDirection.z / 2.7);
	bodyPositions[2].y = (position2.y - 0.5) + leftLegDirection.y;
	bodyPositions[3].x = position2.x + (rightLegDirection.x / 2.7);
	bodyPositions[3].z = position2.z + (rightLegDirection.z / 2.7);
	bodyPositions[3].y = (position2.y - 0.5) + rightLegDirection.y;
}

// ##### Erhalte Position des Spielers. #####
glm::vec3 getPosition(){
	return position2;
}

// ##### Erhalte Position der Fackel. #####
glm::vec3 getPositionWithDirection(){
	return position3;
}

// ##### Erhalte Positionen der Gliedmaßen. #####
glm::vec3* getBodyPositions(){
	return bodyPositions;
}