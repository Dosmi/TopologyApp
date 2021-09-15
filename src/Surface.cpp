#include "Surface.h"
//#include "extra.h"
using namespace std;

#include <glm/gtx/euler_angles.hpp>

#define DIMENSION 3

namespace
{

    // We're only implenting swept surfaces where the profile curve is
    // flat on the xy-plane.  This is a check function.
    static bool checkFlat(const Curve &profile)
    {
        // cerr << "profile size: " << profile.size() << endl;
        for (unsigned i=0; i<profile.size(); i++)
        { // if profile is empty, just returns true (why it works without any normals etc ...
          //cerr << "V: "; profile[i].V.print();
          //cerr << "T: "; profile[i].T.print();
          //cerr << "N: "; profile[i].N.print();
          //cerr << "B: "; profile[i].B.print();
          //cerr << endl;

          // cerr << i << ") V:" << profile[i].V[2] << " T:" << profile[i].T[2] << " N:" << profile[i].N[2] << endl;
          if (profile[i].V[2] != 0.0 ||
              profile[i].T[2] != 0.0 ||
              profile[i].N[2] != 0.0)
              {
                //cerr << i << ") V:" << profile[i].V[2] << " T:" << profile[i].T[2] << " N:" << profile[i].N[2] << endl;
                //cerr << "V: "; profile[i].V.print();
                //cerr << "T: "; profile[i].T.print();
                //cerr << "N: "; profile[i].N.print();
                //cerr << "returning false" << endl;
                // return false;
              }

        }


        return true;
    }
}

float getRotation(glm::vec3 profilePointingVector, glm::vec3 sweepTangentVector)
{

  float dotProduct = glm::dot(profilePointingVector, sweepTangentVector);
  float divider = glm::length(profilePointingVector) * glm::length(sweepTangentVector);
  float cosAngle = dotProduct / divider;

  float angleBetweenVectors = -acos(cosAngle);

  glm::vec3 cross = glm::cross(profilePointingVector, sweepTangentVector);

  // if the coordinate goes negative, need to flip the angle:
  if (cross.z < 0) angleBetweenVectors = -angleBetweenVectors;

  return angleBetweenVectors;
}

glm::vec3 flipNormals(glm::vec3 normals)
{
  return glm::vec3(normals[0]*(-1), normals[1]*(-1), normals[2]*(-1));
}

/******************************************************************************************************

                   '||               .|'''.|                     .'|. '||''|.
.. .. ..    ....    ||  ..    ....   ||..  '  ... ...  ... ..  .||.    ||   ||    ....  .... ...
 || || ||  '' .||   || .'   .|...||   ''|||.   ||  ||   ||' ''  ||     ||''|'   .|...||  '|.  |
 || || ||  .|' ||   ||'|.   ||      .     '||  ||  ||   ||      ||     ||   |.  ||        '|.|
.|| || ||. '|..'|' .||. ||.  '|...' |'....|'   '|..'|. .||.    .||.   .||.  '|'  '|...'    '|


*******************************************************************************************************/

