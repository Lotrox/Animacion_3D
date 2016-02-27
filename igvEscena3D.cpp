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
		glVertex3f(1000,0,0);
		glVertex3f(-1000,0,0);

    glMaterialfv(GL_FRONT,GL_EMISSION,verde);
		glVertex3f(0,1000,0);
		glVertex3f(0,-1000,0);

    glMaterialfv(GL_FRONT,GL_EMISSION,azul);
		glVertex3f(0,0,1000);
		glVertex3f(0,0,-1000);
	glEnd();
}

void pintar_tubo() {
  GLUquadricObj *tubo;
  GLfloat color_tubo[]={0,0,0.5};

  glMaterialfv(GL_FRONT,GL_EMISSION,color_tubo);
  
  tubo=gluNewQuadric();
  gluQuadricDrawStyle(tubo,GLU_FILL);

	glPushMatrix();
	glTranslatef(0,0,-0.5);
  gluCylinder(tubo,0.25,0.25,1,20,20);
	glPopMatrix();

  gluDeleteQuadric(tubo);
}



void igvEscena3D::visualizar(void) {

	// crear luces
	GLfloat luz0[] = { 10,8,9,1 }; // luz puntual
	glLightfv(GL_LIGHT0, GL_POSITION, luz0);
	glEnable(GL_LIGHT0);

	// crear el modelo
	glPushMatrix(); // guarda la matriz de modelado

	  // se pintan los ejes
	if (ejes) pintar_ejes();

	// se pintan los objetos de la escena
	GLfloat color_rojo[] = { 1,0,0 };
	GLfloat color_azul[] = { 0,0,1 };
	GLfloat color_negro[] = { 0,0,0 };

	glMaterialfv(GL_FRONT, GL_EMISSION, color_rojo);


	/*LERP STUFFs*/
	/*Recorrido a seguir*/
	glMaterialfv(GL_FRONT, GL_EMISSION, color_negro);
	glPointSize(1.0);
	Point3D r2;

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < util::TAM - 1; i++) {
		for (double lambd = 0; lambd <= 1.01; lambd += 0.01)
		{
			make_lerp(points[i], points[i + 1], r2, lambd);
			glVertex3f(r2.x, r2.y, r2.z);
		}
	}
	glEnd();
	static float lambda = 0;
	static int keyFrame = 0;
	Point3D r;

	if (lambda > 1) {
		lambda = 0;
		if (keyFrame + 2 < util::TAM) keyFrame++;
		else keyFrame = 0;
		//cout << "KeyFrame: " << keyFrame << "-" << keyFrame + 1 << endl;
	};
	lambda += 1.0 / (kFrames[keyFrame + 1] - kFrames[keyFrame]);
	make_lerp(points[keyFrame], points[keyFrame + 1], r, lambda);



	

	
	/*SLERP STUFFs*/
	float w = CalculateAngle(points[keyFrame], points[keyFrame + 1]);
	Point3D rotY, rotZ;
	rotY.x = 0; rotY.y = 1; rotY.z = 1;
	rotZ.x = 1; rotZ.y = 0; rotZ.z = 1;
	Quaternion quatR(w, rotY);
	quatR.Normalize();
	
	quatR.ExportToMatrix(m);

	float rate = 0.05; //Porcentaje en el que empezará a realizar la interpolación de la orientación (SLERP).
	if ((keyFrame + 2 < util::TAM) && (lambda > rate)) {
		float w2 = CalculateAngle(points[keyFrame + 1], points[keyFrame + 2]);

		Quaternion quatR2(w2, rotY);
		quatR2.Normalize();
		Quaternion quatR3;
		quatR3.w = acos(kFrames[keyFrame] * kFrames[keyFrame + 1]);
		
		static float t = 0;
		if (t > 1) t = 0;
		t = (lambda - rate)/(1 - rate);

	    makeSlerp(quatR, quatR2, quatR3, t);

		quatR3.ExportToMatrix(m);
	}

	m[12] = r.x; m[13] = r.y; m[14] = r.z;
	glPushMatrix();
		glMultMatrixf(m);
		glutSolidTeapot(1);
		//ant.visualizar(m, 0.5);
	glPopMatrix();
	

	glutPostRedisplay();
	
	
	/*for (int i = 0; i < util::TAM; i++) cout << points[i].x << " " << points[i].y << endl;

	glPointSize(5.0f);
	glBegin(GL_POINTS);
		for (int c = 1; c < util::TAM + 1; c++){
			glMaterialfv(GL_FRONT, GL_EMISSION, color_rojo);
			glVertex3f(points[c - 1].x, points[c - 1].y, points[c - 1].z);
			for (float i = points[c-1].x; i < points[c].x; i += 0.02) {
				Point il = util::interpolacionLineal(i, points[c-1], points[c]);
				//std::cout << il.x << " " << il.y << std::endl;
				glMaterialfv(GL_FRONT, GL_EMISSION, color_azul);
				glVertex3f(il.x, il.y, il.z);
			}
		}
	glEnd();*/

	glPopMatrix (); // restaura la matriz de modelado
}

