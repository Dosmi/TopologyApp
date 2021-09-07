#include "ShapePointCloud.h"

#define DIMENSION 3

void ShapePointCloud::generateCirclePC(int points_to_generate, bool random, float radius, glm::vec3 center)
{
    // allocate array which will be returned with points:
    //float* points = (float*)alloca(num_points * sizeof(float));
    float* points = new float[points_to_generate * DIMENSION]; //(float*)alloca(num_points * DIMENSION * sizeof(float));
    unsigned int* indices = new unsigned int[points_to_generate * DIMENSION]; //(float*)alloca(num_points * DIMENSION * sizeof(float));
    float degree_step = 360.f / float(points_to_generate);
    //std::cout << "center: " << center.x << ", " << center.y << ", " << center.z << std::endl;

    int element_count = 0;
    for (int i = 0; i < points_to_generate; i++)
    {
        if (random)
        {
            // TODO
        }
        else
        {
            float deg = 0.f + degree_step * i;
            //std::cout << "move deg:" << deg << std::endl;
            //float circle_x = center.x - radius * glm::sin(deg);
            float circle_x = center.x - radius * glm::sin((deg * glm::pi<float>()) / 180.f);
            //float circle_y = center.y - radius * glm::cos(deg);
            float circle_y = center.y - radius * glm::cos((deg * glm::pi<float>()) / 180.f);
            float circle_z = center.z; //0.f;

            points[element_count] = circle_x;
            points[element_count + 1] = circle_y;
            points[element_count + 2] = circle_z;

            indices[element_count] = element_count;
            indices[element_count + 1] = element_count + 1;
            indices[element_count + 2] = element_count + 2;

            //std::cout << circle_x << ", " << circle_y << ", " << circle_z << std::endl;

            element_count += 3;
        }
    }

    point_array = points;
    index_array = indices;
    num_points = points_to_generate;
    num_elements = element_count;

    //return { points, indices, num_points, element_count };

}


void ShapePointCloud::generateTorusPC(int points_to_generate, bool random, float radius, glm::vec3 center)
{
    // allocate array which will be returned with points:
    //float* points = (float*)alloca(num_points * sizeof(float));
    float* points = new float[points_to_generate * points_to_generate * DIMENSION]; //(float*)alloca(num_points * DIMENSION * sizeof(float));
    unsigned int* indices = new unsigned int[points_to_generate * points_to_generate * DIMENSION]; //(float*)alloca(num_points * DIMENSION * sizeof(float));
    float degree_step = 360.f / float(points_to_generate);
    //std::cout << "center: " << center.x << ", " << center.y << ", " << center.z << std::endl;


    int element_count = 0;

    for (int i = 0; i < points_to_generate; i++)
    {
        float deg_out = 0.f + degree_step * i;
        //std::cout << "move deg:" << deg << std::endl;
        //float circle_x = center.x - radius * glm::sin(deg);
        float center_x = center.x - radius * glm::sin(glm::radians(deg_out));
        //float circle_y = center.y - radius * glm::cos(deg);
        float center_y = center.y - radius * glm::cos(glm::radians(deg_out));
        float center_z = 0.f;

        center = glm::vec3(center_x, center_y, center_z);

        for (int j = 0; j < points_to_generate; j++)
        {
            float deg_in = 0.f + degree_step * j;
            //std::cout << "move deg:" << deg << std::endl;
            //float circle_x = center.x - radius * glm::sin((deg * glm::pi<float>()) / 180.f);
            float circle_x = radius * glm::sin(glm::radians(deg_in));
            //float circle_y = center.y - radius * glm::cos((deg * glm::pi<float>()) / 180.f);
            float circle_y = radius * glm::cos(glm::radians(deg_in));
            float circle_z = 0.f;

            glm::vec3 torus_point = glm::rotateX(glm::vec3(circle_x, circle_y, circle_z), 90 * glm::pi<float>() / 180.f);
            torus_point = glm::rotateZ(torus_point, deg_out * glm::pi<float>() / 180.f);

            points[element_count] = center.x - torus_point.x;
            points[element_count + 1] = center.y - torus_point.y;
            points[element_count + 2] = torus_point.z;

            indices[element_count] = element_count;
            indices[element_count + 1] = element_count + 1;
            indices[element_count + 2] = element_count + 2;

            //std::cout << circle_x << ", " << circle_y << ", " << circle_z << std::endl;

            element_count += 3;

        }
    }

    point_array = points;
    index_array = indices;
    num_points = points_to_generate;
    num_elements = element_count;

    //return { points, indices, num_points, element_count };
}

