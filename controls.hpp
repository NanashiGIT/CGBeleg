#ifndef CONTROLS_HPP
#define CONTROLS_HPP
#include <glm/glm.hpp>
void computeMatricesFromInputs(bool free_flight);
glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();
void setPosition(glm::vec3 position);
glm::vec3 getPosition();

#endif