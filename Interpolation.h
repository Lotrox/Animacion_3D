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


class Point3D
{
public:
	double x, y, z;

    void Normalize()
	{
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

	inline void Multiply(const Quaternion q)
	{
		Quaternion tmp;
		tmp.u.x = ((w * q.u.x) + (u.x * q.w) + (u.y * q.u.z) - (u.z * q.u.y));
		tmp.u.y = ((w * q.u.y) - (u.x * q.u.z) + (u.y * q.w) + (u.z * q.u.x));
		tmp.u.z = ((w * q.u.z) + (u.x * q.u.y) - (u.y * q.u.x) + (u.z * q.w));
		tmp.w = ((w * q.w) - (u.x * q.u.x) - (u.y * q.u.y) - (u.z * q.u.z));
		*this = tmp;
	}

	inline double Norm()
	{
		return sqrt(u.x*u.x + u.y*u.y + u.z*u.z + w*w);
	}


	inline void Normalize()
	{
		double norm = Norm();
		u.x /= norm; u.y /= norm; u.z /= norm;
	}

	inline void Conjugate()
	{
		u.x = -u.x;
		u.y = -u.y;
		u.z = -u.z;
	}

	inline void Inverse()
	{
		double norm = Norm();
		Conjugate();
		u.x /= norm;
		u.y /= norm;
		u.z /= norm;
		w /= norm;
	}

	void ExportToMatrix(float matrix[16])
	{
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

class Slerp {
public:
	Point3D p, q, Rp;
	double lambdaanim = 0.0;

	Slerp() {}
	Slerp(Point3D _p, Point3D _q) { p = _p; q = _q; }

	void makeSlerp(Quaternion q1, Quaternion q2, Quaternion &qr, double lambda)
	{
		float dotproduct = q1.u.x * q2.u.x + q1.u.y * q2.u.y + q1.u.z * q2.u.z + q1.w * q2.w;
		//cout << dotproduct << endl;
		if (dotproduct < 0) {
			q2.w *= -1;
			q2.u.x *= -1; q2.u.y *= -1; q2.u.z *= -1;
		}
		dotproduct = q1.u.x * q2.u.x + q1.u.y * q2.u.y + q1.u.z * q2.u.z + q1.w * q2.w;
		float theta, st, sut, sout, coeff1, coeff2;

		// algorithm adapted from Shoemake's paper
		//lambda = lambda / 2.0;

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
	
	//theta and phi
	 void Spherical2Cartesian(double t, double p, double &X, double &Y, double &Z)
	{
		X = cos(p)*sin(t);
		Y = sin(p);
		Z = cos(p)*cos(t);
	}
	void randPoints()
	{
		// theta phi of point p
		Spherical2Cartesian(M_PI*drand()*2.0, M_PI*drand()*2.0, p.x, p.y, p.z);

		// theta, phi of point q
		Spherical2Cartesian(M_PI*drand()*2.0, M_PI*drand()*2.0, q.x, q.y, q.z);
	}
};


static Quaternion RotateAboutAxis(Point3D pt, double angle, Point3D axis)
{
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
	
	//cout << c.x << " " << c.y << " " << c.z << endl;

	//float h = sqrt(pow(c.x, 2) + pow(c.z, 2));
	return atan2(-c.z, c.x);
}

static void MultiplyPointMatrix(float m[16], Point3D p, Point3D& rotp)
{
	rotp.x = m[0] * p.x + m[1] * p.y + m[2] * p.z;
	rotp.y = m[4] * p.x + m[5] * p.y + m[6] * p.z;
	rotp.z = m[8] * p.x + m[9] * p.y + m[10] * p.z;
}

static void make_lerp(Point3D q1, Point3D q2, Point3D& r, float alpha) {
	r.x = (1 - alpha)*q1.x + q2.x*alpha;
	r.y = (1 - alpha)*q1.y + q2.y*alpha;
	r.z = (1 - alpha)*q1.z + q2.z*alpha;
}


#endif	/* INTERPOLATION_H */