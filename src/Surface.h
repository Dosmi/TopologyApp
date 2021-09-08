#ifndef SURF_H
#define SURF_H

#include "Curve.h"
#include "Tuple.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtc/type_ptr.hpp"  // make_mat4 
#include "glm/gtx/string_cast.hpp" // for printing

#include <iostream>

// Tup3u is a handy shortcut for an array of 3 unsigned integers.  You
// can access elements using [], and you can copy using =, and so on.
typedef tuple< unsigned, 3 > Tup3u;
// typedef tuple< 3, unsigned > Tup3u;

// Surface is just a struct that contains vertices, normals, and
// faces.  VV[i] is the position of vertex i, and VN[i] is the normal
// of vertex i.  A face is a triple i,j,k corresponding to a triangle
// with (vertex i, normal i), (vertex j, normal j), ...
struct Surface
{
    std::vector< glm::vec3 > VV;
    std::vector< glm::vec3 > VN;
    std::vector< Tup3u > VF;
};

glm::vec3 flipNormals( glm::vec3 normals );
// This draws the surface.  Draws the surfaces with smooth shading if
// shaded==true, otherwise, draws a wireframe.
//void drawSurface( const Surface& surface, bool shaded );

// This draws normals to the surface at each vertex of length len.
//void drawNormals( const Surface& surface, float len );

// Sweep a profile curve that lies flat on the xy-plane around the
// y-axis.  The number of divisions is given by steps.
Surface makeSurfRev( const Curve& profile, unsigned steps );

Surface makeGenCyl( const Curve& profile,
                    const Curve& sweep,
                    bool print);

void outputObjFile( std::ostream& out, const Surface& surface );





#endif
