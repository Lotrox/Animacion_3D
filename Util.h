#ifndef UTIL_H
#define	UTIL_H

#include <iostream>
#include <sstream>
#include <fstream>
#include "Interpolation.h"

using namespace std;

namespace util {

	static int TAM = 0;
	static int TAM_V = 0;
	static int TAM_T = 0;

	static Point3D* points;
	static Point3D* pointsT;

	static double* kFrames;
	static double* kFramesT;

	static double* velocity;
	static double* rate;

	static void LoadInputs() {
		if (TAM != 0) {
			delete points;
			delete kFrames;
		}
		
		float data;
		string fich = "keyframes.txt";
		ifstream f(fich.c_str(), std::ifstream::in);
		f >> TAM;
		points = new Point3D[TAM];
		kFrames = new double[TAM];
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
		if (TAM_T != 0) {
			delete pointsT;
			delete kFramesT;
		}
		float data;
		string fich = "traveling.txt";
		ifstream f(fich.c_str(), std::ifstream::in);
		f >> TAM_T;
		pointsT = new Point3D[TAM_T];
		kFramesT = new double[TAM_T];

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

	static void LoadSpeed() {
		if (TAM_V != 0) {
			delete velocity;
			delete rate;
		}

		float data;
		string fich = "speedCurve.txt";
		ifstream f(fich.c_str(), std::ifstream::in);
		f >> TAM_V;
		velocity = new double[TAM_V];
		rate = new double[TAM_V];
		for (int i = 0; i < TAM_V; i++) {
			f >> data;
			velocity[i] = data;
		}
		for (int i = 0; i < TAM_V; i++) {
			f >> data;
			rate[i] = data;
		}

	}
}

#endif	/* UTIL_H */