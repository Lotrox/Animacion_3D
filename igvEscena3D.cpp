#include <cstdlib>
#include <stdio.h>
#include <vector>
#include <iostream>
#include "igvEscena3D.h"

static int keyFrame = 0; //Fotograma clave actual.
static float lambda = 0; //Valor entre 0 y 1 de la interpolación lineal.
static Point3D r; //Punto resultante de la interpolación lineal.

using namespace util;
// Metodos constructores 

igvEscena3D::igvEscena3D () {
	ejes = true;
	pause = false;
	trayec = true;
	LoadInputs();
}

igvEscena3D::~igvEscena3D() {}

// Metodos publicos 

void pintar_ejes(void) {
  GLfloat rojo[]={1,0,0,1.0};
  GLfloat verde[]={0,1,0,1.0};
  GLfloat azul[]={0,0,1,1.0};

	glBegin(GL_LINES);
    glMaterialfv(GL_FRONT,GL_EMISSION,rojo);
		glVertex3f(2000,0,0);
		glVertex3f(-2000,0,0);

    glMaterialfv(GL_FRONT,GL_EMISSION,verde);
		glVertex3f(0,2000,0);
		glVertex3f(0,-2000,0);

    glMaterialfv(GL_FRONT,GL_EMISSION,azul);
		glVertex3f(0,0,2000);
		glVertex3f(0,0,-2000);
	glEnd();
}

void  pintarTrayectoria() {
	GLfloat color_negro[] = { 0,0,0 };
	glMaterialfv(GL_FRONT, GL_EMISSION, color_negro);
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

	static float rateScale = 0.8; //Porción de la trayectora en la que comenzará el objeto a realizar el escalado no-uniforme.
								  /*Interpolación entre los valores de escalado r1 y  r2*/
	Point3D r1(1.1, 0.8, 1.1);
	Point3D r2(0.9, 1.2, 0.9);
	Point3D rR;

	if (lambda < rateScale) makeLerp(r1, r2, rR, lambda / rateScale);
	else makeLerp(r2, r1, rR, (lambda - rateScale) / (1 - rateScale));

	glScaled(rR.x, rR.y, rR.z);
}

void igvEscena3D::visualizar(void) {
	// crear luces
	GLfloat luz0[] = { 10,8,9,1 }; // luz puntual
	glLightfv(GL_LIGHT0, GL_POSITION, luz0);
	glEnable(GL_LIGHT0);

	// crear el modelo
	glPushMatrix(); // guarda la matriz de modelado

	GLfloat color_rojo[] = { 1,0,0 };
	GLfloat color_azul[] = { 0,0,1 };
	GLfloat color_negro[] = { 0,0,0 };

	// se pintan los ejes
	if (ejes) pintar_ejes();

	if (input) { //Recargar inputs si se pulsa la tecla
		input = false;
		LoadInputs();
	}
	
	interpolacionLineal(); //Funcion LERP.
	interpolacionEsferica(); //Funcion SLERP.
	
	m[12] = r.x; m[13] = r.y; m[14] = r.z; //Traslación desde la matriz.

	glPushMatrix();

	glMultMatrixf(m);
	escaladoNoUniforme(); //Funcion de escalado no uniforme.
	glutSolidTeapot(1);

	glPopMatrix();
	glutPostRedisplay();

	glPopMatrix (); 
}

