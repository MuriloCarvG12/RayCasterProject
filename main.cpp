

#include <iostream>

#include "Vector3D.h"
#include "Intersections.h"
#include <cmath>
#include <vector>

struct Pixel
{
    int Red;
    int Green;
    int Blue;
    Pixel(__int8 R, __int8 G, __int8 B);
};

int height = 400;
int width = 600;

//vector stuff - here we set up the vector directions up foward right which will be used to calculate other vectors in our main function
//we also determine the camera position adopting the standard origin XYZ 000

Vector_3D Up = Vector_3D(0, 0, 0);
Vector_3D Foward = Vector_3D(0, 0, -1);
Vector_3D Right = Vector_3D(1, 0, 0);
Vector_3D CameraPosition = Vector_3D(0, 0, 0);


//camera stuff here we define the aspect ratio, as well as the FOV
//reminder that it will be the FOV that will be usd to define how large or thin or image plane will be
//also convert it to radians

double AspectRatio = width / double(height);
int FOV = 60;

int FovRad = (FOV * M_PI )/ 180;
double CameraDistance = 1.0;

//imageplane stuff setting up the width height and locating its center , its important to remember
//that we are assuming the image plane is exactly in front of our camera thus the center is just the camera pos vec
// + the foward * 1

double ImagePlaneWidth = 2.0 * CameraDistance * std::tan(FovRad * 0.5);
double ImagePlaneHeight = ImagePlaneWidth * AspectRatio;

Vector_3D PlaneCenter = CameraPosition + Foward.ScalarMultiplication(CameraDistance);
Vector_3D BottomLeft = PlaneCenter - Right.ScalarMultiplication((ImagePlaneWidth * 0.5)) - Up.ScalarMultiplication((ImagePlaneHeight * 0.5));

//sphere object stuffs
double SphereObjectRadius = 1;
Vector_3D SphereCenter = Vector_3D(0, 0, -5);

// initializing the image buffer setting all pixels to black
std::vector<Pixel> ImageBuffer(height * width, Pixel(0, 0, 0));
int main() {

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double CurrentUp = (y + 0.5) / double(height);
            double CurrentRight = (x + 0.5) / double(width);

            Vector_3D CurrentImagePlanePositon = BottomLeft + Up.ScalarMultiplication((CurrentUp * ImagePlaneHeight)) +
                                                              Right.ScalarMultiplication((CurrentRight * ImagePlaneWidth));

            Vector_3D Direction = CameraPosition - CurrentImagePlanePositon;
            Ray3D CameraRay = Ray3D(CameraPosition,Direction);

            double IntersectionPoint = SphericalIntersection(CameraRay ,SphereCenter,SphereObjectRadius);

            if (IntersectionPoint > 0)
            {
                //our ray sucefully collided against the object in this case a sphere
                Vector_3D pixel = CameraPosition + (Direction.ScalarMultiplication(IntersectionPoint));
                Vector_3D Normal = pixel - SphereCenter;
                Pixel Color(Normal.x, Normal.y, Normal.z);
                ImageBuffer[(width * y) + x] = Color;

            }
            else {
                //return sky color
                Pixel Color(0xAA, 0x00, 0xFF);
                ImageBuffer[(width * y) + x] = Color;
            }
        }
    }
};
