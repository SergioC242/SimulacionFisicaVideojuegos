#include <math.h>

#pragma once
class Vector3D
{

public:
	Vector3D(float X = 0, float Y = 0, float Z = 0) : x(X), y(Y), z(Z){

	}

	//vector modulo
	float Modulo() {
		return sqrt(x * x + y * y + z * z);
	}

	Vector3D Normalize() {
		//Calculate module of vector
		float module = Modulo();
		//Return normalized vector
		return Vector3D(x / module, y / module, z / module);
	}

	// Producto escalar de Vector3 * Vector3
	float Escalar(Vector3D v, Vector3D v2) {
		//return producto escalar
		return (v.x * v2.x) + (v.y * v2.y) + (v.z * v2.z);
	}

	// this * Vector3D
	float Escalar(const Vector3D& v) const {
		return (x * v.x) + (y * v.y) + (z * v.z);
	}

	//override operayor +
	Vector3D operator+(const Vector3D& v) {
		return Vector3D(x + v.x, y + v.y, z + v.z);
	}

	//override operator -
	Vector3D operator-(const Vector3D& v) {
		return Vector3D(x - v.x, y - v.y, z - v.z);
	}

	//override operator * float
	Vector3D operator*(const float f) {
		return Vector3D(x * f, y * f, z * f);
	}

	//override operator * Vector3D
	Vector3D operator*(const Vector3D& v) {
		return Vector3D(x * v.x, y * v.y, z * v.z);
	}

	//override operator /
	Vector3D operator/(const float f) {
		return Vector3D(x / f, y / f, z / f);
	}

	//override operator =
	Vector3D operator=(const Vector3D& v) {
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}

	//override operator +=
	Vector3D operator+=(const Vector3D& v) {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	//override operatior *=
	Vector3D operator*=(const float f) {
		x *= f;
		y *= f;
		z *= f;
		return *this;
	}

	//getters
	float getX() { return x; }
	float getY() { return y; }
	float getZ() { return z; }

private: 
	float x, y, z;
};

