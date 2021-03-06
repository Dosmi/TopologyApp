/***** Ball.c *****/
/* Ken Shoemake, 1993 */
#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/glu.h>

#include "Arcball.h"
#include "ArcballMath.h"
//#include "../../fakeirisgl.h"
#include "ArcballAux.h"

#include <iostream>

#define LG_NSEGS 4
#define NSEGS (1<<LG_NSEGS)
#define RIMCOLOR()    glColor3i(255, 255, 255)
#define FARCOLOR()    glColor3i(195, 127, 31)
#define NEARCOLOR()   glColor3i(255, 255, 63)
#define DRAGCOLOR()   glColor3i(127, 255, 255)
#define RESCOLOR()    glColor3i(195, 31, 31)


HMatrix mId = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};
float otherAxis[][4] = {{-0.48f, 0.80f, 0.36f, 1.f}};

/* Establish reasonable initial values for controller. */
void Ball_Init(BallData *ball)
{
    int i;
//    ball->center = qOne;
    ball->center = {0, 0, 0, 1};
    ball->radius = 1.0;
    ball->vDown = ball->vNow = qOne;
    ball->qDown = ball->qNow = qOne;
    for (i=15; i>=0; i--)
	((float *)ball->mNow)[i] = ((float *)ball->mDown)[i] = ((float *)mId)[i];
    ball->showResult = ball->dragging = FALSE;
    ball->axisSet = NoAxes;
    ball->sets[CameraAxes] = mId[X]; ball->setSizes[CameraAxes] = 3;
    ball->sets[BodyAxes] = ball->mDown[X]; ball->setSizes[BodyAxes] = 3;
    ball->sets[OtherAxes] = otherAxis[X]; ball->setSizes[OtherAxes] = 1;
}

/* Set the center and size of the controller. */
void Ball_Place(BallData *ball, HVect center, float radius)
{
    ball->center = center;
    ball->radius = radius;
}

/* Incorporate new mouse position. */
void Ball_Mouse(BallData *ball, HVect vNow)
{
    ball->vNow = vNow;
}

void Ball_Mouse(BallData *ball, int x, int y, int width, int height)
{
    HVect vNow;
    vNow.x = 2.f*(x - 0) / width  - 1.f;
    vNow.y = 2.f*(y - 0) / height - 1.f;

    ball->mouse_current_x = x;
    ball->mouse_current_y = y;
    ball->vNow = vNow;
}


/* Choose a constraint set, or none. */
void Ball_UseSet(BallData *ball, AxisSet axisSet)
{
    if (!ball->dragging) ball->axisSet = axisSet;
}

/* Begin drawing arc for all drags combined. */
void Ball_ShowResult(BallData *ball)
{
    ball->showResult = TRUE;
}

/* Stop drawing arc for all drags combined. */
void Ball_HideResult(BallData *ball)
{
    ball->showResult = FALSE;
}

/* Using vDown, vNow, dragging, and axisSet, compute rotation etc. */
void Ball_Update(BallData *ball)
{
    switch (ball->mouse_button_state)
    {
        case RIGHT:
        {
//            qDebug() << "* ball->mouse_start_y " << ball->mouse_start_y << " - ball->viewport[1]" << ball->viewport[1];
            int sy = ball->mouse_start_y - ball->viewport[1];
//            qDebug() << "* y " << ball->mouse_current_y << " - ball->viewport[1]" << ball->viewport[1];
            int cy = ball->mouse_current_y - ball->viewport[1];

            float delta = float(cy-sy)/ball->viewport[3];

            // exponential zoom factor
            ball->current_distance = ball->start_distance * exp(delta);
            //std::cout << "* bal: updating current distance: " << ball->current_distance << std::endl;
            //std::cout << "ball->start_distance: " << ball->start_distance << std::endl;
            //std::cout << "delta: " << delta << std::endl;
            //std::cout << "sy: " << sy << ", cy: " << cy << std::endl;
            //std::cout << "ball->viewport[1]: " << ball->viewport[1] << std::endl;
            //std::cout << "ball->viewport[3]: " << ball->viewport[3] << std::endl;
//            qDebug() << "* bal: updating current distance: " << ball->current_distance;
            break;
        }
        case LEFT:
        {
            int setSize = ball->setSizes[ball->axisSet];
            HVect *set = (HVect *)(ball->sets[ball->axisSet]);
            ball->vFrom = MouseOnSphere(ball->vDown, ball->center, ball->radius);
            ball->vTo = MouseOnSphere(ball->vNow, ball->center, ball->radius);
            if (ball->dragging) {
            if (ball->axisSet!=NoAxes) {
                ball->vFrom = ConstrainToAxis(ball->vFrom, set[ball->axisIndex]);
                ball->vTo = ConstrainToAxis(ball->vTo, set[ball->axisIndex]);
            }
            ball->qDrag = Qt_FromBallPoints(ball->vFrom, ball->vTo);
            ball->qNow = Qt_Mul(ball->qDrag, ball->qDown);
            } else {
            if (ball->axisSet!=NoAxes) {
                ball->axisIndex = NearestConstraintAxis(ball->vTo, set, setSize);
            }
            }
            Qt_ToBallPoints(ball->qDown, &ball->vrFrom, &ball->vrTo);
            Qt_ToMatrix(Qt_Conj(ball->qNow), &ball->mNow); /* Gives transpose for GL. */

            break;

        }
    }
}

