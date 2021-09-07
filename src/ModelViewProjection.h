#pragma once
#include "glm/glm.hpp"

struct ModelViewProjection
{
	glm::mat4 model_matrix;
	glm::mat4 view_matrix;
	glm::mat4 projection_matrix;
};