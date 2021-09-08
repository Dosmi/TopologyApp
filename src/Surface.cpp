#include "Surface.h"
//#include "extra.h"
using namespace std;

#include <glm/gtx/euler_angles.hpp>

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

          surface.VV.push_back(surfacePoint);


          // -----------------------------------------------------------------------------------------
          // glm::vec3 flippedNormals = flipNormals(profile[j].N);
          // CurvePoint flippedPoint;
          //
          // flippedPoint.N = glm::vec3(flippedNormals[0], flippedNormals[1], flippedNormals[2] );
          // glm::vec3 rotatedX90Normal = rotateNormalAroundAxis( flippedPoint, 1.57079632679, 'x' );
           flagFlippedNormals = true;

          // *****************************************************************************************
          glm::vec3 rotatedX90Normal = rotateNormalAroundAxis( profile[j], 1.57079632679, 'x' );

          // -----------------------------------------------------------------------------------------

          CurvePoint rotatedX90Normal_CP;
          rotatedX90Normal_CP.N = glm::vec3(rotatedX90Normal[0], rotatedX90Normal[1], rotatedX90Normal[2] );
          //
          surface.VN.push_back( rotateNormalAroundAxis( rotatedX90Normal_CP, singleRotation, 'z' ) );
      }
    }

    // TODO: Here you should build the surface.  See surf.h for details.
    // std::vector< std::vector<glm::vec3> > surfacePoints;

    // have a separate part FOR NOW, to be move, this is experiemental ...
    int steps = sweep.size();
    for( unsigned currentProfile = 0; currentProfile < profile.size()-1; currentProfile++ )
    {
      unsigned rotation_number = 0;
      for( unsigned i = 0; i < sweep.size(); i++ )
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

              surface.VF.push_back(triangle);
              surface.VF.push_back(bottomtriangle);
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

              surface.VF.push_back(triangle);
              surface.VF.push_back(bottomtriangle);
          }

          // cerr << triangle[0] << endl;
          // surface.VV[triangle[0]].print();
          //
          // cerr << triangle[1] << endl;
          // surface.VV[triangle[1]].print();
          //
          // cerr << triangle[2] << endl;
          // surface.VV[triangle[2]].print();
          // cerr << endl;



          rotation_number++;
      }
    }

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
