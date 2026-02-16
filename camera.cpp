//
// Created by Chine on 09/02/2026.
//

#include "camera.h"
#include "Vector3D.h"

struct Camera {
    Vector_3D Position;
    Vector_3D Forward;
    Vector_3D Right;
    Vector_3D Up;
};


Vector_3D WorldUp(0 , 1 , 0);
Vector_3D Position(0,0,0);
Vector_3D LookDirection(1,0,0);

Vector_3D Forward = LookDirection.normalized();
Vector_3D Right = WorldUp.CrossProduct(Forward).normalized();
Vector_3D Up = Forward.CrossProduct(Right).normalized();

Camera cam = {
    Position,  Forward,  Right, Up
};

double FOV = 90;
double FOVRadians = FOV * (M_PI / 180.0);

double AspectRatio = 16.0f / 9.0f;


struct ImagePlane {
    double ImagePlaneHeight =  2.0 * tan(FOVRadians / 2.0);
    double ImagePlaneWidth = ImagePlaneHeight * AspectRatio;
    Vector_3D ImagePlanePosition; // which will be placed 1 unit ahead of the camera's position so its position will be camera.origin + Foward we are talking about vectors of course
    double BottomLeftX = -(ImagePlaneWidth/2);
    double BottomLeftY = -(ImagePlaneHeight/2);
};