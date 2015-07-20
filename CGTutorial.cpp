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

#include "shader.hpp"
#include "objects.hpp"
#include "objloader.hpp"
#include "texture.hpp"
#define groesse 1.0f

using namespace std;

void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}


glm::mat4 Projection;
glm::mat4 View;
glm::mat4 Model;
GLuint menuID;						// ID der Shader für das Menü
GLuint gameID;						// ID der Shader für das Spiel.
GLuint textures[11];				// Array für alle Texturen
glm::vec3 position;					// Position des Spielers
int levelCount = 1;					// Das aktuelle Level
int frameCounter = 0;				// Framezähler für die Animationen
bool dead = 0;						// Merker, ob man gestorben ist
bool finished = 0;					// Merker, ob man das letzte Level geschafft hat.
bool restart = 0;					//merker, ob das Level oder Spiel neugestartet wird. Wichtig für das Zurücksetzen der DeltaTime in controls.
bool init = false;					//merker, der eigendlich nur entscheidet, ob ENTER gedrückt werden kann oder nicht.
glm::vec2 currentBlock;				//Block, auf dem wir uns befinden.
vector< vector<int> > level;		//Vektor mit dem Level, welches wir auslesen.
int dimension = 0;					//Die Breite/Höhe des gesamten Levels.
void loop_game();
void loop_menu();
void loadTextures();		
void animateBody();
int main();
float x = 0.0f, y = 0.0f, z=0.0f;	//Koordinaten für die Position des Spielers.
int textureSelector = 0;			//Entscheidet, welche Textur für eine Falle benutzt werden soll.


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
	case GLFW_KEY_W:
		animateBody();
		break;
//##### Genutzt um das Spiel beim letzten Finish neustarten zu können. #####
	case GLFW_KEY_ENTER:
		if (init == false){
			finished = 0;
			init = true;
			loop_game();
		}
		break;
