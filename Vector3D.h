//
// Created by Chine on 03/02/2026.
//

#include <cmath>

#ifndef UNTITLED_VECTOR3D_H
#define UNTITLED_VECTOR3D_H

struct Vector_3D
{
    double x, y, z;

    Vector_3D(double x, double y, double z);

    Vector_3D operator+(const Vector_3D& VectorB) const;

    Vector_3D operator-(const Vector_3D &VectorB) const;

    Vector_3D ScalarMultiplication(double Scalar) const;

    Vector_3D ScalarDivision(double Scalar) const;

    double DotProduct(Vector_3D VectorB) const;

    Vector_3D CrossProduct(const Vector_3D& VectorB) const;

    Vector_3D ParametricEquation(const Vector_3D& VectorB, double Scalar) const;

    double length() const;


    Vector_3D normalized() const;
};


#endif //UNTITLED_VECTOR3D_H