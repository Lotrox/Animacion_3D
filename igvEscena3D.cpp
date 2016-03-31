                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      #include <cstdlib>
#include <stdio.h>
#include <vector>
#include <iostream>
#include "igvEscena3D.h"
#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat)
#include <opencv2/highgui/highgui.hpp>  // Video write
#include <opencv2/imgproc/imgproc.hpp>
#include <thread>
#include <chrono>

using namespace cv;

static int keyFrame = 0; //Fotograma clave actual.
static float lambda = 0; //Valor entre 0 y 1 de la interpolación lineal.
static Point3D r; //Punto resultante de la interpolación lineal.
static GLfloat* bufferHermite = NULL;
static int c = 0;
static int keyVelocity = 0;
static double lambda_V = 0;
static double lambda_new = 0;

using namespace util;
// Metodos constructores 

igvEscena3D::igvEscena3D() {
	ejes = true;
	pause = false;
	lineal = false;
	trayec = true;
	video = false;
	LoadInputs();
	LoadSpeed();
}

igvEscena3D::~igvEscena3D() {}

// Metodos publicos 

void pintar_ejes(void) {
	glLineWidth(2);
	GLfloat rojo[] = { 1,0,0,1.0 };
	GLfloat verde[] = { 0,1,0,1.0 };
	GLfloat azul[] = { 0,0,1,1.0 };

	glBegin(GL_LINES);
	glMaterialfv(GL_FRONT, GL_EMISSION, rojo);
	glVertex3f(2000, 0, 0);
	glVertex3f(-2000, 0, 0);

	glMaterialfv(GL_FRONT, GL_EMISSION, verde);
	glVertex3f(0, 2000, 0);
	glVertex3f(0, -2000, 0);

	glMaterialfv(GL_FRONT, GL_EMISSION, azul);
	glVertex3f(0, 0, 2000);
	glVertex3f(0, 0, -2000);
	glEnd();
}

void pintarPuntos() {
	GLfloat color_morado[] = { 0.5f,0,0.5f };
	glMaterialfv(GL_FRONT, GL_EMISSION, color_morado);
	glPointSize(8.0);
	for (int i = 0; i < util::TAM; i++) {
		glBegin(GL_POINTS);
		glVertex3f(points[i].x, points[i].y, points[i].z);
		glEnd();
	}
}

void pintarTrayectoria() {
	pintarPuntos();
	GLfloat color_verde[] = { 0.3f, 1.0f, 0.6f };
	glMaterialfv(GL_FRONT, GL_EMISSION, color_verde);
	
	for (int i = 0; i < util::TAM - 1; i++) {
		glBegin(GL_LINE_STRIP);
		for (double lambd = 0; lambd <= 1.01; lambd += 0.01)
		{
			Point3D r2;
			makeLerp(points[i], points[i + 1], r2, lambd);
			glVertex3f(r2.x, r2.y, r2.z);
		}
		glEnd();
	}
}

void pintarTrayectoriaHermite() {
	pintarPuntos();
	GLfloat color_negro[] = { 0,0,0 };
	glMaterialfv(GL_FRONT, GL_EMISSION, color_negro);
	glLineWidth(2);

	if (bufferHermite == NULL) {
		bufferHermite = new GLfloat[3 * 101 * (TAM - 1)];
		c = 0;
		for (int i = 0; i < util::TAM - 1; i++) {
			for (double lambd = 0; lambd <= 1.01; lambd += 0.01)
			{
				int ini, end;
				if (i == 0) ini = i;
				else ini = i - 1;
				if (i + 1 == TAM - 1) end = TAM - 1;
				else end = i + 2;
				Point3D r2 = HermiteInterpolate(points[ini], points[i], points[i + 1], points[end], lambd, 0, 0);
				bufferHermite[c++] = r2.x;
				bufferHermite[c++] = r2.y;
				bufferHermite[c++] = r2.z;
			}
		}
	}
	else {
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, bufferHermite);
		glDrawArrays(GL_LINE_STRIP, 0, 101 * (TAM - 1));
		glDisableClientState(GL_VERTEX_ARRAY);
	}
}


void igvEscena3D::interpolacionCurva() {
	static int ini2 = 0, end2 = 0, ini = 0, end = 0;

	if (lambda >= 1) {
		lambda = 0;
		if (keyFrame + 2 < util::TAM) keyFrame++;
		else {
			keyFrame = 0;
			lambda_V = 0;
			keyVelocity = 0;
			lambda_new = 0;
			ini = 0, end = 0;
		}
		if (keyFrame == 0) ini = keyFrame;
		else ini = keyFrame - 1;
		if (keyFrame + 1 == TAM - 1) end = TAM - 1;
		else end = keyFrame + 2;
	};

	/*---------Velocidad de curva---------*/
	if (((lambda_V >= rate[keyVelocity]) || (end2 == 0)) && (rate[keyVelocity] != 1)) {
		lambda_new = 0;
		if (keyVelocity + 1 < TAM_V) keyVelocity++;
		else keyVelocity = 0;

		if (keyVelocity != 1) ini2 = keyVelocity - 2;
		if (keyVelocity + 1 >= TAM_V - 1) end2 = TAM_V - 1;
		else end2 = keyVelocity + 1;
	}
	if (!pause) lambda_V +=  1 / (kFrames[TAM-1] - kFrames[0]);
	
	int a;
	if (keyVelocity == 0) a = 0;
	else a = keyVelocity - 2;
	lambda_new = (lambda_V - rate[a]) / (rate[keyVelocity] - rate[a]);

	double h = HermiteInterpolate(velocity[ini2], velocity[keyVelocity - 1], velocity[keyVelocity], velocity[end2], lambda_new, 0, 0);

	/*---------Curva---------*/
	/*Trayectoria a seguir*/
	if (trayec && lineal) pintarTrayectoria();
	if (trayec && !lineal) pintarTrayectoriaHermite();
	if (!pause) lambda += 2 * h / (kFrames[keyFrame + 1] - kFrames[keyFrame]);
	r = HermiteInterpolate(points[ini], points[keyFrame], points[keyFrame + 1], points[end], lambda, 0, 0);
}