Surface makeSurfRev(const Curve &profile, unsigned steps)
{   // Surfaces of Revolution
    Surface surface;

    if (!checkFlat(profile))
    {
        cerr << "surfRev profile curve must be flat on xy plane." << endl;
        // exit(0);
    }
    // TODO: Here you should build the surface.  See surf.h for details.

    // +1. evaluate the control points along the profile curve (curve.cpp)
    /* 2. generate the vertices of the surface ...
          ... by simply duplicating the evaluated curve ...
          ... points at evenly sampled values of rotation. */

    //cerr << profile.size() << endl;

    std::vector< std::vector<glm::vec3> > surfacePoints;

    for( unsigned i = 0; i < profile.size(); i++ )
    {
      int profile_number = i;
      // int surface_point_index = 0;
      // cerr << "steps:" << steps << endl;
      int rotation_number = 0;
      std::vector <glm::vec3> rotationsOfOneProfile;
      for ( float singleRotation = (2*3.14)/steps; singleRotation <= 2*3.14; singleRotation += (2*3.14)/steps )
      {
        surface.VV.push_back( rotateVertexAroundAxis_y( profile[i], singleRotation ) );
        surface.VN.push_back( rotateNormalAroundAxis( profile[i], singleRotation, 'y' ) );

        rotationsOfOneProfile.push_back(rotateVertexAroundAxis_y( profile[i], singleRotation ));

        // Tup3u triangle(1u,2u,3u);
        //
        // surface.VF.push_back(triangle);

      }
      surfacePoints.push_back(rotationsOfOneProfile);

    }

    for( unsigned currentProfile = 0; currentProfile < profile.size(); currentProfile++ )
    {
      unsigned rotation_number = 0;
      for ( float singleRotation = (2*3.14)/steps; singleRotation <= 2*3.14; singleRotation += (2*3.14)/steps )
      {
          Tup3u triangle
          (
            ((currentProfile + 1) * steps) + rotation_number + 1,
            (currentProfile * steps) + rotation_number + 1,
            (currentProfile * steps) + rotation_number
          );


          surface.VF.push_back(triangle);

          Tup3u bottomtriangle
          (
            ((currentProfile + 1) * steps) + rotation_number,
            ((currentProfile + 1) * steps) + rotation_number + 1,
            (currentProfile * steps) + rotation_number
          );


          surface.VF.push_back(bottomtriangle);

          rotation_number++;
      }
    }

    //cerr << "\t>>> makeSurfRev called (but not implemented).\n\t>>> Returning empty surface." << endl;

    return surface;
}

glm::mat4 rotateX(float radians)
{
    float c = glm::cos(radians);
    float s = glm::sin(radians);

    float rotation_matrix[16] = {
        1, 0, 0, 0,
        0, c, -s, 0,
        0, s, c, 0,
        0, 0, 0, 1
    };

    return glm::make_mat4(rotation_matrix);
}

// static
glm::mat4 rotateY(float radians)
{
    float c = cos(radians);
    float s = sin(radians);

    float rotation_matrix[16] = {
        c, 0, s, 0,
        0, 1, 0, 0,
        -s, 0, c, 0,
        0, 0, 0, 1
    };

    return glm::make_mat4(rotation_matrix);
}

// static
glm::mat4 rotateZ(float radians)
{
    float c = cos(radians);
    float s = sin(radians);

    float rotation_matrix[16] = {
        c, -s, 0, 0,
        s, c, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };

    return glm::make_mat4(rotation_matrix);

}


//void debugCamera(float rot[][4])
//{
//    std::cout << "!camera-transform: \n"
//        << "!< (0) [0][0] " << rot[0][0] << " (4) [0][1] " << rot[0][1] << " (8) [0][2] " << rot[0][2] << " (12) [0][3] " << rot[0][3] << ">\n"
//        << "!< (1) [1][0] " << rot[1][0] << " (5) [1][1] " << rot[1][1] << " (9) [1][2] " << rot[1][2] << " (13) [1][3] " << rot[1][3] << ">\n"
//        << "!< (2) [2][0] " << rot[2][0] << " (6) [2][1]" << rot[2][1] << " (10) [2][2] " << rot[2][2] << " (14) [2][3] " << rot[2][3] << ">\n"
//        << "!< (3) [3][0] " << rot[3][0] << " (7) [3][1]" << rot[3][1] << " (11) [3][2] " << rot[3][2] << " (15) [3][3] " << rot[3][3] << ">\n]";
//}

