#include <cstdlib>
#include <stdio.h>
#include <vector>
#include <iostream>
#include "igvEscena3D.h"

#define M_PI 3.14159265
#define toRad(x) ((x)*(M_PI/180.0))
// Metodos constructores 

igvEscena3D::igvEscena3D () {
	ejes = true;
	util::inputPoints(points, frames);
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
  GLfloat luz0[]={10,8,9,1}; // luz puntual
  glLightfv(GL_LIGHT0,GL_POSITION,luz0);
  glEnable(GL_LIGHT0);

	// crear el modelo
	glPushMatrix(); // guarda la matriz de modelado

	  // se pintan los ejes
	  if (ejes) pintar_ejes();

		// se pintan los objetos de la escena
    GLfloat color_rojo[]={1,0,0};
	GLfloat color_azul[] = {0,0,1};

	
	glMaterialfv(GL_FRONT, GL_EMISSION, color_rojo);


	GLfloat m[16];
	double s = 1.0;
	
	
	static float lambda = 0;
	static int keyFrame = 0;
	Point3D r;
	static Slerp sl(points[keyFrame], points[keyFrame + 1]);

	
	if (lambda > 1) {
		lambda = 0;
		if (keyFrame + 2  < util::TAM) keyFrame++;
		else keyFrame = 0;
		cout << "Frame: " << keyFrame << "-" << keyFrame + 1 << endl;
		
	};
	lambda += 1.0 / (frames[keyFrame + 1] - frames[keyFrame]);
	make_lerp(points[keyFrame], points[keyFrame+1], r, lambda);
	ant.trasladarCuerpo(r.x, r.y, r.z);
	ant.visualizar(0.1);
	
	/*if (sR) {
		sl.randPoints();
		cout << "P(" << sl.p.x << ", " << sl.p.x << ", " << sl.p.z << ")" << endl;
		cout << "Q(" << sl.q.x << ", " << sl.q.x << ", " << sl.q.z << ")" << endl;
		cout << endl;
		sl.lambdaanim = 0;
		slerpRand();
	}


	glColor3f(0, 0, 1);
	glPointSize(10);
	glBegin(GL_POINTS);
	glVertex3f(sl.p.x, sl.p.y, sl.p.z);
	glEnd();

	glColor3f(1, 0, 1);
	glBegin(GL_POINTS);
	glVertex3f(sl.q.x, sl.q.y, sl.q.z);
	glEnd();

	Quaternion quatp, quatq, quatr;

	quatp.u = sl.p; quatp.w = 0;
	quatq.u = sl.q; quatq.w = 0;
	//glutSolidSphere(0.99, 32, 32);
	glBegin(GL_LINE_STRIP);
	for (double lambda = 0; lambda <= 1.01; lambda += 0.01)
	{
		//glLoadIdentity();
		//Quaternion rp = RotateAboutAxis(a, 2.0*lambda*M_PI / 1000.0, b);
		sl.makeSlerp(quatp, quatq, quatr, lambda);
		quatr.ExportToMatrix(m);
		MultiplyPointMatrix(m, sl.p, sl.Rp);
		glVertex3f(s*sl.Rp.x, s*sl.Rp.y, s*sl.Rp.z);
	}
	glEnd();

	sl.lambdaanim += 0.0001;
	if (sl.lambdaanim>1.0) sl.lambdaanim = 0.0;

	sl.makeSlerp(quatp, quatq, quatr, sl.lambdaanim);

	//glLoadIdentity();
	double angle = 20;
	//Quaternion rp = RotateAboutAxis(a, toRad(angle), b);
	quatr.ExportToMatrix(m);
	MultiplyPointMatrix(m, sl.p, sl.Rp);
	glMaterialfv(GL_FRONT, GL_EMISSION, color_rojo);
	glPointSize(5);
	
	ant.trasladarCuerpo(s*sl.Rp.x, s*sl.Rp.y, s*sl.Rp.z);
	ant.visualizar(0.1);

	glBegin(GL_POINTS);
		glVertex3f(s*sl.Rp.x, s*sl.Rp.y, s*sl.Rp.z);
	glEnd();*/

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

