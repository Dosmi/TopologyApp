#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtc/type_ptr.hpp"  // make_mat4 
#include "glm/gtx/string_cast.hpp" // for printing

struct ShapePointCloud
{
    void generateCirclePC(int points_to_generate, bool random, float radius, glm::vec3 center);
    void generateTorusPC(int points_to_generate, bool random, float radius, glm::vec3 center);
    void generateSpherePC(const float& radius, const float& squish_stretch_factor,
                          const int& steps1, const int& steps2);

    float* point_array;
    unsigned int* index_array;
    int    num_points;
    int    num_elements;
};