Surface generateSphereSurface2(const float& radius, const float& squish_stretch_factor,
                               const int& stackCount, const int& sectorCount)
{
    const float PI = 3.1459;
    float sectorStep = 2 * PI / sectorCount;
    float stackStep = PI / stackCount;
    float sectorAngle, stackAngle;
    float x, y, z, xy;                              // vertex position
    float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
    int element_count = 0;

    int num_points = 0;
    for (int i = 0; i <= stackCount; ++i)
    {
        for (int j = 0; j <= sectorCount; ++j)
        {
            num_points += 4;
        }
    }
    float* points = new float[2 * num_points * DIMENSION]; //(float*)alloca(num_points * DIMENSION * sizeof(float));
    unsigned int* indices = new unsigned int[num_points * 5]; //num_points * DIMENSION]; //(float*)alloca(num_points * DIMENSION * sizeof(float));

    for (int i = 0; i <= stackCount; ++i)
    {
        stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
        xy = radius * cosf(stackAngle);             // r * cos(u)
        z = radius * sinf(stackAngle);              // r * sin(u)

        // add (sectorCount+1) vertices per stack
        // the first and last vertices have same position and normal, but different tex coords
        for (int j = 0; j <= sectorCount; ++j)
        {
            sectorAngle = j * sectorStep;           // starting from 0 to 2pi

            // vertex position (x, y, z)
            x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
            points[element_count    ] = x; //vertices.push_back(x);
            points[element_count + 1] = y; //vertices.push_back(y);
            points[element_count + 2] = z; //vertices.push_back(z);

            // normalized vertex normal (nx, ny, nz)
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;

            points[element_count + 3] = nx;
            points[element_count + 4] = ny;
            points[element_count + 5] = nz;

            element_count += 6;
        }
    }

    int index_counter = 0;
    int k1, k2;
    for (int i = 0; i < stackCount; ++i)
    {
        k1 = i * (sectorCount + 1);     // beginning of current stack
        k2 = k1 + sectorCount + 1;      // beginning of next stack

        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
        {
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1
            if (i != 0)
            {
                indices[index_counter] = k1;
                indices[index_counter + 1] = k2;
                indices[index_counter + 2] = k1 + 1;
                index_counter += 3;
            }

            // k1+1 => k2 => k2+1
            if (i != (stackCount - 1))
            {
                indices[index_counter] = k1 + 1;
                indices[index_counter + 1] = k2;
                indices[index_counter + 2] = k2 + 1;
                index_counter += 3;
            }

            // store indices for lines
             //vertical lines for all stacks, k1 => k2
            indices[index_counter] = k1;
            indices[index_counter + 1] = k2;

            index_counter += 2; // 12;

            if (i != 0)  // horizontal lines except 1st stack, k1 => k+1
            {
                indices[index_counter + 2] = k1;
                indices[index_counter + 3] = k1 + 1;
                //lineIndices.push_back(k1 + 1);
                index_counter += 2; // 12;
            }
        }
    }
    Surface surface;

    surface.point_array = points;
    surface.index_array = indices;
    surface.num_points = num_points; // point_count;
    surface.num_elements = element_count;
    surface.num_triangles = index_counter;

    return surface;

}