void ShapePointCloud::generateSpherePC(const float& radius, const float& squish_stretch_factor,
                                       const int& steps1, const int& steps2)
{
    const float PI = 3.1459;
    float x0, x1, x2, x3, y0, y1, y2, y3, z0, z1, z2, z3;

    int num_points = 0;
    for (float theta = 0; theta < 2 * PI; theta += 2 * PI / steps1)
    {
        for (float phi = 0; phi < 2 * PI; phi += 2 * PI / steps2)
        {
            num_points += 4;
        }
    }

    float* points = new float[num_points * DIMENSION]; //(float*)alloca(num_points * DIMENSION * sizeof(float));
    unsigned int* indices = new unsigned int[num_points * DIMENSION]; //(float*)alloca(num_points * DIMENSION * sizeof(float));

    int element_count = 0;
    int point_count = 0;
    for (float theta = 0; theta < 2 * PI; theta += 2 * PI / steps1)
    {
        for (float phi = 0; phi < 2 * PI; phi += 2 * PI / steps2)
        {
            x0 = radius * cos(phi) * sin(theta);
            z0 = radius * sin(phi) * sin(theta);
            y0 = radius * cos(theta) * squish_stretch_factor;

            points[element_count] = x0;
            points[element_count + 1] = z0;
            points[element_count + 2] = y0;

            indices[element_count] = element_count;
            indices[element_count + 1] = element_count + 1;
            indices[element_count + 2] = element_count + 2;


            x1 = radius * cos(phi + 2 * PI / steps2) * sin(theta);
            z1 = radius * sin(phi + 2 * PI / steps2) * sin(theta);
            y1 = radius * cos(theta) * squish_stretch_factor;

            points[element_count + 3] = x1;
            points[element_count + 4] = z1;
            points[element_count + 5] = y1;

            indices[element_count + 3] = element_count + 3;
            indices[element_count + 4] = element_count + 4;
            indices[element_count + 5] = element_count + 5;



            x2 = radius * cos(phi) * sin(theta + 2 * PI / steps1);
            z2 = radius * sin(phi) * sin(theta + 2 * PI / steps1);
            y2 = radius * cos(theta + 2 * PI / steps1) * squish_stretch_factor;

            points[element_count + 6] = x2;
            points[element_count + 7] = z2;
            points[element_count + 8] = y2;

            indices[element_count + 6] = element_count + 6;
            indices[element_count + 7] = element_count + 7;
            indices[element_count + 8] = element_count + 8;



            x3 = radius * cos(phi + 2 * PI / steps2) * sin(theta + 2 * PI / steps1);
            z3 = radius * sin(phi + 2 * PI / steps2) * sin(theta + 2 * PI / steps1);
            y3 = radius * cos(theta + 2 * PI / steps2) * squish_stretch_factor;

            points[element_count + 9] = x3;
            points[element_count + 10] = z3;
            points[element_count + 11] = y3;

            indices[element_count + 9] = element_count + 9;
            indices[element_count + 10] = element_count + 10;
            indices[element_count + 11] = element_count + 11;

            element_count += 12;
            point_count += 4;

        }
    }

    point_array = points;
    index_array = indices;
    num_points = point_count;
    num_elements = element_count;

    //return { points, indices, point_count, element_count };
}
