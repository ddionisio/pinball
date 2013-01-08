#ifndef _vector_h
#define _vector_h

//#include <assert.h>
#include "common.h"
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
	Vector3D(double newX, double newY, double newZ = 0.0) : x(newX), y(newY), z(newZ) {}
	Vector3D(const Vector3D &other) : x(other.x), y(other.y), z(other.z) {}
	Vector3D & operator = (const Vector3D &other){ x = other.x; y = other.y; z = other.z; return *this; }
	
	Vector3D operator + (const Vector3D &other) { return Vector3D(x+other.x, y+other.y, z+other.z); }
	Vector3D operator - (const Vector3D &other) { return Vector3D(x-other.x, y-other.y, z-other.z); }
	Vector3D operator * (const Vector3D &other) { return Vector3D(x*other.x, y*other.y, z*other.z); }
	Vector3D operator / (const Vector3D &other) { assert(other.x > 0); assert(other.y > 0); assert(other.z > 0); return Vector3D(x/other.x, y/other.y, z/other.z); }
	Vector3D & operator += (const Vector3D &other) { *this = *this+other; return *this; }
	Vector3D & operator -= (const Vector3D &other) { *this = *this-other; return *this; }
	Vector3D & operator *= (const Vector3D &other) { *this = *this*other; return *this; }
	Vector3D & operator /= (const Vector3D &other) { *this = *this/other; return *this; }
	
	Vector3D operator + (double other) { return Vector3D(x+other, y+other, z+other); }
	Vector3D operator - (double other) { return Vector3D(x-other, y-other, z-other); }
	Vector3D operator * (double other) { return Vector3D(x*other, y*other, z*other); }
	Vector3D operator / (double other) { assert(other > 0); return Vector3D(x/other, y/other, z/other); }
	Vector3D & operator += (double other) { *this = *this+other; return *this; }
	Vector3D & operator -= (double other) { *this = *this-other; return *this; }
	Vector3D & operator *= (double other) { *this = *this*other; return *this; }
	Vector3D & operator /= (double other) { *this = *this/other; return *this; }
	
	const double &getX() const {return x;}
	const double &getY() const {return y;}
	const double &getZ() const {return z;}
	void setX(const double &newX) { x = newX; }
	void setY(const double &newY) { y = newY; }
	void setZ(const double &newZ) { y = newZ; }

	void setXYZ(const double &newX, const double &newY, const double &newZ) { x = newX; y = newY; z = newZ; }
	double DotProd(const Vector3D &other) const { return x*other.x + y*other.y + z*other.z; }
	Vector3D CrossProd(const Vector3D &other) const { return Vector3D(y*other.z - z*other.y, -(x*other.z - z*other.x), x*other.y - y*other.x); }
	Vector3D ProjOnto(const Vector3D &other) const { double thing = DotProd(other)/DotProd(*this); return Vector3D(x*thing, y*thing, z*thing); }
	double GetLength() { return sqrt(x*x + y*y + z*z); }
};

//returns -1 1 or 0 depending wether pts are clockwise,couter-clockwise...etc.
int CAC(const POINT &checkpt, const POINT &p1, const POINT &p2);

class Line {
public:
	Line() { memset(pt, 0, sizeof(POINT)*2); }
	Line(const POINT & a, const POINT & b) { pt[0] = a; pt[1] = b; }
	Line(const long &x0, const long &y0, const long &x1, const long &y1) { pt[0].x = x0; pt[0].y = y0; pt[1].x = x1; pt[1].y = y1; }
	Line(const Line & other) { pt[0] = other.pt[0]; pt[1] = other.pt[1]; }
	Line & operator =(const Line & other) { pt[0] = other.pt[0]; pt[1] = other.pt[1]; return *this; }
	void setPoints(const POINT & a, const POINT & b) { pt[0] = a; pt[1] = b; }
	void setPoint0(const POINT & a) { pt[0] = a; }
	void setPoint1(const POINT & a) { pt[1] = a; }
	void setPoints(const long &x0, const long &y0, const long &x1, const long &y1) { pt[0].x = x0; pt[0].y = y0; pt[1].x = x1; pt[1].y = y1; }
	double length() //returns the distance between the two points
	{ 
		int dx = pt[0].x - pt[1].x;
		int dy = pt[0].y - pt[1].y;
		return sqrt(dx*dx + dy*dy); 
	}

	double lengthx2() //returns the distance between the two points without sqrt
	{ 
		int dx = pt[0].x - pt[1].x;
		int dy = pt[0].y - pt[1].y;
		return dx*dx + dy*dy;
	}

	bool isHorizontal() { return pt[0].y == pt[1].y ? true : false; }
	bool isVertical() { return pt[0].x == pt[1].x ? true : false; }

	bool isIntersecting(const Line &l, POINT & intersectPt) //caliculate the intersection and returns true if it does intersect
	{
		double larry1, larry2;
		double curly1, curly2;
		double moe1, moe2;
		double determinator;
		double s, t;
		double temp;
		
		POINT A = l.pt[0], B = l.pt[1];

		larry1 = pt[1].x - pt[0].x;
		larry2 = pt[1].y - pt[0].y;

		curly1 = -(B.x - A.x);
		curly2 = -(B.y - A.y);

		moe1 = A.x - pt[0].x;
		moe2 = A.y - pt[0].y;

		determinator = larry1 * curly2 - larry2 * curly1;

		larry1 /= determinator;
		larry2 /= -determinator;
		
		curly1 /= -determinator;
		curly2 /= determinator;

		temp = curly2;

		curly2 = larry1;
		larry1 = temp;

		s = larry1 * moe1 + larry2 * moe2;
		t = curly1 * moe1 + curly2 * moe2;

		if (s >= 0.0 && s <= 1.0 && t >= 0.0 && t <= 1.0)
		{
			double p1 = pt[0].x + s * (pt[1].x - pt[0].x);
			double p2 = pt[0].y + s * (pt[1].y - pt[0].y);

			intersectPt.x = int(p1);
			intersectPt.y = int(p2);

			return true;
		}

		return false;
	}

	bool isIntersecting(const Line & l)
	{
		if(((CAC(pt[0], pt[1], l.pt[0]) *
			CAC(pt[0], pt[1], l.pt[1])) <= 0) &&
			((CAC(l.pt[0], l.pt[1], pt[0]) *
			CAC(l.pt[0], l.pt[1], pt[1])) <= 0))
			return TRUE;
		else
			return FALSE;
	}

	const int getLeftMostX() const { return (pt[0].x < pt[1].x) ? pt[0].x : pt[1].x; }
	const int getRightMostX() const { return (pt[0].x > pt[1].x) ? pt[0].x : pt[1].x; }
	const int getHighestY() const { return (pt[0].y < pt[1].y) ? pt[0].y : pt[1].y; }
	const int getLowestY() const { return (pt[0].y > pt[1].y) ? pt[0].y : pt[1].y; }

	const POINT &getP0() const { return pt[0]; }											  //returns the first point
	const POINT &getP1() const { return pt[1]; }											  //returns the second point
private:
	POINT pt[2];
};

#endif