Surface generateSphereSurface(const float& radius, const float& squish_stretch_factor,
                              const int& steps1, const int& steps2)
{
    Surface surface;
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

    float* points = new float[2*num_points * DIMENSION]; //(float*)alloca(num_points * DIMENSION * sizeof(float));
    unsigned int* indices = new unsigned int[num_points * 5]; //num_points * DIMENSION]; //(float*)alloca(num_points * DIMENSION * sizeof(float));

    int element_count = 0;
    int index_count = 0;
    int point_count = 0;

    //float theta = 0;

    //steps1 = 6;
    int k1, k2;
    int i_counter = 0;
    for (float theta = 0; theta < 2 * PI; theta += 2 * PI / steps1)
    {

        k1 = i_counter * (steps2 + 1);     // beginning of current stack
        k2 = k1 + steps2 + 1;      // beginning of next stack

        for (float phi = 0; phi < 2 * PI; phi += 2 * PI / steps2)
        {
            x0 = radius * cos(phi) * sin(theta);
            z0 = radius * sin(phi) * sin(theta);
            y0 = radius * cos(theta) * squish_stretch_factor;

            points[element_count]     = x0;
            points[element_count + 1] = z0;
            points[element_count + 2] = y0;
            // normals:
            points[element_count + 3] = x0;
            points[element_count + 4] = z0;
            points[element_count + 5] = y0;

            //indices[index_count    ] = element_count;
            //indices[index_count + 1] = element_count + 1;
            //indices[index_count + 2] = element_count + 2;


            x1 = radius * cos(phi + 2 * PI / steps2) * sin(theta);
            z1 = radius * sin(phi + 2 * PI / steps2) * sin(theta);
            y1 = radius * cos(theta) * squish_stretch_factor;

            points[element_count + 6] = x1;
            points[element_count + 7] = z1;
            points[element_count + 8] = y1;

            // normals:
            points[element_count + 9]  = x1;
            points[element_count + 10] = z1;
            points[element_count + 11] = y1;

            //indices[index_count + 3] = element_count + 3;
            //indices[index_count + 4] = element_count + 4;
            //indices[index_count + 5] = element_count + 5;



            x2 = radius * cos(phi) * sin(theta + 2 * PI / steps1);
            z2 = radius * sin(phi) * sin(theta + 2 * PI / steps1);
            y2 = radius * cos(theta + 2 * PI / steps1) * squish_stretch_factor;

            points[element_count + 12] = x2;
            points[element_count + 13] = z2;
            points[element_count + 14] = y2;
            // normals:
            points[element_count + 15] = x2;
            points[element_count + 16] = z2;
            points[element_count + 17] = y2;

            //indices[index_count + 6] = element_count + 6;
            //indices[index_count + 7] = element_count + 7;
            //indices[index_count + 8] = element_count + 8;


            x3 = radius * cos(phi + 2 * PI / steps2) * sin(theta + 2 * PI / steps1);
            z3 = radius * sin(phi + 2 * PI / steps2) * sin(theta + 2 * PI / steps1);
            y3 = radius * cos(theta + 2 * PI / steps2) * squish_stretch_factor;

            points[element_count + 18] = x3;
            points[element_count + 19] = z3;
            points[element_count + 20] = y3;
            // normals:
            points[element_count + 21] = x3;
            points[element_count + 22] = z3;
            points[element_count + 23] = y3;


            // connect the points
            indices[index_count    ] = point_count;
            indices[index_count + 1] = point_count + 1;
            indices[index_count + 2] = point_count + steps1 + 1;
            indices[index_count + 3] = point_count + steps1;

            element_count += 12*2;
            index_count += 4; // 12;
            point_count += 4;

        }

        i_counter++;
    }

    surface.point_array = points;
    surface.index_array = indices;
    surface.num_points = point_count;
    surface.num_elements = element_count;
    surface.num_triangles = index_count;

    return surface;

    //return { points, indices, point_count, element_count };
}

/********************************************************************************************************

                   '||               ..|'''.|                     ..|'''.|          '||
.. .. ..    ....    ||  ..    ....  .|'     '    ....  .. ...   .|'     '  .... ...  ||
 || || ||  '' .||   || .'   .|...|| ||    .... .|...||  ||  ||  ||          '|.  |   ||
 || || ||  .|' ||   ||'|.   ||      '|.    ||  ||       ||  ||  '|.      .   '|.|    ||
.|| || ||. '|..'|' .||. ||.  '|...'  ''|...'|   '|...' .||. ||.  ''|....'     '|    .||.
                                                                           .. |
                                                                            ''
*********************************************************************************************************/

