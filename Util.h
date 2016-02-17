#ifndef UTIL_H
#define	UTIL_H

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

struct Point {
	float x, y, z;
};

namespace util {

	static int TAM = 0;

	static Point interpolacionLineal(float x, Point a, Point b) {
		Point s;
		s.x = x;
		s.y = a.y + ((b.y - a.y) / (b.x - a.x))*(s.x - a.x);
		s.z = a.z + ((b.z - a.z) / (b.y - a.y))*(s.y - a.y);
		return s;
	}

	static Point* inputPoints() {
		float data;
		string fich = "INPUT.txt";
		ifstream f(fich.c_str(), std::ifstream::in);
		f >> TAM;
		cout << TAM << endl;
		Point* points = new Point[TAM];

		for (int i = 0; i < TAM; i++) {
			f >> data;
			points[i].x = data;
			f >> data;
			points[i].y = data;
			f >> data;
			points[i].z = data;
		}
		return points;
	}
}

#endif	/* UTIL_H */