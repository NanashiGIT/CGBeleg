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
// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// Initial vertical angle : none
float verticalAngle = 0.0f;
// Initial Field of View
float initialFoV = 45.0f;

float verticalAngle_LowerLimit = -1.0f;
float verticalAngle_UpperLimit = 1.3f;
float speed = 1.0f; // 3 units / second
float mouseSpeed = 0.005f;
int levelControls[12][12];

void readLevelControls(){
	string line;
	ifstream myfile("Muster.txt");
	if (myfile.is_open())
	{
		int i = 0;
		while (getline(myfile, line))
		{
			for (int j = 0; j<13; j++)
			{
				levelControls[i][j] = line[j] - '0';
				cout << levelControls[i][j];
			}
			cout << "\n";
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
	float posx = 0.0;
	float posz = 0.0;
	float distance = 4.0f;
	float distance_temp;
	float x = position2.x;
	float z = position2.z;
	float delta_x, delta_z;

	for (int i = 0; i < 12; i++){
		for (int j = 0; j < 12; j++){
			posx = groesse*j - 0.5;
			posz = groesse*i - 0.5;
			delta_x = x - posx;
			delta_z = z - posz;
			distance_temp = sqrt(delta_x*delta_x + delta_z*delta_z);
			if (distance < 1.0){
				break;
			}
			if (distance_temp < distance){
				distance = distance_temp;
				currentBlock = glm::vec2(i, j);
			}
		}
	}
	std::cout << currentBlock.x << currentBlock.y << std::endl;
	return currentBlock;
}

bool checkBoundary(glm::vec2 cBlock){
	glm::vec2 currentBlock = cBlock;
	int i = cBlock.y;
	int j = cBlock.x;
	float posx, posz, delta_x, delta_z, distance;
	
	//oben
	if (levelControls[i + 1][j] == 1){
		posx = groesse*j - 0.5;
		posz = groesse*(i-1) - 0.5;
		cout << posx << " oben " << posz << endl;
		cout << position2.z << endl << endl << endl;
		if (position2.z < posz - 1.0) {
			return false;
		}
	}

	//unten
	if (levelControls[i - 1][j] == 1){
		posx = groesse*j - 0.5;
		posz = groesse*(i + 1) - 0.5;

		cout << posx << " unten " << posz << endl;
		cout << position2.z << endl << endl << endl;
		if (position2.z > posz + 1.0) {
			return false;
		}
	}

	//rechts
	if (levelControls[i][j + 1] == 1){
		posx = groesse*(j + 1) - 0.5;
		posz = groesse*i - 0.5;

		cout << posx << " rechts " << posz << endl;
		cout << position2.x << endl << endl << endl;
		if (position2.x > posx + 1.0) {
			return false;
		}
	}

	//links
	if (levelControls[i][j - 1] == 1){
		posx = groesse*(j - 1) - 0.5;
		posz = groesse*i - 0.5;

		cout << posx << " links " << posz << endl;
		cout << position2.x << endl << endl << endl;
		if (position2.x < posx - 1.0) {
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
	
	// Right vector
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f/2.0f), 
		0,
		cos(horizontalAngle - 3.14f/2.0f)
	);
	
	// Up vector
	//glm::vec3 up = glm::cross( right, direction );
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);


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
}

glm::vec3 getPosition(){
	return position2;
}