Surface makeGenCyl(const Curve &profile, const Curve &sweep, bool print )
{   // function to make generalized cylinders:
    Surface surface;
    bool flagFlippedNormals = false;

    if (!checkFlat(sweep))
    {
        cerr << "Generalized Cylinder Profile Curve must be flat on the XY Plane." << endl;
        exit(0);
    }

    std::vector< glm::vec3 > vertex_positions;
    std::vector< glm::vec3 > vertex_normals;
    std::vector< Tup3u >     triangle_connections;

    for( unsigned int j = 0; j < profile.size(); j++ )
    {
      for( unsigned int i = 0; i < sweep.size(); i++ )
      {
          // get angle to rotate each profile circle by (when the circle is swept):
          float singleRotation = getRotation(glm::vec3(0,-1,0), glm::vec3(sweep[i].T.x, sweep[i].T.y, sweep[i].T.z));

          glm::mat4 translationMatrix = {
            1,  0,  0, sweep[i].V.x, //[0],
            0,  1,  0, sweep[i].V.y, //[1],
            0,  0,  1, sweep[i].V.z, //[2],
            0,  0,  0,  1
          };

          glm::mat4 originalPoint = {
            1,  0,  0, profile[j].V.x, //[0],
            0,  1,  0, profile[j].V.y, //[1],
            0,  0,  1, profile[j].V.z, //[2],
            0,  0,  0,  1
          };

          glm::vec3 sweep_vec   = glm::vec3(sweep[j].V.x,   sweep[j].V.y,   sweep[j].V.z);
          glm::vec3 profile_vec = glm::vec3(profile[j].V.x, profile[j].V.y, profile[j].V.z);

          glm::mat4 rotateX90 = glm::eulerAngleXYZ(1.57079632679, 0.0, 0.0);//matrixOfOnes.rotateX(1.57079632679);
          //glm::mat4 rotateX90;
          //rotateX90 = rotateX(1.57079632679f);
          glm::mat4 rotateZn = glm::eulerAngleXYZ(0.0, 0.0, (double)singleRotation);
          //glm::mat4 rotateZn;
          //rotateZn = rotateZ(singleRotation);

          // performs 90 degree rotation (since the circle lies flat at start)
          glm::mat4 rotated90Profile;
          /* flipped rotation */
          //rotated90Profile = rotateX90 * originalPoint;
          rotated90Profile = originalPoint * rotateX90;
          
          /* flipped rotation */
          //glm::mat4 rotatedProfile = rotateZn * rotated90Profile;
          glm::mat4 rotatedProfile = rotated90Profile * rotateZn;

          /* flipped rotation */
          glm::mat4 movedPoint = rotatedProfile * translationMatrix;
          glm::vec3 surfacePoint = glm::vec3(movedPoint[0][3], movedPoint[1][3], movedPoint[2][3]); //.getCol(3);

          //surface.VV.push_back(surfacePoint);
          vertex_positions.push_back(surfacePoint);


          // -----------------------------------------------------------------------------------------
           glm::vec3 flippedNormals = flipNormals(profile[j].N);
           glm::vec3 rotatedX90Normal = glm::rotateX(flippedNormals, 1.57079632679f); //rotateNormalAroundAxis( flippedPoint, 1.57079632679, 'x' );
           flagFlippedNormals = true;

          // *****************************************************************************************
           //glm::vec3 rotatedX90Normal = glm::rotateX(profile_vec, 1.57079632679f);
          //glm::vec3 rotatedX90Normal = rotateNormalAroundAxis( profile[j], 1.57079632679, 'x' );

          // -----------------------------------------------------------------------------------------

          //glm::vec3 rotatedX90Normal_CP;
          ////CurvePoint rotatedX90Normal_CP;
          //rotatedX90Normal_CP = glm::vec3(rotatedX90Normal[0], rotatedX90Normal[1], rotatedX90Normal[2] );
          //
          //surface.VN.push_back(glm::rotateZ(rotatedX90Normal, singleRotation));
          vertex_normals.push_back(glm::rotateZ(rotatedX90Normal, singleRotation));
          //surface.VN.push_back( rotateNormalAroundAxis( rotatedX90Normal_CP, singleRotation, 'z' ) );
          //surface.VN.push_back(surfacePoint);
      }
    }

    // TODO: Here you should build the surface.  See surf.h for details.
    // std::vector< std::vector<glm::vec3> > surfacePoints;

    // have a separate part FOR NOW, to be move, this is experiemental ...
    int steps = sweep.size();
    for( unsigned currentProfile = 0; currentProfile < profile.size()-1; currentProfile++ )
    {
      unsigned rotation_number = 0;
      for( unsigned i = 0; i < sweep.size()-1; i++ )
      {
          if (flagFlippedNormals == true)
          {
              Tup3u triangle
              (
                ((currentProfile + 1) * steps) + rotation_number + 1,
                (currentProfile * steps) + rotation_number,
                (currentProfile * steps) + rotation_number + 1
              );

              Tup3u bottomtriangle
              (
                ((currentProfile + 1) * steps) + rotation_number,
                (currentProfile * steps) + rotation_number,
                ((currentProfile + 1) * steps) + rotation_number + 1
              );

              //cerr << triangle[0] << endl;
              //surface.VV[triangle[0]].print();

              //cerr << triangle[1] << endl;
              //surface.VV[triangle[1]].print();

              //cerr << triangle[2] << endl;
              //surface.VV[triangle[2]].print();
              //cerr << endl;

              //surface.VF.push_back(triangle);
              triangle_connections.push_back(triangle);
              //surface.VF.push_back(bottomtriangle);
              triangle_connections.push_back(bottomtriangle);
          }
          // the normals had to be flipped - since the original shape given had them inversed,
          // so to draw the surface properly we must give triangles to OpenGL in

          else
          {
              Tup3u triangle
              (
                ((currentProfile + 1) * steps) + rotation_number + 1,
                (currentProfile * steps) + rotation_number + 1,
                (currentProfile * steps) + rotation_number
              );

              Tup3u bottomtriangle
              (
                ((currentProfile + 1) * steps) + rotation_number,
                ((currentProfile + 1) * steps) + rotation_number + 1,
                (currentProfile * steps) + rotation_number
              );

              //cerr << triangle[0] << endl;
              //surface.VV[triangle[0]].print();

              //cerr << triangle[1] << endl;
              //surface.VV[triangle[1]].print();

              //cerr << triangle[2] << endl;
              //surface.VV[triangle[2]].print();
              //cerr << endl;

              //surface.VF.push_back(triangle);
              triangle_connections.push_back(triangle);
              //surface.VF.push_back(bottomtriangle);
              triangle_connections.push_back(bottomtriangle);
          }

          rotation_number++;
      }
    }


    int num_points = profile.size() * sweep.size();

    float* points = new float[vertex_positions.size() * DIMENSION + vertex_normals.size() * DIMENSION]; //(float*)alloca(num_points * DIMENSION * sizeof(float));
    unsigned int* indices = new unsigned int[triangle_connections.size() * DIMENSION]; //(float*)alloca(num_points * DIMENSION * sizeof(float));

    int element_count = 0;
    int point_count = 0;

    int count = 0;
    for (int i = 0; i < vertex_positions.size() * DIMENSION + vertex_normals.size() * DIMENSION; i += 2 * DIMENSION)
    {
        points[i] = vertex_positions[count].x;
        points[i + 1] = vertex_positions[count].y;
        points[i + 2] = vertex_positions[count].z;

        points[i + 3] = vertex_normals[count].x;
        points[i + 4] = vertex_normals[count].y;
        points[i + 5] = vertex_normals[count].z;

        count++;
    }
    count = 0;
    for (int i = 0; i < triangle_connections.size() * DIMENSION; i += DIMENSION)
    {
        indices[i] = triangle_connections[count][0];
        indices[i + 1] = triangle_connections[count][1];
        indices[i + 2] = triangle_connections[count][2];

        count++;
    }

    surface.point_array = points;
    surface.index_array = indices;
    surface.num_elements  = vertex_positions.size() * DIMENSION + vertex_normals.size() * DIMENSION;
    surface.num_points    = vertex_positions.size() * DIMENSION + vertex_normals.size() * DIMENSION;
    surface.num_triangles = triangle_connections.size() * DIMENSION;

    //cerr << "\t>>> makeGenCyl called (but not implemented).\n\t>>> Returning empty surface." <<endl;

    return surface;
}

