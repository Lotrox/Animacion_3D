#ifndef INTERPOLATION_H
#define	INTERPOLATION_H

#include <functional>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <vector>

using namespace std;

#define W 800
#define H 800

inline double drand() { return rand() / (double)RAND_MAX; }


#define M_PI 3.14159265
#define toRad(x) ((x)*(M_PI/180.0))


class Point3D{
public:
	double x, y, z;

	Point3D() { x = 0; y = 0; z = 0; };
	Point3D(double _x, double _y, double _z) { x = _x; y = _y; z = _z; }

    void Normalize(){
		double norm = sqrt(x*x + y*y + z*z);
		x /= norm; y /= norm; z /= norm;
	}

};

class Quaternion {
public:
	double w;
	Point3D u;

	Quaternion() {}

	Quaternion(double t, Point3D p) {
		p.Normalize();
		u = p;
		float uv = sqrt(u.x*u.x + u.y*u.y + u.z*u.z);
		w = cos(t / 2);
		u.x = sin(t / 2)*u.x/uv;
		u.y = sin(t / 2)*u.y/uv;
		u.z = sin(t / 2)*u.z/uv;
	}

	Quaternion(const Quaternion& q) {
		w = q.w;
		u = q.u;
	}

	Quaternion& operator=(const Quaternion& q) {
		w = q.w;
		u = q.u;
		return *this;
	}

	inline void Multiply(const Quaternion q){
		Quaternion tmp;
		tmp.u.x = ((w * q.u.x) + (u.x * q.w) + (u.y * q.u.z) - (u.z * q.u.y));
		tmp.u.y = ((w * q.u.y) - (u.x * q.u.z) + (u.y * q.w) + (u.z * q.u.x));
		tmp.u.z = ((w * q.u.z) + (u.x * q.u.y) - (u.y * q.u.x) + (u.z * q.w));
		tmp.w = ((w * q.w) - (u.x * q.u.x) - (u.y * q.u.y) - (u.z * q.u.z));
		*this = tmp;
	}

	inline double Norm(){
		return sqrt(u.x*u.x + u.y*u.y + u.z*u.z + w*w);
	}


	inline void Normalize(){
		double norm = Norm();
		u.x /= norm; u.y /= norm; u.z /= norm;
	}

	inline void Conjugate(){
		u.x = -u.x;
		u.y = -u.y;
		u.z = -u.z;
	}

	inline void Inverse(){
		double norm = Norm();
		Conjugate();
		u.x /= norm;
		u.y /= norm;
		u.z /= norm;
		w /= norm;
	}

	void ExportToMatrix(float matrix[16]){
		float wx, wy, wz, xx, yy, yz, xy, xz, zz;
		// adapted from Shoemake
		xx = u.x * u.x;
		xy = u.x * u.y;
		xz = u.x * u.z;
		yy = u.y * u.y;
		zz = u.z * u.z;
		yz = u.y * u.z;

		wx = w * u.x;
		wy = w * u.y;
		wz = w * u.z;

		matrix[0] = 1.0f - 2.0f*(yy + zz);
		matrix[4] = 2.0f*(xy - wz);
		matrix[8] = 2.0f*(xz + wy);
		matrix[12] = 0.0;

		matrix[1] = 2.0f*(xy + wz);
		matrix[5] = 1.0f - 2.0f*(xx + zz);
		matrix[9] = 2.0f*(yz - wx);
		matrix[13] = 0.0;

		matrix[2] = 2.0f*(xz - wy);
		matrix[6] = 2.0f*(yz + wx);
		matrix[10] = 1.0f - 2.0f*(xx + yy);
		matrix[14] = 0.0;

		matrix[3] = 0;
		matrix[7] = 0;
		matrix[11] = 0;
		matrix[15] = 1;
	}

};

static void makeSlerp(Quaternion q1, Quaternion q2, Quaternion &qr, double lambda, bool longWay = false){
	float dotproduct = q1.u.x * q2.u.x + q1.u.y * q2.u.y + q1.u.z * q2.u.z + q1.w * q2.w;
	if ((dotproduct < 0) && (!longWay)) { /*Condición para tomar el camino más corto de la interpolación.*/
		q2.w *= -1;
		q2.u.x *= -1; q2.u.y *= -1; q2.u.z *= -1;
	}
	dotproduct = q1.u.x * q2.u.x + q1.u.y * q2.u.y + q1.u.z * q2.u.z + q1.w * q2.w;
	float theta, st, sut, sout, coeff1, coeff2;

	theta = (float)acos(dotproduct);
	if (theta<0.0) theta = -theta;

	st = (float)sin(theta);
	sut = (float)sin(lambda*theta);
	sout = (float)sin((1 - lambda)*theta);
	coeff1 = sout / st;
	coeff2 = sut / st;

	qr.u.x = coeff1*q1.u.x + coeff2*q2.u.x;
	qr.u.y = coeff1*q1.u.y + coeff2*q2.u.y;
	qr.u.z = coeff1*q1.u.z + coeff2*q2.u.z;
	qr.w = coeff1*q1.w + coeff2*q2.w;

	qr.Normalize();
}

static void makeLerp(Point3D q1, Point3D q2, Point3D& r, float alpha) {
	r.x = (1 - alpha)*q1.x + q2.x*alpha;
	r.y = (1 - alpha)*q1.y + q2.y*alpha;
	r.z = (1 - alpha)*q1.z + q2.z*alpha;
}

	

static Quaternion RotateAboutAxis(Point3D pt, double angle, Point3D axis){
	Quaternion q, p, qinv;

	q.w = cos(0.5*angle);
	q.u.x = sin(0.5*angle)*axis.x;
	q.u.y = sin(0.5*angle)*axis.y;
	q.u.z = sin(0.5*angle)*axis.z;
	p.w = 0;
	p.u = pt;
	qinv = q;
	qinv.Inverse();

	q.Multiply(p);
	q.Multiply(qinv);

	return q;
}

static float CalculateAngle(Point3D a, Point3D b) {
	Point3D c;
	c.x = b.x - a.x;
	c.y = b.y - a.y;
	c.z = b.z - a.z;

	c.Normalize();

	return atan2(-c.z, c.x);
}

static void MultiplyPointMatrix(float m[16], Point3D p, Point3D& rotp){
	rotp.x = m[0] * p.x + m[1] * p.y + m[2] * p.z;
	rotp.y = m[4] * p.x + m[5] * p.y + m[6] * p.z;
	rotp.z = m[8] * p.x + m[9] * p.y + m[10] * p.z;
}



#endif	/* INTERPOLATION_H */