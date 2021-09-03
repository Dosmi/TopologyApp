/***** Ball.h *****/
#ifndef _H_Ball
#define _H_Ball
#include "ArcballAux.h"
#include <cmath>

typedef enum AxisSet{NoAxes, CameraAxes, BodyAxes, OtherAxes, NSets} AxisSet;
typedef enum { NONE, LEFT, MIDDLE, RIGHT } Button;
typedef float *ConstraintSet;
typedef struct {
    HVect center;
    int mouse_start_x, mouse_start_y,
        mouse_current_x, mouse_current_y;
    int viewport[4];
    Button mouse_button_state;
    float radius, start_distance, current_distance;
    Quat qNow, qDown, qDrag;
    HVect vNow, vDown, vFrom, vTo, vrFrom, vrTo;
    HMatrix mNow, mDown;
    Bool showResult, dragging;
    ConstraintSet sets[NSets];
    int setSizes[NSets];
    AxisSet axisSet;
    int axisIndex;
} BallData;

/* Public routines */
void Ball_Init(BallData *ball);
void Ball_Place(BallData *ball, HVect center, float radius);
void Ball_Mouse(BallData *ball, HVect vNow);
void Ball_Mouse(BallData *ball, int x, int y, int width, int height);
void Ball_UseSet(BallData *ball, AxisSet axisSet);
void Ball_ShowResult(BallData *ball);
void Ball_HideResult(BallData *ball);
void Ball_Update(BallData *ball);
void Ball_Value(BallData *ball, HMatrix mNow);
void Ball_BeginDrag(BallData *ball, Button mousekey);
void Ball_EndDrag(BallData *ball);
void Ball_Draw(BallData *ball);

void Ball_SetViewport(BallData* ball, int width, int height);

/* Private routines */
void DrawAnyArc(HVect vFrom, HVect vTo);
void DrawHalfArc(HVect n);
void Ball_DrawConstraints(BallData *ball);
void Ball_DrawDragArc(BallData *ball);
void Ball_DrawResultArc(BallData *ball);
#endif