/***********************************************************************************************************

     '||                               .|'''.|                     .'|.
   .. ||  ... ..   ....   ... ... ...  ||..  '  ... ...  ... ..  .||.    ....     ....    ....
 .'  '||   ||' '' '' .||   ||  ||  |    ''|||.   ||  ||   ||' ''  ||    '' .||  .|   '' .|...||
 |.   ||   ||     .|' ||    ||| |||   .     '||  ||  ||   ||      ||    .|' ||  ||      ||
 '|..'||. .||.    '|..'|'    |   |    |'....|'   '|..'|. .||.    .||.   '|..'|'  '|...'  '|...'


************************************************************************************************************/

//void drawSurface(const Surface &surface, bool shaded)
//{
//    // Save current state of OpenGL
//    glPushAttrib(GL_ALL_ATTRIB_BITS);
//
//    if (shaded)
//    {
//        // This will use the current material color and light
//        // positions.  Just set these in drawScene();
//        glEnable(GL_LIGHTING);
//        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//
//        // This tells openGL to *not* draw backwards-facing triangles.
//        // This is more efficient, and in addition it will help you
//        // make sure that your triangles are drawn in the right order.
//        glEnable(GL_CULL_FACE);
//        glCullFace(GL_BACK);
//    }
//    else
//    {
//        glDisable(GL_LIGHTING);
//        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//
//        glColor4f(0.4f,0.4f,0.4f,1.f);
//        glLineWidth(1);
//    }
//
//    glBegin(GL_TRIANGLES);
//    for (unsigned i=0; i<surface.VF.size(); i++)
//    {
//
//      glNormal(surface.VN[surface.VF[i][0]]);
//      glVertex(surface.VV[surface.VF[i][0]]);
//
//      glNormal(surface.VN[surface.VF[i][1]]);
//      glVertex(surface.VV[surface.VF[i][1]]);
//
//      glNormal(surface.VN[surface.VF[i][2]]);
//      glVertex(surface.VV[surface.VF[i][2]]);
//
//      // if ((surface.VV[surface.VF[i][0]] != 0.0) &&
//      //     (surface.VV[surface.VF[i][1]] != 0.0) &&
//      //     (surface.VV[surface.VF[i][2]] != 0.0))
//      //   {
//      //     glNormal(surface.VN[surface.VF[i][0]]);
//      //     glVertex(surface.VV[surface.VF[i][0]]);
//      //
//      //     glNormal(surface.VN[surface.VF[i][1]]);
//      //     glVertex(surface.VV[surface.VF[i][1]]);
//      //
//      //     glNormal(surface.VN[surface.VF[i][2]]);
//      //     glVertex(surface.VV[surface.VF[i][2]]);
//      //   }
//
//    }
//    glEnd();
//
//    glPopAttrib();
//}

