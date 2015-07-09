#ifndef CONTROLS_HPP
#define CONTROLS_HPP
#include <glm/glm.hpp>
void computeMatricesFromInputs(bool restartMerker);
glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();
void setPosition(glm::vec3 position);
glm::vec3 getPosition();
glm::vec3 getPositionWithDirection();
void readLevelControls(int level);
glm::vec2 findPosition();
#endif