#include "curve.h"
//#include "extra.h"
#ifdef WIN32
#include <windows.h>
#endif
//#include <GL/gl.h>

// Additional includes:
#include<cmath> // for pow()
#include <string> // for using strings

#define GLM_SWIZZLE_XYZW 
#include <glm/glm.hpp>              // to enable swizzling .xyz on vectors
#include <glm/gtx/norm.hpp>         // glm::length, glm::length2
#include <glm/gtx/euler_angles.hpp> // glm::eulerAngleXYZ

using namespace std;

namespace
{
    // Approximately equal to.  We don't want to use == because of
    // precision issues with floating point.
    inline bool approx( const glm::vec3& lhs, const glm::vec3& rhs )
    {
        const float eps = 1e-8f;
        return glm::length2(lhs - rhs) < eps;
    }
}

glm::vec3 x (glm::vec3 a, glm::vec3 b)
{
  glm::vec3 axb = glm::vec3 (
                            a[1] * b[2] - a[2] * b[1],
                            a[2] * b[0] - a[0] * b[2],
                            a[0] * b[1] - a[1] * b[0]
                          );
  return axb;
}

glm::vec3 rotateVertexAroundAxis_y ( CurvePoint pointOnCurve, float radians)
{
  glm::mat4 matrixOfOnes(1);
  // translate the 3x1 profile vector by the matrix of ones:
  // (just so we are able to apply transforms later by multiplying it with a 4x4 matrix)
  glm::mat4 profileMatrix = glm::translate(matrixOfOnes, pointOnCurve.V);//matrixOfOnes.translation(pointOnCurve.V);

  // this gets the necessary rotation matrix (on Y axis)
  glm::mat4 rotationMatrix = glm::eulerAngleXYZ(0.0, (double)radians, 0.0); //matrixOfOnes.rotateY(radians);
  // finally, this multiplication rotates the vector (represented by a matrix)
  glm::mat4 rotatedYmatrix = rotationMatrix * profileMatrix;

  // get the last column (where the homogenious vector is)
  glm::vec4 rotatedHomogeniousVector = rotatedYmatrix[3];//.getCol(3);

  return glm::vec3( rotatedHomogeniousVector.x, rotatedHomogeniousVector.y, rotatedHomogeniousVector.z );
}

glm::mat3 getSubmatrix3x3(glm::mat4 in, int i0, int j0)
{
    glm::mat3 out;

    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            out[i][j] = in[i + i0][j + j0];
        }
    }

    return out;
}

glm::vec3 rotateNormalAroundAxis ( CurvePoint pointOnCurve, float radians, char axis)
{
  glm::mat4 matrixOfOnes(1);
  // translate the 3x1 profile vector by the matrix of ones:
  // (just so we are able to apply transforms later by multiplying it with a 4x4 matrix)
  glm::mat3 profileMatrix(
                          1.0, 0.0, pointOnCurve.N[0],
                          0.0, 1.0, pointOnCurve.N[1],
                          0.0, 0.0, pointOnCurve.N[2]
                        );

  glm::mat4 rotationMatrix;
  // this gets the necessary rotation matrix (on Y axis)
  if (axis == 'z')
    rotationMatrix = glm::eulerAngleXYZ(0.0, 0.0, (double)radians); //matrixOfOnes.rotateZ(radians);

  else if (axis == 'y')
    rotationMatrix = glm::eulerAngleXYZ(0.0, (double)radians, 0.0); //matrixOfOnes.rotateY(radians);

  else // (axis == "x")
    rotationMatrix = glm::eulerAngleXYZ((double)radians, 0.0 , 0.0); // matrixOfOnes.rotateX(radians);

  glm::mat3 topleftSubmiatrix = getSubmatrix3x3(rotationMatrix, 0, 0); //rotationMatrix.getSubmatrix3x3(0,0);
  glm::transpose(topleftSubmiatrix); //topleftSubmiatrix.transpose();
  glm::mat3 inversedtranspose = glm::inverse(topleftSubmiatrix); //topleftSubmiatrix.inverse();

  // finally, this multiplication rotates the vector (represented by a matrix)
  // cerr << "DEBUGGING NORMALS: " << endl;
  // cerr << "inversedtranspose" << endl;
  // inversedtranspose.print();
  // cerr << "* profileMatrix" << endl;
  // profileMatrix.print();
  glm::mat3 rotatedYmatrix = inversedtranspose * profileMatrix;
  // cerr << "==" << endl;
  // rotatedYmatrix.print();

  // get the last column (where the homogenious vector is)
  glm::vec3 rotatedNormal = rotatedYmatrix[2]; //.getCol(2);

  return rotatedNormal;
}




