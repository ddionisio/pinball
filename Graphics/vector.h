#ifndef _vector_h
#define _vector_h

#include <assert.h>
#include <math.h>

class Vector2D {
	int x, y;
public:
	Vector2D() : x(0), y(0) {}
	Vector2D(int newX, int newY) : x(newX), y(newY) {}
	Vector2D(const Vector2D &other) : x(other.x), y(other.y) {}
	Vector2D & operator = (const Vector2D &other){ x = other.x; y = other.y; return *this; }
	Vector2D operator + (const Vector2D &other) { return Vector2D(x+other.x, y+other.y); }
	Vector2D operator - (const Vector2D &other) { return Vector2D(x-other.x, y-other.y); }
	Vector2D operator * (const Vector2D &other) { return Vector2D(x*other.x, y*other.y); }
	Vector2D operator / (const Vector2D &other) { assert(other.x > 0); assert(other.y > 0); return Vector2D(x/other.x, y/other.y); }
	int getX() {return x;}
	int getY() {return y;}
	void setX(int newX) { x = newX; }
	void setY(int newY) { y = newY; }
	void setXY(int newX, int newY) { x = newX; y = newY; }
	int DotProd(const Vector2D &other) { return x*other.x + y*other.y; }
	int CrossProd(const Vector2D &other) { return (x*other.y) - (y*other.x); }
};

class Vector3D {
	double x, y, z;
public:
	Vector3D() : x(0.0), y(0.0), z(0.0) {}
	Vector3D(double newX, double newY, double newZ) : x(newX), y(newY), z(newZ) {}
	Vector3D(const Vector3D &other) : x(other.x), y(other.y), z(other.z) {}
	Vector3D & operator = (const Vector3D &other){ x = other.x; y = other.y; z = other.z; return *this; }
	Vector3D operator + (const Vector3D &other) { return Vector3D(x+other.x, y+other.y, z+other.z); }
	Vector3D operator - (const Vector3D &other) { return Vector3D(x-other.x, y-other.y, z-other.z); }
	Vector3D operator * (const Vector3D &other) { return Vector3D(x*other.x, y*other.y, z*other.z); }
	Vector3D operator / (const Vector3D &other) { assert(other.x > 0); assert(other.y > 0); assert(other.z > 0); return Vector3D(x/other.x, y/other.y, z/other.z); }
	
	Vector3D operator + (double other) { return Vector3D(x+other, y+other, z+other); }
	Vector3D operator - (double other) { return Vector3D(x-other, y-other, z-other); }
	Vector3D operator * (double other) { return Vector3D(x*other, y*other, z*other); }
	Vector3D operator / (double other) { assert(other > 0); return Vector3D(x/other, y/other, z/other); }
	
	double getX() {return x;}
	double getY() {return y;}
	double getZ() {return z;}
	void setX(double newX) { x = newX; }
	void setY(double newY) { y = newY; }
	void setZ(double newZ) { y = newZ; }

	void setXYZ(double newX, double newY, double newZ) { x = newX; y = newY; z = newZ; }
	double DotProd(const Vector3D &other) { return x*other.x + y*other.y + z*other.z; }
	Vector3D CrossProd(const Vector3D &other) { return Vector3D(y*other.z - z*other.y, x*other.z - z*other.x, x*other.y - y*other.x); }
	double GetLength() { return sqrt(x*x + y*y + z*z); }
};

#endif