
#include <iostream>
#include <ostream>
#include <cmath>

struct Vector_3D
{
    double x, y, z;

    Vector_3D(double x, double y, double z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    Vector_3D operator+(const Vector_3D& VectorB) const
    {

        return Vector_3D(

            x + VectorB.x,

            y + VectorB.y,

            z + VectorB.z
        );
    }

    Vector_3D operator-(const Vector_3D &VectorB) const
    {

        return Vector_3D(

           x - VectorB.x,

           y - VectorB.y,

           z - VectorB.z
       );

    }

    Vector_3D ScalarMultiplication(double Scalar) const
    {

        return Vector_3D(

           x * Scalar,

           y * Scalar,

           z * Scalar
       );

    }

    Vector_3D ScalarDivision(double Scalar) const
    {

        return Vector_3D
        (

            x / Scalar,

            y / Scalar,

            z / Scalar

            );
    }

    double DotProduct(Vector_3D VectorA, Vector_3D VectorB) const
    {

        double Result;

        Result = (VectorA.x *  VectorB.x) + (VectorA.y *  VectorB.y) + (VectorA.z *  VectorB.z);

        return Result;

    }

    Vector_3D CrossProduct(const Vector_3D& VectorB) const
    {
        return Vector_3D
        (
            y * VectorB.z - z * VectorB.y,

            z * VectorB.x - x * VectorB.z,

            x * VectorB.y - y * VectorB.x
        );
    }

    Vector_3D ParametricEquation(const Vector_3D& VectorB, double Scalar) const
    {
        return Vector_3D
        (
            x + Scalar * VectorB.x,

            y + Scalar * VectorB.y,

            z + Scalar * VectorB.z
        );
    }

    double length() const
    {
        double SquareSum = x*x + y*y + z*z;
        return std::sqrt(SquareSum);
    }

    Vector_3D normalized() const
    {
        double len = length();

        return Vector_3D
        (
            x / len,
            y / len,
            z / len);
    }


};

int main() {
    Vector_3D A = {1,1,1};
    Vector_3D B = {1,1,1};
    Vector_3D C = A.ScalarMultiplication(9);

    std::cout << C.x << " " << C.y << " " << C.z << std::endl;
}