//
//Curve generateCircle(int steps, float radius)
//{
//    Curve circle;
//
//    cerr << "  radius [" << radius << "]" << endl;
//
//    curves.push_back(evalCircle(rad, steps));
//
//
//
//    return circle;
//}

//                          '||    ..|'''.|  ||                  '||
//   ....  .... ...  ....    ||  .|'     '  ...  ... ..    ....   ||    ....
// .|...||  '|.  |  '' .||   ||  ||          ||   ||' '' .|   ''  ||  .|...||
// ||        '|.|   .|' ||   ||  '|.      .  ||   ||     ||       ||  ||
//  '|...'    '|    '|..'|' .||.  ''|....'  .||. .||.     '|...' .||.  '|...'
//
Curve evalCircle( float radius, unsigned steps )
{
    // This is a sample function on how to properly initialize a Curve
    // (which is a vector< CurvePoint >).

    // Preallocate a curve with steps+1 CurvePoints
    Curve R( steps+1 );

    // Fill it in counterclockwise
    for( unsigned i = 0; i <= steps; ++i )
    {
        // step from 0 to 2pi
        float t = 2.0f * glm::pi<float>() * float( i ) / steps;

        // Initialize position
        // We're pivoting counterclockwise around the y-axis
        R[i].V = radius * glm::vec3( cos(t), sin(t), 0 );

        // Tangent vector is first derivative
        R[i].T = glm::vec3( -sin(t), cos(t), 0 );

        // Normal vector is second derivative
        R[i].N = glm::vec3( -cos(t), -sin(t), 0 );

        // Finally, binormal is facing up.
        R[i].B = glm::vec3( 0, 0, 1 );
    }

    return R;
}


 //     '||                                ..|'''.|
 //   .. ||  ... ..   ....   ... ... ... .|'     '  ... ...  ... ..  .... ...   ....
 // .'  '||   ||' '' '' .||   ||  ||  |  ||          ||  ||   ||' ''  '|.  |  .|...||
 // |.   ||   ||     .|' ||    ||| |||   '|.      .  ||  ||   ||       '|.|   ||
 // '|..'||. .||.    '|..'|'    |   |     ''|....'   '|..'|. .||.       '|     '|...'
 //
//void drawCurve( const Curve& curve, float framesize )
//{
//    // Save current state of OpenGL
//    glPushAttrib( GL_ALL_ATTRIB_BITS );
//
//    // Setup for line drawing
//    glDisable( GL_LIGHTING );
//    glColor4f( 1, 1, 1, 1 );
//    glLineWidth( 1 );
//
//    // Draw curve
//    glBegin( GL_LINE_STRIP );
//    for( unsigned i = 0; i < curve.size(); ++i )
//    {
//        glVertex( curve[ i ].V );
//    }
//    glEnd();
//
//    glLineWidth( 1 );
//
//    // Draw coordinate frames if framesize nonzero
//    if( framesize != 0.0f )
//    {
//        glm::mat4 M;
//
//        for( unsigned i = 0; i < curve.size(); ++i )
//        {
//            M.setCol( 0, glm::vec4( curve[i].N, 0 ) );
//            M.setCol( 1, glm::vec4( curve[i].B, 0 ) );
//            M.setCol( 2, glm::vec4( curve[i].T, 0 ) );
//            M.setCol( 3, glm::vec4( curve[i].V, 1 ) );
//
//            glPushMatrix();
//            glMultMatrixf( M );
//            glScaled( framesize, framesize, framesize );
//            glBegin( GL_LINES );
//            // normals (red):
//            // glColor3f(red, green, blue)
//            // glVertex3d( 0, 0, 0 )
//            // glVertex3d( 1, 0, 0 ) which vertex to show (if 0,1,1 will draw B+T, 0,0,1 - T)
//            glColor3f( 1, 0, 0 ); glVertex3d( 0, 0, 0 ); glVertex3d( 1, 0, 0 );
//            // binormals B (green)
//            glColor3f( 0, 1, 0 ); glVertex3d( 0, 0, 0 ); glVertex3d( 0, 1, 0 );
//            // recolored T's to cyan for better normal contrast: (0,1,1) instead of (0,0,1)
//            // glColor3f(red, green, blue):
//            glColor3f( 0, 1, 1 ); glVertex3d( 0, 0, 0 ); glVertex3d( 0, 0, 1 );
//            glEnd();
//            glPopMatrix();
//        }
//    }
//
//    // Pop state
//    glPopAttrib();
//}
