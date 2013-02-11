#ifndef _POINT_H_
#define _POINT_H_

#include <iostream>
using namespace std;

// Point - permet representar un punt a l'espai
class Point
{
 public:
  Point(float px=0., float py=0., float pz=0.);

  // atributs
  float x, y, z;	// coordenades del punt
};

// Vector - permet representar un vector de l'espai
class Vector
{
 public:
  Vector (float vx=0., float vy=0., float vz=0.);
  explicit Vector(const Point &p);
  
  float  operator[](int i) const;
  float& operator[](int i); 
  
  float length() const; // modul del vector
  float squaredLength() const; // modul al quadrat
  
  void normalize();
  
  // atributs
  float x, y, z;	// components del vector
};


// Operacions bàsiques entre punts i vectors

// P+Q
Point   operator+(const Point&, const Point&);
Point&  operator+=(Point&, const Point&);

// P-Q
Vector operator-(const Point&, const Point&);

// P+v
Point  operator+(const Point&, const Vector &v);
Point& operator+=(Point&, const Vector &v);

// P-v 
Point  operator-(const Point&, const Vector&);
Point& operator-=(Point&, const Vector &v);

// v+u
Vector   operator+(const Vector&, const Vector&);
Vector&  operator+=(Vector&, const Vector&);

// v-u
Vector operator-(const Vector&, const Vector&);
Vector& operator-=(Vector&, const Vector &v);

// aP
Point operator* (const Point&, float f);
Point operator* (float f, const Point&);

// P/a
Point operator/ (const Point&, float f);
Point&operator/=(Point&, float f);

// av
Vector operator* (const Vector&, float f);
Vector operator* (float f, const Vector&);

// v/a
Vector operator/ (const Vector&, float f);
Vector&operator/=(Vector&, float f);


// escriptura
ostream& operator<< (ostream &os, const Point &p);
ostream& operator<< (ostream &os, const Vector &p);

#endif
