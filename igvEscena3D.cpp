#include <cstdlib>
#include <stdio.h>
#include <vector>
#include <iostream>
#include "igvEscena3D.h"

#define M_PI 3.14159265
#define toRad(x) ((x)*(M_PI/180.0))

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

static float rate = 0.3; //Porción de la trayectoria en el que comenzará a realizar la interpolación de la orientación (SLERP).
static float rateScale = 0.8; //Porción de la trayectora en la que comenzará el objeto a realizar el escalado no-uniforme.

void igvEscena3D::visualizar(void) {
	// crear luces
	GLfloat luz0[] = { 10,8,9,1 }; // luz puntual
	glLightfv(GL_LIGHT0, GL_POSITION, luz0);
	glEnable(GL_LIGHT0);

	// crear el modelo
	glPushMatrix(); // guarda la matriz de modelado

	// se pintan los ejes
	if (ejes) pintar_ejes();

	if (input) {
		input = false;
		LoadInputs();
	}
	// se pintan los objetos de la escena
	GLfloat color_rojo[] = { 1,0,0 };
	GLfloat color_azul[] = { 0,0,1 };
	GLfloat color_negro[] = { 0,0,0 };

	//glMaterialfv(GL_FRONT, GL_EMISSION, color_rojo);
	static float lambda = 0;
	static int keyFrame = 0;
	

	/*---------LERP---------*/
	/*Trayectoria a seguir*/	
	glMaterialfv(GL_FRONT, GL_EMISSION, color_negro);
	if (trayec) {
		for (int i = 0; i < util::TAM - 1; i++) {
			glBegin(GL_LINE_STRIP);
			for (double lambd = 0; lambd <= 1.01; lambd += 0.01)
			{
				Point3D r2;
				make_lerp(points[i], points[i + 1], r2, lambd);
				glVertex3f(r2.x, r2.y, r2.z);
			}
			glEnd();
		}
	}

	if (lambda > 1) {
		lambda = 0;
		if (keyFrame + 2 < util::TAM) keyFrame++;
		else keyFrame = 0;
	//	cout << "KeyFrame: " << keyFrame << "-" << keyFrame + 1 << endl;
	};
	if(!pause) lambda += 1.0 / (kFrames[keyFrame + 1] - kFrames[keyFrame]);
	
	Point3D r;
	make_lerp(points[keyFrame], points[keyFrame + 1], r, lambda);
	
	/*---------SLERP---------*/
	float w = CalculateAngle(points[keyFrame], points[keyFrame + 1]);
	Point3D rotY, rotZ;
	rotY.x = 0; rotY.y = 1; rotY.z = 1;
	rotZ.x = 1; rotZ.y = 0; rotZ.z = 1;
	Quaternion quatR(w, rotY);
	quatR.Normalize();
	quatR.ExportToMatrix(m); //Quaternion con la orientacion entre keyFrame y keyFrame+1.

	if ((keyFrame + 2 < util::TAM) && (lambda > rate)) { 
		float w2 = CalculateAngle(points[keyFrame + 1], points[keyFrame + 2]);
		Quaternion quatR2(w2, rotY);
		quatR2.Normalize();
		Quaternion quatR3;
		quatR3.w = acos(kFrames[keyFrame] * kFrames[keyFrame + 1]);
		
		static float t = 0;
		if (t > 1) t = 0;
		t = (lambda - rate)/(1 - rate);

	    makeSlerp(quatR, quatR2, quatR3, t); //Rotación Slerp entre el q y p.
		quatR3.ExportToMatrix(m);
	}

	m[12] = r.x; m[13] = r.y; m[14] = r.z; //Traslación desde la matriz.

	glPushMatrix();
		glMultMatrixf(m);
		
		/*--------ESCALADO NO-UNIFORME--------*/
		/*Interpolación entre los valores de escalado r1 y  r2*/
		Point3D r1, r2, rR;
		r1.x = 1.1; r1.y = 0.8; r1.z = 1.1;
		r2.x = 0.9; r2.y = 1.2; r2.z = 0.9;

		if(lambda < rateScale) make_lerp(r1, r2, rR, lambda/ rateScale);
		else make_lerp(r2, r1, rR, (lambda - rateScale)/(1 - rateScale));
		glScaled(rR.x, rR.y, rR.z);
		glutSolidTeapot(1);
	glPopMatrix();

	glutPostRedisplay();
	glPopMatrix (); 
}

