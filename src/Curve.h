#ifndef CURVE_H
#define CURVE_H

#include "Tuple.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtc/type_ptr.hpp"  // make_mat4 
#include "glm/gtx/string_cast.hpp" // for printing


//#include <vecmath.h>
#include <vector>

// The CurvePoint object stores information about a point on a curve
// after it has been tesselated: the vertex (V), the tangent (T), the
// normal (N), and the binormal (B).  It is the responsiblility of
// functions that create these objects to fill in all the data.
struct CurvePoint
{
    glm::vec3 V; // Vertex
    glm::vec3 T; // Tangent  (unit)
    glm::vec3 N; // Normal   (unit)
    glm::vec3 B; // Binormal (unit)
};

// This is just a handy shortcut.

typedef std::vector< CurvePoint > Curve;


////////////////////////////////////////////////////////////////////////////
// The following two functions take an array of control points (stored
// in P) and generate an STL Vector of CurvePoints.  They should
// return an empty array if the number of control points in C is
// inconsistent with the type of curve.  In both these functions,
// "step" indicates the number of samples PER PIECE.  E.g., a
// 7-control-point Bezier curve will have two pieces (and the 4th
// control point is shared).
////////////////////////////////////////////////////////////////////////////

// ------------- other implemented functions --------------------------- //
glm::vec3 rotateVertexAroundAxis_y ( CurvePoint pointOnCurve, float radians);
glm::vec3 rotateNormalAroundAxis ( CurvePoint pointOnCurve, float radians, char axis);

// Assume number of control points properly specifies a piecewise
// Bezier curve.  I.e., C.size() == 4 + 3*n, n=0,1,...
//Curve evalBezier( const std::vector< glm::vec3 >& P, unsigned steps );

// Bsplines only require that there are at least 4 control points.
//Curve evalBspline( const std::vector< glm::vec3 >& P, unsigned steps );

// Create a circle on the xy-plane of radius and steps
//Curve generateCircle(int steps, float radius);
Curve evalCircle( float radius, unsigned steps);


// Draw the curve and (optionally) the associated coordinate frames
// If framesize == 0, then no frames are drawn.  Otherwise, drawn.
//void drawCurve( const Curve& curve, float framesize = 0 );

#endif
