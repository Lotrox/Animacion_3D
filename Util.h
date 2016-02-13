#ifndef UTIL_H
#define	UTIL_H

#include <functional>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

struct Point {
	float x, y;
};

namespace util {

	static int TAM = 0;

	Point interpolacionLineal(float x, Point a, Point b) {
		Point s;
		s.x = x;
		s.y = a.y + ((b.y - a.y) / (b.x - a.x))*(s.x - a.x);
		return s;
	}

	Point* inputPoints() {
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
		}
		cout << "END" << endl;
		return points;
	}


}

#endif	/* UTIL_H */