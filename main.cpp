

#include <iostream>

#include "Vector3D.h"
#include "Intersections.h"
#include <cmath>
#include <fstream>
#include <vector>

struct Pixel
{
    double  Red;
    double  Green;
    double  Blue;
    Pixel(double  R, double  G, double  B)
    : Red(R), Green(G), Blue(B) {}
};

int height = 400;
int width = 600;

//vector stuff - here we set up the vector directions up foward right which will be used to calculate other vectors in our main function
//we also determine the camera position adopting the standard origin XYZ 000

Vector_3D Up = Vector_3D(0, 1, 0);
Vector_3D Foward = Vector_3D(0, 0, -1);
Vector_3D Right = Vector_3D(1, 0, 0);
Vector_3D CameraPosition = Vector_3D(0, 0, 0);


//camera stuff here we define the aspect ratio, as well as the FOV
//reminder that it will be the FOV that will be usd to define how large or thin or image plane will be
//also convert it to radians

double AspectRatio = width / double(height);
int FOV = 60;

double  FovRad = (FOV * M_PI )/ 180;
double CameraDistance = 1.0;

//imageplane stuff setting up the width height and locating its center , its important to remember
//that we are assuming the image plane is exactly in front of our camera thus the center is just the camera pos vec
// + the foward * 1

double ImagePlaneHeight = 2.0 * CameraDistance * std::tan(FovRad * 0.5);
double ImagePlaneWidth  = ImagePlaneHeight * AspectRatio;

Vector_3D PlaneCenter = CameraPosition + Foward.ScalarMultiplication(CameraDistance);
Vector_3D BottomLeft = PlaneCenter - Right.ScalarMultiplication((ImagePlaneWidth * 0.5)) - Up.ScalarMultiplication((ImagePlaneHeight * 0.5));

//sphere object stuffs
double SphereObjectRadius = 1;
Vector_3D SphereCenter = Vector_3D(0, 0, -5);

// initializing the image buffer setting all pixels to black
std::vector<Pixel> ImageBuffer(height * width, Pixel(0, 0, 0));

void WritePpm(std::vector<Pixel> *ImageBuffer, int BufferWidth, int BufferHeight)
{
    std::ofstream ofs("Output.ppm", std::ios::binary);
    ofs << "P6\n"; //making the ppm in the p6 file format
    ofs << BufferWidth << " " << BufferHeight << "\n";
    ofs << "255\n";
    for (int y = BufferHeight - 1; y >= 0; y--)
    {
        for (int x = 0; x < BufferWidth; x++)
        {
            Pixel CurrentBufferPixel = (*ImageBuffer)[(y * BufferWidth) + x];

            auto fixGamma = [](double value) {
                value = std::max(0.0, std::min(1.0, value));
                value = std::pow(value, 1.0 / 2.2);
                return (unsigned char) (std::round(value * 255.0));
            };

            unsigned char R, g, b;
            R = fixGamma(CurrentBufferPixel.Red);
            g = fixGamma(CurrentBufferPixel.Green);
            b = fixGamma(CurrentBufferPixel.Blue);

            ofs << R << g << b;

        }
    }
    ofs.close();
}

int main() {

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double CurrentUp = (y + 0.5) / double(height);
            double CurrentRight = (x + 0.5) / double(width);

            Vector_3D CurrentImagePlanePositon = BottomLeft + Up.ScalarMultiplication((CurrentUp * ImagePlaneHeight)) +
                                                              Right.ScalarMultiplication((CurrentRight * ImagePlaneWidth));

            Vector_3D Direction = (CurrentImagePlanePositon - CameraPosition).normalized();
            Ray3D CameraRay = Ray3D(CameraPosition,Direction);

            double IntersectionPoint = SphericalIntersection(CameraRay ,SphereCenter,SphereObjectRadius);

            if (IntersectionPoint > 0)
            {
                //our ray sucefully collided against the object in this case a sphere
                Vector_3D pixel = CameraPosition + (Direction.ScalarMultiplication(IntersectionPoint));
                Vector_3D Normal = (pixel - SphereCenter).normalized();
                Pixel Color(
                    Normal.x * 0.5 + 0.5,
                    Normal.y * 0.5 + 0.5,
                    Normal.z * 0.5 + 0.5
                );
                ImageBuffer[(width * y) + x] = Color;

            }
            else {
                //return sky color
                Pixel Color(0x9c / 255.0, 0xc1 / 255.0, 0xfc / 255.0);
                ImageBuffer[(width * y) + x] = Color;
            }
        }
    }
    WritePpm(&ImageBuffer, width, height);
    return 0;
};
