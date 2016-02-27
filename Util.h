#ifndef UTIL_H
#define	UTIL_H

#include <iostream>
#include <sstream>
#include <fstream>
#include "Interpolation.h"

using namespace std;

namespace util {

	static int TAM = 0;
	static int TAM_T = 0;
	static Point3D* points;
	static float* kFrames;
	static Point3D* pointsT;
	static float* kFramesT;

	static void LoadInputs() {
		float data;
		string fich = "INPUT.txt";
		ifstream f(fich.c_str(), std::ifstream::in);
		f >> TAM;
		points = new Point3D[TAM];
		kFrames = new float[TAM];
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
			kFrames[i] = data;
		}
	}
	static void LoadTraveling() {
		float data;
		string fich = "TRAVELING.txt";
		ifstream f(fich.c_str(), std::ifstream::in);
		f >> TAM_T;
		pointsT = new Point3D[TAM_T];
		kFramesT = new float[TAM_T];

		for (int i = 0; i < TAM_T; i++) {
			f >> data;
			pointsT[i].x = data;
			f >> data;
			pointsT[i].y = data;
			f >> data;
			pointsT[i].z = data;
		}
		for (int i = 0; i < TAM_T; i++) {
			f >> data;
			kFramesT[i] = data;
		}
	}
}

#endif	/* UTIL_H */