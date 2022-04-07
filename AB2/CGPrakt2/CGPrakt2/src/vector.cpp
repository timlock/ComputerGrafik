#include "vector.h"
#include <assert.h>
#include <math.h>


#define EPSILON 1e-6

Vector::Vector( float x, float y, float z)
{
	X = x;
	Y = y;
	Z = z;

}

Vector::Vector()
{
	
}

float Vector::dot(const Vector& v) const
{
	return  X * v.X + Y * v.Y + Z * v.Z ;
}

Vector Vector::cross(const Vector& v) const
{
	float cX = Y * v.Z - Z * v.Y;
	float cY = Z * v.X - X * v.Z;
	float cZ = X * v.Y - Y * v.X;
	return Vector(cX, cY, cZ);
}



Vector Vector::operator+(const Vector& v) const
{
	return Vector(X + v.X, Y + v.Y, Z + v.Z);
}

Vector Vector::operator-(const Vector& v) const
{
	return Vector(X - v.X, Y - v.Y, Z - v.Z);
}

Vector Vector::operator*(float c) const
{
	return Vector(X * c, Y * c, Z * c);
}

Vector Vector::operator-() const
{
	return Vector(-X, -Y, -Z); 
}

Vector& Vector::operator+=(const Vector& v)
{
	X += v.X;
	Y += v.Y;
	Z += v.Z;
	return *this; 
}



Vector& Vector::normalize()
{
	float l = length();
	X = X * 1.0f / l;
	Y = Y * 1.0f / l;
	Z = Z * 1.0f / l; 
	return *this;
}

float Vector::length() const
{
	return sqrt(lengthSquared());
}

float Vector::lengthSquared() const
{
	return X * X + Y * Y + Z * Z;
}

Vector Vector::reflection( const Vector& normal) const
{
	Vector reflectionV = *this - normal * (2.0f * dot(normal));
	//Vector reflectionV = normal * dot(normal) *  2.0f - *this;
	return reflectionV;
}

float triangleSurface(Vector a, Vector b, Vector c) 
{
	return (b - a).cross(c - a).length() / 2.0f;
}

bool Vector::triangleIntersection( const Vector& d, const Vector& a, const Vector& b, const Vector& c, float& s) const
{
	Vector normaleEbene = (b - a).cross(c - a);
	normaleEbene = normaleEbene * (1.0f / normaleEbene.length());
	float ebeneD = a.dot(normaleEbene);
	s = (ebeneD - normaleEbene.dot(*this)) / normaleEbene.dot(d);
	if (s < 0) return false;
	Vector p = *this + d * s;
	return triangleSurface(a,b,c) + EPSILON >= triangleSurface(a,b,p) +  triangleSurface(a,c,p) + triangleSurface(b,c,p);
}
