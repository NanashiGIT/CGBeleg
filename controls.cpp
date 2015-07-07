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

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix(){
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix(){
	return ProjectionMatrix;
}


// Initial position : on +Z
glm::vec3 position2; 
glm::vec3 position3;
// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// Initial vertical angle : none
float verticalAngle = 0.0f;
// Initial Field of View
float initialFoV = 75.0f;

float verticalAngle_LowerLimit = -1.0f;
float verticalAngle_UpperLimit = 1.3f;
float speed = 1.0f; // 3 units / second
float mouseSpeed = 0.005f;
vector< vector<int> > levelControls;
int dimensionControls = 0;

void readLevelControls(){
	string line;
	ifstream myfile("Muster2.txt");
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

bool checkBoundary(glm::vec2 cBlock){
	glm::vec2 currentBlock = cBlock;
	int j = cBlock.x;
	int i = cBlock.y;
	//cout << j << i << endl;
	float posx, posz;
	
	//oben
	if (levelControls[i-1][j] == 1){
		posx = groesse*j;
		posz = groesse*(i-1);
		if (position2.z < posz + 0.65) {
			cout << " ERROR OBEN "<< endl;
			return false;
		}
	}

	//unten
	if (levelControls[i + 1][j] == 1){
		posx = groesse*j;
		posz = groesse*(i + 1);

		if (position2.z > posz - 0.65) {
			cout << " ERROR UNTEN " << endl;
			return false;
		}
	}

	//rechts
	if (levelControls[i][j + 1] == 1){
		posx = groesse*(j + 1);
		posz = groesse*i;

		if (position2.x > posx - 0.65) {
			return false;
		}
	}

	//links
	if (levelControls[i][j - 1] == 1){
		posx = groesse*(j - 1);
		posz = groesse*i;

		if (position2.x < posx + 0.65) {
			return false;
		}
	}
	return true;
}

void computeMatricesFromInputs(bool free_flight){
	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();
	glm::vec3 position_old = position2;
	glm::vec2 currentBlock;
	bool check = true;


	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	// Reset mouse position for next frame
	glfwSetCursorPos(window, 1024/2, 768/2);

	// Compute new orientation
	horizontalAngle += mouseSpeed * float(1024/2 - xpos );
	verticalAngle   += mouseSpeed * float( 768/2 - ypos );
	if (verticalAngle < verticalAngle_LowerLimit)
		verticalAngle = verticalAngle_LowerLimit;
	else if (verticalAngle > verticalAngle_UpperLimit)
		verticalAngle = verticalAngle_UpperLimit;

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle), 
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);
	
	glm::vec3 objectDirection(
		sin(horizontalAngle)/4,
		sin(verticalAngle)/4,
		cos(horizontalAngle)/4
		);

	// Right vector
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f/2.0f), 
		0,
		cos(horizontalAngle - 3.14f/2.0f)
	);
	
	// Up vector
	glm::vec3 up = glm::cross( right, direction );
	//glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);


	// Move forward
	if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS){
		if (free_flight)
			position2 += direction * deltaTime * speed;
		else
			position2 += (glm::vec3(1.0f, 0.0f, 1.0f)* direction) * deltaTime * speed;
	}
	// Move backward
	if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS){
		if (free_flight)
			position2 -= direction * deltaTime * speed; 
		else
			position2 -= (glm::vec3(1.0f,0.0f,1.0f)* direction) * deltaTime * speed;
	}
	// Strafe right
	if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS){
		position2 += right * deltaTime * speed;
	}
	// Strafe left
	if (glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS){
		position2 -= right * deltaTime * speed;
	}

	currentBlock = findPosition();
	check = checkBoundary(currentBlock);
	
	if (check == false){
		position2 = position_old;
	}
	//cout << position2.x << " " << position2.z << " " << levelControls[(int)currentBlock.y][(int)currentBlock.x] << endl;

	float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(FoV, 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	ViewMatrix       = glm::lookAt(
								position2,           // Camera is here
								position2+direction, // and looks here : at the same position, plus "direction"
								up                  // Head is up (set to 0,-1,0 to look upside-down)
						   );

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
	position3 = position2 + objectDirection;
		cout<<"(" << position3.x << "," << position3.z << ")/(" << position2.x << "," << position2.z <<")" <<endl;
}

glm::vec3 getPosition(){
	return position2;
}

glm::vec3 getPositionWithDirection(){
	return position3;
}