/*****************************************************************************************************

     '||                              '|.   '|'                                    '||
   .. ||  ... ..   ....   ... ... ...  |'|   |    ...   ... ..  .. .. ..    ....    ||   ....
 .'  '||   ||' '' '' .||   ||  ||  |   | '|. |  .|  '|.  ||' ''  || || ||  '' .||   ||  ||. '
 |.   ||   ||     .|' ||    ||| |||    |   |||  ||   ||  ||      || || ||  .|' ||   ||  . '|..
 '|..'||. .||.    '|..'|'    |   |    .|.   '|   '|..|' .||.    .|| || ||. '|..'|' .||. |'..|'

*****************************************************************************************************/

//void drawNormals(const Surface &surface, float len)
//{
//    // Save current state of OpenGL
//    glPushAttrib(GL_ALL_ATTRIB_BITS);
//
//    glDisable(GL_LIGHTING);
//    glColor4f(0,1,1,1);
//    glLineWidth(1);
//
//    glBegin(GL_LINES);
//    for (unsigned i=0; i<surface.VV.size(); i++)
//    {
//        glVertex(surface.VV[i]);
//        glVertex(surface.VV[i] + surface.VN[i] * len);
//    }
//    glEnd();
//
//    glPopAttrib();
//}

/*******************************************************************************************************

                   .                       .    ..|''||   '||         || '||''''|  ||  '||
  ...   ... ...  .||.  ... ...  ... ...  .||.  .|'    ||   || ...    ...  ||  .   ...   ||    ....
.|  '|.  ||  ||   ||    ||'  ||  ||  ||   ||   ||      ||  ||'  ||    ||  ||''|    ||   ||  .|...||
||   ||  ||  ||   ||    ||    |  ||  ||   ||   '|.     ||  ||    |    ||  ||       ||   ||  ||
 '|..|'  '|..'|.  '|.'  ||...'   '|..'|.  '|.'  ''|...|'   '|...'     || .||.     .||. .||.  '|...'
                        ||                                         .. |'
                       ''''                                         ''
********************************************************************************************************/

void outputObjFile(ostream &out, const Surface &surface)
{

    for (unsigned i=0; i<surface.VV.size(); i++)
        out << "v  "
            << surface.VV[i][0] << " "
            << surface.VV[i][1] << " "
            << surface.VV[i][2] << endl;

    for (unsigned i=0; i<surface.VN.size(); i++)
        out << "vn "
            << surface.VN[i][0] << " "
            << surface.VN[i][1] << " "
            << surface.VN[i][2] << endl;

    out << "vt  0 0 0" << endl;

    for (unsigned i=0; i<surface.VF.size(); i++)
    {
        out << "f  ";
        for (unsigned j=0; j<3; j++)
        {
            unsigned a = surface.VF[i][j]+1;
            out << a << "/" << "1" << "/" << a << " ";
        }
        out << endl;
    }
}