/* Return rotation matrix defined by controller use. */
void Ball_Value(BallData *ball, HMatrix mNow)
{
    int i;
    for (i=15; i>=0; i--) ((float *)mNow)[i] = ((float *)ball->mNow)[i];
}


/* Begin drag sequence. */
void Ball_BeginDrag(BallData *ball, Button mousekey)
{
    ball->dragging = TRUE;
    ball->vDown = ball->vNow;

    ball->mouse_button_state = mousekey;
}

/* Stop drag sequence. */
void Ball_EndDrag(BallData *ball)
{
    switch (ball->mouse_button_state)
    {
    case LEFT:
        int i;
        ball->dragging = FALSE;
        ball->qDown = ball->qNow;
        for (i=15; i>=0; i--)
        ((float *)ball->mDown)[i] = ((float *)ball->mNow)[i];
    break;
    case RIGHT:
        ball->dragging = FALSE;
        ball->qDown = ball->qNow;
        ball->start_distance = ball->current_distance;
    break;
    default:
    break;
    }

}

/* Draw the controller with all its arcs. */
void Ball_Draw(BallData *ball)
{
    float r = ball->radius;
//    pushmatrix();
    glPushMatrix();
//    loadmatrix(mId);
    glLoadIdentity();
//    ortho2(-1.0, 1.0, -1.0, 1.0);
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    RIMCOLOR();
//    scale(r, r, r);
    glScalef(r,r,r);
//    circ(0.0, 0.0, 1.0);
    Ball_DrawResultArc(ball);
    Ball_DrawConstraints(ball);
    Ball_DrawDragArc(ball);
//    popmatrix();
    glPopMatrix();
}

/* Draw an arc defined by its ends. */
void DrawAnyArc(HVect vFrom, HVect vTo)
{
    int i;
    HVect pts[NSEGS+1];
    float dot;
    pts[0] = vFrom;
    pts[1] = pts[NSEGS] = vTo;
    for (i=0; i<LG_NSEGS; i++) pts[1] = V3_Bisect(pts[0], pts[1]);
    dot = 2.f*V3_Dot(pts[0], pts[1]);
    for (i=2; i<NSEGS; i++) {
	pts[i] = V3_Sub(V3_Scale(pts[i-1], dot), pts[i-2]);
    }
    glBegin(GL_LINE_STRIP);
//    bgnline();
    for (i=0; i<=NSEGS; i++)
    {
        glVertex2fv((float *)&pts[i]);
    }
//	v3f((float *)&pts[i]);
//    endline();
    glEnd();
}

/* Draw the arc of a semi-circle defined by its axis. */
void DrawHalfArc(HVect n)
{
    HVect p, m;
    p.z = 0;
    if (n.z != 1.0) {
	p.x = n.y; p.y = -n.x;
	p = V3_Unit(p);
    } else {
	p.x = 0; p.y = 1;
    }
    m = V3_Cross(p, n);
    DrawAnyArc(p, m);
    DrawAnyArc(m, V3_Negate(p));
}

/* Draw all constraint arcs. */
void Ball_DrawConstraints(BallData *ball)
{
    ConstraintSet set;
    HVect axis;
    int axisI, setSize = ball->setSizes[ball->axisSet];
    if (ball->axisSet==NoAxes) return;
    set = ball->sets[ball->axisSet];
    for (axisI=0; axisI<setSize; axisI++) {
	if (ball->axisIndex!=axisI) {
	    if (ball->dragging) continue;
	    FARCOLOR();
	} else NEARCOLOR();
	axis = *(HVect *)&set[4*axisI];
	if (axis.z==1.0) {
//	    circ(0.0, 0.0, 1.0);
	} else {
	    DrawHalfArc(axis);
	}
    }
}

/* Draw "rubber band" arc during dragging. */
void Ball_DrawDragArc(BallData *ball)
{
    DRAGCOLOR();
    if (ball->dragging) DrawAnyArc(ball->vFrom, ball->vTo);
}

/* Draw arc for result of all drags. */
void Ball_DrawResultArc(BallData *ball)
{
    RESCOLOR();
    if (ball->showResult) DrawAnyArc(ball->vrFrom, ball->vrTo);
}

void Ball_SetViewport(BallData *ball, int width, int height)
{
    ball->viewport[0] = 0;
    ball->viewport[1] = 0;
    ball->viewport[2] = width;
    ball->viewport[3] = height;
}

