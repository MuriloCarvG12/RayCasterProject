//
// Created by Chine on 03/02/2026.
//

#include "Vector3D.h"

#ifndef UNTITLED_INTERSECTIONS_H
#define UNTITLED_INTERSECTIONS_H

struct Ray3D
{

    Vector_3D origin;
    Vector_3D direction;
    Ray3D(const Vector_3D& o, const Vector_3D& d)
    : origin(o), direction(d)
    {}
};

double SphericalIntersection(Ray3D Ray, Vector_3D Center, double radius)
{
    Vector_3D OC = Ray.origin - Center;
    double distance = 0;
    double HalfB = (OC.DotProduct(Ray.direction));
    double A = Ray.direction.DotProduct(Ray.direction);
    double C = OC.DotProduct(OC) - radius*radius;

    double RootOne = (-(HalfB) + sqrt(HalfB * HalfB - (A * C)))/A;
    double RootTwo = (-(HalfB) - sqrt(HalfB * HalfB - (A * C)))/A;

    if (RootOne < RootTwo)
        { distance = RootTwo; }
    else
        { distance = RootOne; }

    if (std::abs(distance) < 1e-8)
        return -1;

    if ( distance < 0)
        return -1;

    return distance;

}

Vector_3D PlanarIntersection(Ray3D Ray, Vector_3D N, Vector_3D PointOrigin)
{
    Vector_3D PointDifference = PointOrigin - Ray.origin;
    double dividend = N.DotProduct(PointDifference);
    double divisor = N.DotProduct(Ray.direction);

    if (std::abs(divisor) < 1e-8)
        return Vector_3D(0,0,0);

    double result = dividend / divisor;

    if (result < 0)
        return Vector_3D(0,0,0);

    return Ray.origin + Ray.direction.ScalarMultiplication(result);
}

Vector_3D TriangularIntersection(Ray3D Ray, Vector_3D VertexZero, Vector_3D VertexOne, Vector_3D VertexTwo)
{
    Vector_3D Edge1 = VertexOne - VertexZero;
    Vector_3D Edge2 = VertexTwo - VertexZero;
    Vector_3D P = Edge2.CrossProduct(Ray.direction);
    double Det = Edge1.DotProduct(P);

    if (std::abs(Det) < 1e-8)
        return Vector_3D(0, 0, 0);

    double invDet = 1.0 / Det;

    Vector_3D T = Ray.origin - VertexZero;
    double u = T.DotProduct(P) * invDet;

    if (u < 0)
        return Vector_3D(0, 0, 0);

    Vector_3D Q = T.CrossProduct(Edge1);
    double v = Ray.direction.DotProduct(Q) * invDet;

    if (v < 0)
        return Vector_3D(0, 0, 0);

    double t = Edge2.DotProduct(Q) * invDet;

    if (t < 0)
        return Vector_3D(0, 0, 0);

    return Ray.origin + Ray.direction.ScalarMultiplication(t);
}

#endif //UNTITLED_INTERSECTIONS_H