//##### Genutzt um das Level neustarten zu können. #####
	case GLFW_KEY_SPACE:
		if (init == true){
			glDeleteTextures(1, &textures[0]);
			glDeleteTextures(1, &textures[1]);
			glDeleteTextures(1, &textures[2]);
			glDeleteTextures(1, &textures[3]);
			glDeleteTextures(1, &textures[4]);
			glDeleteTextures(1, &textures[5]);
			glDeleteTextures(1, &textures[6]);
			glDeleteTextures(1, &textures[7]);
			glDeleteTextures(1, &textures[8]);
			glDeleteTextures(1, &textures[9]);
			glDeleteTextures(1, &textures[10]);
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
	glm::mat4 MVP = Projection * View * Model; 
	glUniformMatrix4fv(glGetUniformLocation(gameID, "MVP"), 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(gameID, "M"), 1, GL_FALSE, &Model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(gameID, "V"), 1, GL_FALSE, &View[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(gameID, "P"), 1, GL_FALSE, &Projection[0][0]);

	glUniformMatrix4fv(glGetUniformLocation(menuID, "MVP"), 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(menuID, "M"), 1, GL_FALSE, &Model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(menuID, "V"), 1, GL_FALSE, &View[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(menuID, "P"), 1, GL_FALSE, &Projection[0][0]);
}

//##### Liest das Level aus einer Textdatei ein. #####
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

//##### Zeichnet das Level. #####
static void drawLevel(){
	glm::mat4 save = Model;

	//##### Zeichnen des Bodens #####
	for (int i = 1; i < dimension - 1; i++){
		for (int j = 1; j < dimension - 1; j++){
			//##### Falls Block eine Wand, normaler Boden, Ende oder der Start ist benutze Boden-Textur #####
			if (level[i][j] == 1 || level[i][j] == 0 || level[i][j] == 3 || level[i][j] == 4){
				glBindTexture(GL_TEXTURE_2D, textures[10]);
				Model = glm::translate(Model, glm::vec3(groesse*j, (-1)*groesse*0.5, groesse*i));
				Model = glm::scale(Model, glm::vec3(groesse*0.5, groesse*0.5, groesse*0.5));
				sendMVP();
				drawCube();
				Model = save;
			//##### Falls Block eine Falle ist, nutze eine der Falle-Texturen #####
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
		}
	}

	//##### Zeichnen der Wände #####
	for (int i = 0; i < dimension; i++){
		for (int j = 0; j < dimension; j++){
			//##### Falls Block eine Wand ist, nutze die Wand-Textur #####
			if (level[i][j] == 1){
				glBindTexture(GL_TEXTURE_2D, textures[0]);
				Model = glm::translate(Model, glm::vec3(groesse*j, groesse*0.5, groesse*i));
				Model = glm::scale(Model, glm::vec3(groesse*0.5, groesse*0.5, groesse*0.5));
				sendMVP();
				drawCube();
				Model = save;
			//##### Falls Block das Ziel ist, nutze Ziel-Textur #####
			}else if (level[i][j] == 4){
				glBindTexture(GL_TEXTURE_2D, textures[3]);
				Model = glm::translate(Model, glm::vec3(groesse*j, groesse*0.5, groesse*i));
				Model = glm::scale(Model, glm::vec3(groesse*0.5, groesse*0.5, groesse*0.5));
				sendMVP();
				drawCube();
				Model = save;
			}
			//##### falls Block Start ist, setzte die Spielerposition auf die Koordinaten. #####
			if (level[i][j] == 3){
				x = groesse*j;
				y = groesse*0.5;
				z = groesse*i;
			}
		}
	}

	//##### Zeichnen der Decke #####
	for (int i = 1; i < dimension - 1; i++){
		for (int j = 1; j < dimension - 1; j++){
			//##### Falls Block der Start ist, nutze Start-Textur #####
			if (level[i][j] == 3)
				glBindTexture(GL_TEXTURE_2D, textures[2]);
			//##### Ansonsten Standart-Textur #####
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
//##### Zeichnen des rechten Arms und der Fackel #####
void drawSeg(glm::vec3 v1){
	glm::mat4 Save = Model;
	glm::mat4 inversed = getInversedView();
	glm::vec3* bodyPositions = getBodyPositions();
	glm::vec3 rightArm = bodyPositions[1];

	Model = glm::translate(Model, v1);
	Model = Model * inversed;
	Model = glm::scale(Model, glm::vec3(0.02f, 0.02f, 0.02f));
	sendMVP();
	drawSphere(50, 50);
	Model = Save;


	Model = glm::translate(Model, glm::vec3(v1.x, v1.y - 0.12f, v1.z));
	Model = Model * inversed;
	Model = glm::scale(Model, glm::vec3(0.01f, 0.1f, 0.01f));
	glBindTexture(GL_TEXTURE_2D, textures[8]);
	sendMVP();
	drawCube();
	Model = Save;

	
	Model = glm::translate(Model, glm::vec3(rightArm.x, rightArm.y - 0.12f, rightArm.z));
	Model = Model * inversed;
	Model = glm::scale(Model, glm::vec3(0.02f, 0.02f, 0.08f));
	glBindTexture(GL_TEXTURE_2D, textures[9]);
	sendMVP();
	drawLimbCube();
	Model = Save;

}

//##### Zeichnen und Animieren der beiden Beine und des linken Arms #####
void animateBody(){
	glm::mat4 Save = Model;
	glm::mat4 inversed = getInversedView();
	glm::vec3* bodyPositions = getBodyPositions();
	glm::vec3 leftArm = bodyPositions[0];
	glm::vec3 leftLeg = bodyPositions[2];
	glm::vec3 rightLeg = bodyPositions[3];
	float i = -0.12;
	int fps = getFPS();
	// Animation der Gliedmaßen in 1/8tel Schritten.
	if (frameCounter <= fps*0.125){
		Model = glm::translate(Model, glm::vec3(leftLeg.x, leftLeg.y + 0.2, leftLeg.z));
		Model = Model * inversed;
		Model = glm::scale(Model, glm::vec3(0.03f, 0.15f, 0.03f));
		glBindTexture(GL_TEXTURE_2D, textures[9]);
		sendMVP();
		drawLimbCube();
		Model = Save;

		Model = glm::translate(Model, glm::vec3(rightLeg.x, rightLeg.y +0.2, rightLeg.z));
		Model = Model * inversed;
		Model = glm::scale(Model, glm::vec3(0.03f, 0.15f, 0.03f));
		glBindTexture(GL_TEXTURE_2D, textures[9]);
		sendMVP();
		drawLimbCube();
		Model = Save;

		Model = glm::translate(Model, glm::vec3(leftArm.x, leftArm.y + i, leftArm.z));
		Model = Model * inversed;
		Model = glm::scale(Model, glm::vec3(0.02f, 0.08f, 0.02f));
		glBindTexture(GL_TEXTURE_2D, textures[9]);
		sendMVP();
		drawLimbCube();
		Model = Save;
	}
	else if (frameCounter <= fps*0.25){
		Model = glm::translate(Model, glm::vec3(leftLeg.x, leftLeg.y + 0.2, leftLeg.z));
		Model = Model * inversed;
		Model = glm::rotate(Model, 22.5f, glm::vec3(1, 0, 0));
		Model = glm::scale(Model, glm::vec3(0.03f, 0.15f, 0.03f));
		glBindTexture(GL_TEXTURE_2D, textures[9]);
		sendMVP();
		drawLimbCube();
		Model = Save;

		Model = glm::translate(Model, glm::vec3(rightLeg.x, rightLeg.y + 0.2, rightLeg.z));
		Model = Model * inversed;
		Model = glm::scale(Model, glm::vec3(0.03f, 0.15f, 0.03f));
		glBindTexture(GL_TEXTURE_2D, textures[9]);
		sendMVP();
		drawLimbCube();
		Model = Save;

	}
	else if (frameCounter <= fps*0.375){
		Model = glm::translate(Model, glm::vec3(leftLeg.x, leftLeg.y + 0.2, leftLeg.z));
		Model = Model * inversed;
		Model = glm::rotate(Model, 45.0f, glm::vec3(1, 0, 0));
		Model = glm::scale(Model, glm::vec3(0.03f, 0.15f, 0.03f));
		glBindTexture(GL_TEXTURE_2D, textures[9]);
		sendMVP();
		drawLimbCube();
		Model = Save;

		Model = glm::translate(Model, glm::vec3(rightLeg.x, rightLeg.y + 0.2, rightLeg.z));
		Model = Model * inversed;
		Model = glm::scale(Model, glm::vec3(0.03f, 0.15f, 0.03f));
		glBindTexture(GL_TEXTURE_2D, textures[9]);
		sendMVP();
		drawLimbCube();
		Model = Save;


	}else if (frameCounter <= fps*0.5){
		Model = glm::translate(Model, glm::vec3(leftLeg.x, leftLeg.y + 0.2, leftLeg.z));
		Model = Model * inversed;
		Model = glm::rotate(Model, 22.5f, glm::vec3(1, 0, 0));
		Model = glm::scale(Model, glm::vec3(0.03f, 0.15f, 0.03f));
		glBindTexture(GL_TEXTURE_2D, textures[9]);
		sendMVP();
		drawLimbCube();
		Model = Save;

		Model = glm::translate(Model, glm::vec3(rightLeg.x, rightLeg.y + 0.2, rightLeg.z));
		Model = Model * inversed;
		Model = glm::scale(Model, glm::vec3(0.03f, 0.15f, 0.03f));
		glBindTexture(GL_TEXTURE_2D, textures[9]);
		sendMVP();
		drawLimbCube();
		Model = Save;

	}
	else if (frameCounter <= fps*0.625){
		Model = glm::translate(Model, glm::vec3(leftLeg.x, leftLeg.y + 0.2, leftLeg.z));
		Model = Model * inversed;
		Model = glm::scale(Model, glm::vec3(0.03f, 0.15f, 0.03f));
		glBindTexture(GL_TEXTURE_2D, textures[9]);
		sendMVP();
		drawLimbCube();
		Model = Save;

		Model = glm::translate(Model, glm::vec3(rightLeg.x, rightLeg.y + 0.2, rightLeg.z));
		Model = Model * inversed;
		Model = glm::scale(Model, glm::vec3(0.03f, 0.15f, 0.03f));
		glBindTexture(GL_TEXTURE_2D, textures[9]);
		sendMVP();
		drawLimbCube();
		Model = Save;

		Model = glm::translate(Model, glm::vec3(leftArm.x, leftArm.y + i, leftArm.z));
		Model = Model * inversed;
		Model = glm::scale(Model, glm::vec3(0.02f, 0.08f, 0.02f));
		glBindTexture(GL_TEXTURE_2D, textures[9]);
		sendMVP();
		drawLimbCube();
		Model = Save;
	}
	else if (frameCounter <= fps*0.75){
		Model = glm::translate(Model, glm::vec3(leftLeg.x, leftLeg.y + 0.2, leftLeg.z));
		Model = Model * inversed;
		Model = glm::scale(Model, glm::vec3(0.03f, 0.15f, 0.03f));
		glBindTexture(GL_TEXTURE_2D, textures[9]);
		sendMVP();
		drawLimbCube();
		Model = Save;

		Model = glm::translate(Model, glm::vec3(rightLeg.x, rightLeg.y + 0.2, rightLeg.z));
		Model = Model * inversed;
		Model = glm::rotate(Model, 22.5f, glm::vec3(1, 0, 0));
		Model = glm::scale(Model, glm::vec3(0.03f, 0.15f, 0.03f));
		glBindTexture(GL_TEXTURE_2D, textures[9]);
		sendMVP();
		drawLimbCube();
		Model = Save;

		Model = glm::translate(Model, glm::vec3(leftArm.x, leftArm.y + i, leftArm.z));
		Model = Model * inversed;
		Model = glm::rotate(Model, 11.25f, glm::vec3(1, 0, 0));
		Model = glm::scale(Model, glm::vec3(0.02f, 0.08f, 0.02f));
		glBindTexture(GL_TEXTURE_2D, textures[9]);
		sendMVP();
		drawLimbCube();
		Model = Save;

	}
	else if (frameCounter <= fps*0.875){
		Model = glm::translate(Model, glm::vec3(leftLeg.x, leftLeg.y + 0.2, leftLeg.z));
		Model = Model * inversed;
		Model = glm::scale(Model, glm::vec3(0.03f, 0.15f, 0.03f));
		glBindTexture(GL_TEXTURE_2D, textures[9]);
		sendMVP();
		drawLimbCube();
		Model = Save;

		Model = glm::translate(Model, glm::vec3(rightLeg.x, rightLeg.y + 0.2, rightLeg.z));
		Model = Model * inversed;
		Model = glm::rotate(Model, 45.0f, glm::vec3(1, 0, 0));
		Model = glm::scale(Model, glm::vec3(0.03f, 0.15f, 0.03f));
		glBindTexture(GL_TEXTURE_2D, textures[9]);
		sendMVP();
		drawLimbCube();
		Model = Save;

		Model = glm::translate(Model, glm::vec3(leftArm.x, leftArm.y + i, leftArm.z));
		Model = Model * inversed;
		Model = glm::rotate(Model, 22.5f, glm::vec3(1, 0, 0));
		Model = glm::scale(Model, glm::vec3(0.02f, 0.08f, 0.02f));
		glBindTexture(GL_TEXTURE_2D, textures[9]);
		sendMVP();
		drawLimbCube();
		Model = Save;

	}
	else if (frameCounter <= fps){
		Model = glm::translate(Model, glm::vec3(leftLeg.x, leftLeg.y + 0.2, leftLeg.z));
		Model = Model * inversed;
		Model = glm::scale(Model, glm::vec3(0.03f, 0.15f, 0.03f));
		glBindTexture(GL_TEXTURE_2D, textures[9]);
		sendMVP();
		drawLimbCube();
		Model = Save;

		Model = glm::translate(Model, glm::vec3(rightLeg.x, rightLeg.y + 0.2, rightLeg.z));
		Model = Model * inversed;
		Model = glm::rotate(Model, 22.5f, glm::vec3(1, 0, 0));
		Model = glm::scale(Model, glm::vec3(0.03f, 0.15f, 0.03f));
		glBindTexture(GL_TEXTURE_2D, textures[9]);
		sendMVP();
		drawLimbCube();
		Model = Save;

		Model = glm::translate(Model, glm::vec3(leftArm.x, leftArm.y + i, leftArm.z));
		Model = Model * inversed;
		Model = glm::rotate(Model, 11.25f, glm::vec3(1, 0, 0));
		Model = glm::scale(Model, glm::vec3(0.02f, 0.08f, 0.02f));
		glBindTexture(GL_TEXTURE_2D, textures[9]);
		sendMVP();
		drawLimbCube();
		Model = Save;
	}

}

//##### Beim Auslösen der Falle #####
void triggerTrap(){
	dead = 1;
	restart = 1;
	loop_menu();
}

//##### Beim Auslösen des Zieles eines Levels.  #####
void triggerFinish(){
	//##### Wenn es noch nicht das letzte Level war #####
	if (levelCount < 2){
		levelCount++;
		loop_game();
	}
	//##### Wenn es das letzte Level war #####
	else{
		finished = 1;
		restart = 1;
		loop_menu();
	}
	
}

//##### Einbinden der Texturen #####
void loadTextures(){
	//##### Texturen für Level 1 #####
	if (levelCount == 1){
		textures[0] = loadBMP_custom("stones.bmp");
		textures[1] = loadBMP_custom("trap_beartrap.bmp");
		textures[2] = loadBMP_custom("start.bmp");
		textures[3] = loadBMP_custom("finish_1.bmp");
		textures[4] = loadBMP_custom("trap_hole.bmp");
		textures[5] = loadBMP_custom("menu.bmp");
		textures[6] = loadBMP_custom("death-screen.bmp");
		textures[7] = loadBMP_custom("finish-screen.bmp");
		textures[8] = loadBMP_custom("wood.bmp");
		textures[9] = loadBMP_custom("arm.bmp");
		textures[10] = loadBMP_custom("stones.bmp");
	}
	//##### Texturen für Level 2 #####
	else if(levelCount == 2){
		textures[0] = loadBMP_custom("level2_walls.bmp");
		textures[1] = loadBMP_custom("trap_beartrap_level2.bmp");
		textures[2] = loadBMP_custom("start_2.bmp");
		textures[3] = loadBMP_custom("finish_2.bmp");
		textures[4] = loadBMP_custom("trap_hole_level2.bmp");
		textures[5] = loadBMP_custom("menu.bmp");
		textures[6] = loadBMP_custom("death-screen.bmp");
		textures[7] = loadBMP_custom("finish-screen.bmp");
		textures[8] = loadBMP_custom("wood.bmp");
		textures[9] = loadBMP_custom("arm.bmp");
		textures[10] = loadBMP_custom("floor_2.bmp");
	}
}

//##### Anzeige des Menüs #####
void loop_menu(){

	glfwSetWindowTitle(window, "CGBeleg - Menu");
	glUseProgram(menuID);

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(menuID, "myTextureSampler"), 0);

	glm::mat4 save = Model;
	glm::vec4 lightPos = glm::vec4(-4,0,0,0);
	glUniform3f(glGetUniformLocation(menuID, "LightPosition_worldspace"), lightPos.x, lightPos.y, lightPos.z);
	
	while (!glfwWindowShouldClose(window)){
		Model = save;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		View = glm::lookAt(
			glm::vec3(0.0f, 0.0f, 0.0f),           
			glm::vec3(1.0f, 0.0f, 0.0f), 
			glm::vec3(0.0f, 1.0f, 0.0f)          
			);
		Projection = glm::perspective(75.0f, 4.0f / 3.0f, 0.1f, 100.0f);
		
		//##### Wenn es der Finish-Screen sein soll #####
		if (finished == 1){
			glBindTexture(GL_TEXTURE_2D, textures[7]);
			init = false;
			levelCount = 1;
			level.clear();
		}
		//##### Wenn man tot ist #####
		else if (dead == 1)
			glBindTexture(GL_TEXTURE_2D, textures[6]);
		//##### Wenn das Spiel gestartet/neugestartet wurde #####
		else if (dead == 0)
			glBindTexture(GL_TEXTURE_2D, textures[5]);

		Model = glm::translate(Model, glm::vec3(0.98f, 0.0f, 0.0f));
		Model = glm::scale(Model, glm::vec3(groesse*0.5, groesse*0.5, groesse*0.5));
		sendMVP();
		drawCube();
		Model = save;

		
		glfwSwapBuffers(window);

		
		glfwPollEvents();
	}
}

//##### Anzeige und Berechnungen der Spielwelt #####
void loop_game(){
	string title = "CGBeleg - Level ";
	title.append(to_string(levelCount));
	glfwSetWindowTitle(window, title.c_str());
	readLevel();
	readLevelControls(levelCount);
	
	loadTextures();
	glUseProgram(gameID);

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(gameID, "myTextureSampler"), 0);

	
	Model = glm::mat4(1.0f);
	glm::mat4 Save = Model;
	drawLevel();
	position = glm::vec3(x, y, z);
	setPosition(position);
	
	while (!glfwWindowShouldClose(window))
	{
		
		int fps = getFPS();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		computeMatricesFromInputs(restart);
		// //##### Zurücksetzen des Merkers, da deltaTime in computeMatricesFromInputs nur einmal pro Restart auf 0 gesetzt werden muss #####
		if (restart == true)
			restart = false;
		Projection = getProjectionMatrix();
		View = getViewMatrix();
		glm::mat4 MVP = Projection * View * Model;

		Model = Save;
		drawLevel();

		glm::vec4 lightPos = glm::vec4(getPositionWithDirection(), 1);
		glUniform3f(glGetUniformLocation(gameID, "LightPosition_worldspace"), lightPos.x, lightPos.y, lightPos.z);

		drawSeg(getPositionWithDirection());

		Model = Save;
		//##### Wenn man sich nach vorne bewegt, soll animiert werden. #####
		//##### Wenn man sich nicht bewegt, soll der frameCounter auf 0 gesetzt werden. #####
		if (glfwGetKey(window, GLFW_KEY_W) != GLFW_PRESS){
			frameCounter = 0;
		}
		else if (frameCounter < fps){
			frameCounter++;
		}
		//##### Wenn frameCounter die frames per second überschritten hat. #####
		else{
			frameCounter = 0;
		}

		animateBody();
		currentBlock = findPosition();
		int x = currentBlock.x;
		int z = currentBlock.y;
		//##### Wenn man sich auf einer Falle befindet. #####
		if (level[z][x] == 2){
			triggerTrap();
		}
		//##### Wenn man sich am Ziel befindet. #####
		else if (level[z][x] == 4){
			triggerFinish();
		}

		glfwSwapBuffers(window);

		glfwPollEvents();
	}
}

//##### Initialisierungen von Parametern. #####
int main()
{
		
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		exit(EXIT_FAILURE);
	}

	glfwSetErrorCallback(error_callback);

	window = glfwCreateWindow(1024, 
		768, 
		"CG Beleg", 
		NULL,  
		NULL); 
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);

	glewExperimental = true; 

	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}
	// Auf Keyboard-Events reagieren
	glfwSetKeyCallback(window, key_callback);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetCursorPos(window, 1024 / 2, 768 / 2);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	menuID = LoadShaders("StandardShading.vertexshader", "StandardShading-menu.fragmentshader");
	gameID = LoadShaders("StandardShading.vertexshader", "StandardShading.fragmentshader");

	glUseProgram(menuID);

	loadTextures();
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(menuID, "myTextureSampler"), 0);

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
	glDeleteTextures(1, &textures[7]);
	glDeleteTextures(1, &textures[8]);
	glDeleteTextures(1, &textures[9]);
	glDeleteTextures(1, &textures[10]);
	glDeleteProgram(menuID);
	glDeleteProgram(gameID);
	glfwTerminate();
	return 0;
}