void igvEscena3D::interpolacionLineal() {
	/*---------LERP---------*/
	/*Trayectoria a seguir*/

	if (trayec) pintarTrayectoria();
	if (lambda > 1) {
		lambda = 0;
		if (keyFrame + 2 < util::TAM) keyFrame++;
		else keyFrame = 0;
	};
	if (!pause) lambda += 1.0 / (kFrames[keyFrame + 1] - kFrames[keyFrame]);


	makeLerp(points[keyFrame], points[keyFrame + 1], r, lambda);
}

void igvEscena3D::interpolacionEsferica() {
	/*---------SLERP---------*/
	static float rate = 0.3; //Porción de la trayectoria en el que comenzará a realizar la interpolación de la orientación (SLERP).

	float w = CalculateAngle(points[keyFrame], points[keyFrame + 1]);
	Point3D rot(0, 1, 1);

	Quaternion quatR(w, rot);
	quatR.Normalize();
	quatR.ExportToMatrix(m); //Quaternion con la orientacion entre keyFrame y keyFrame+1.

	if ((keyFrame + 2 < util::TAM) && (lambda > rate)) {
		float w2 = CalculateAngle(points[keyFrame + 1], points[keyFrame + 2]);
		Quaternion quatR2(w2, rot);
		quatR2.Normalize();
		Quaternion quatR3;
		quatR3.w = acos(kFrames[keyFrame] * kFrames[keyFrame + 1]);

		static float t = 0;
		if (t > 1) t = 0;
		t = (lambda - rate) / (1 - rate);

		makeSlerp(quatR, quatR2, quatR3, t); //Rotación Slerp entre el q y p.
		quatR3.ExportToMatrix(m);
	}
}

void escaladoNoUniforme() {
	/*--------ESCALADO NO-UNIFORME--------*/

	static float rateScale = 0.3; //Porción de la trayectora en la que comenzará el objeto a realizar el escalado no-uniforme.
								  /*Interpolación entre los valores de escalado r1 y  r2*/
	Point3D r1(1.1, 0.8, 1.1);
	Point3D r2(0.8, 1.1, 0.8);
	Point3D rR;

	if (lambda < rateScale) makeLerp(r1, r2, rR, lambda / rateScale);
	else makeLerp(r2, r1, rR, (lambda - rateScale) / (1 - rateScale));

	glScaled(rR.x, rR.y, rR.z);
}




void igvEscena3D::visualizar(void) {
	// crear luces
	GLfloat luz0[] = { 1,1,1,1 }; // luz puntual
	glLightfv(GL_LIGHT0, GL_SPECULAR, luz0);
	glEnable(GL_LIGHT0);
	// crear el modelo
	glPushMatrix(); // guarda la matriz de modelado

	GLfloat color_rojo[] = { 1,0,0 };
	GLfloat color_azul[] = { 0,0,1 };
	GLfloat color_negro[] = { 0,0,0 };
	
	//Ajuste para velocidad de grabacion.
	/*if (video) for (int i = 0; i < TAM_V; i++) {
		video = false;
		velocity[i] *= 2;
	}*/
	// se pintan los ejes
	if (ejes) pintar_ejes();


	if (input) { //Recargar parametros de entrada.
		input = false;
		LoadInputs();
		LoadSpeed();
		bufferHermite = NULL;
	}
	static int currentFrame = 0;

	static auto beginTime = chrono::high_resolution_clock::now();
	
	auto duracion = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - beginTime);
	cout << " (" << duracion.count() << " mili)." << endl;
	beginTime = chrono::high_resolution_clock::now();
	
	int sleepTime = (int)((1000/60) - duracion.count());

	if (sleepTime > 0) {
		Sleep(sleepTime);
	}

	if (lineal) interpolacionLineal(); //Funcion LERP.
	else interpolacionCurva(); //Funcion curva con Hermite.
	interpolacionEsferica(); //Funcion SLERP.
	m[12] = r.x; m[13] = r.y; m[14] = r.z; //Traslación desde la matriz.

	glPushMatrix();
	glMultMatrixf(m);
		escaladoNoUniforme(); //Funcion de escalado no uniforme.
		glMaterialfv(GL_FRONT, GL_EMISSION, color_negro);
		glutSolidTeapot(1); //Visualización del modelo.
	glPopMatrix();
	
	glPopMatrix();
}                                                                                                                                                                                                                                                                                                                                                                                      