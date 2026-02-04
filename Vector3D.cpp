//
// Created by Chine on 03/02/2026.
//

#include "Vector3D.h"
Vector_3D::Vector_3D(double x, double y, double z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}


Vector_3D Vector_3D::operator+(const Vector_3D& VectorB) const
{
    return Vector_3D(
        x + VectorB.x,
        y + VectorB.y,
        z + VectorB.z
    );
}


Vector_3D Vector_3D::operator-(const Vector_3D& VectorB) const
{
    return Vector_3D(
        x - VectorB.x,
        y - VectorB.y,
        z - VectorB.z
    );
}


Vector_3D Vector_3D::ScalarMultiplication(double Scalar) const
{
    return Vector_3D(
        x * Scalar,
        y * Scalar,
        z * Scalar
    );
}


Vector_3D Vector_3D::ScalarDivision(double Scalar) const
{
    return Vector_3D(
        x / Scalar,
        y / Scalar,
        z / Scalar
    );
}


double Vector_3D::DotProduct(Vector_3D VectorB) const
{
    return (x * VectorB.x)
         + (y * VectorB.y)
         + (z * VectorB.z);
}


Vector_3D Vector_3D::CrossProduct(const Vector_3D& VectorB) const
{
    return Vector_3D(
        y * VectorB.z - z * VectorB.y,
        z * VectorB.x - x * VectorB.z,
        x * VectorB.y - y * VectorB.x
    );
}


Vector_3D Vector_3D::ParametricEquation(const Vector_3D& VectorB, double Scalar) const
{
    return Vector_3D(
        x + Scalar * VectorB.x,
        y + Scalar * VectorB.y,
        z + Scalar * VectorB.z
    );
}


double Vector_3D::length() const
{
    return std::sqrt(x*x + y*y + z*z);
}


Vector_3D Vector_3D::normalized() const
{
    double len = length();

    return Vector_3D(
        x / len,
        y / len,
        z / len
    );
}