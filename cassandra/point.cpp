#include "point.h"
#include <cmath>

Point::Point(float px, float py, float pz) : x(px), y(py), z(pz) 
{}


Vector::Vector (float vx, float vy, float vz) : x(vx), y(vy), z(vz)
{}

Vector::Vector(const Point &p)
{
	x=p.x;
	y=p.y;
	z=p.z;
}

float Vector::length() const
{
	return sqrt(squaredLength());
}

float Vector::squaredLength() const
{
	return x*x+y*y+z*z;	
}

void Vector::normalize()
{
	float inv = 1/length();
	x*=inv;
	y*=inv;
	z*=inv;
}


// P+Q
Point   operator+(const Point& p, const Point& q)
{
	return Point(p.x+q.x, p.y+q.y, p.z+q.z);
}

Point&  operator+=(Point& p, const Point& q)
{
	p.x+=q.x;
	p.y+=q.y;
	p.z+=q.z;
	return p;
}

// P-Q
Vector operator-(const Point& p, const Point& q)
{
	return Vector(p.x-q.x, p.y-q.y, p.z-q.z);
}

// P+v
Point  operator+(const Point& p, const Vector &v)
{
	return Point(p.x+v.x, p.y+v.y, p.z+v.z);
}

Point& operator+=(Point& p, const Vector &v)
{
	p.x+=v.x;
	p.y+=v.y;
	p.z+=v.z;
	return p;
}

// P-v 
Point  operator-(const Point& p, const Vector& v)
{
	return Point(p.x-v.x, p.y-v.y, p.z-v.z);
}

Point& operator-=(Point& p, const Vector &v)
{
	p.x-=v.x;
	p.y-=v.y;
	p.z-=v.z;
	return p;
}


Vector   operator+(const Vector& u, const Vector& v)
{
	return Vector(u.x+v.x, u.y+v.y, u.z+v.z);
}

Vector&  operator+=(Vector& u, const Vector& v)
{
	u=u+v;
	return u;
}

// v-u
Vector operator-(const Vector& u, const Vector& v)
{
	return Vector(u.x-v.x, u.y-v.y, u.z-v.z);
}

Vector& operator-=(Vector& u, const Vector &v)
{
	u=u-v;
	return u;
}


// aP
Point operator* (const Point& p, float f)
{
	return Point(p.x*f, p.y*f, p.z*f);
}

Point operator* (float f, const Point& p)
{
	return p*f;
}

// P/a
Point operator/ (const Point& p, float f)
{
	float inv = 1./f;
	return Point(p.x*inv, p.y*inv, p.z*inv);
}

Point& operator/=(Point& p, float f)
{
	float inv = 1./f;
	p.x*=inv;
	p.y*=inv;
	p.z*=inv;
	return p;
}


// av
Vector operator* (const Vector& v, float f)
{
	return Vector(v.x*f, v.y*f, v.z*f);
}

Vector operator* (float f, const Vector& v)
{
	return v*f;
}

// v/a
Vector operator/ (const Vector& v, float f)
{
	float inv = 1./f;
	return Vector(v.x*inv, v.y*inv, v.z*inv);
}

Vector& operator/=(Vector& v, float f)
{
	float inv = 1./f;
	v.x*=inv;
	v.y*=inv;
	v.z*=inv;
	return v;
}


ostream& operator<< (ostream &os, const Point &p) 
{
	os << p.x << ", " << p.y << ", " << p.z << endl;
	return os;
}

ostream& operator<< (ostream &os, const Vector &v) 
{
	os << v.x << ", " << v.y << ", " << v.z << endl;
	return os;
}

