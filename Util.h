#ifndef UTIL_H
#define	UTIL_H

#include <iostream>
#include <sstream>
#include <fstream>
#include "Interpolation.h"

using namespace std;

namespace util {

	static int TAM = 0;

	static void inputPoints(Point3D* &points, float* &frames) {
		float data;
		string fich = "INPUT.txt";
		ifstream f(fich.c_str(), std::ifstream::in);
		f >> TAM;
		points = new Point3D[TAM];
		frames = new float[TAM];
		for (int i = 0; i < TAM; i++) {
			f >> data;
			points[i].x = data;
			f >> data;
			points[i].y = data;
			f >> data;
			points[i].z = data;
		}
		for (int i = 0; i < TAM; i++) {
			f >> data;
			frames[i] = data;
		}
	}
}

#endif	/* UTIL_H */