#include <cstdlib>
#include <stdio.h>
#include <vector>
#include <iostream>
#include "igvEscena3D.h"

// Metodos constructores 

igvEscena3D::igvEscena3D () {ejes = true;}

igvEscena3D::~igvEscena3D() {}

struct Point {
	float x, y;
};
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

Point interpolacionLineal(float x, Point a, Point b) {
	Point s;
	s.x = x;
	s.y = a.y + ((b.y - a.y) / (b.x - a.x))*(s.x - a.x);
	return s;
}

void igvEscena3D::visualizar(void) {
	
	/*Se crean las dos ecuaciones y planos de recorte.*/
	  GLdouble pl1[]={-1, 0, 0, 0.001};
	  GLdouble pl2[]={ 0, 0, 1, 0.001};
	  glClipPlane(GL_CLIP_PLANE0, pl1);
	  glClipPlane(GL_CLIP_PLANE1, pl2);
	
	
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
    
	
	std::vector<Point> puntos(10);
	puntos[0].x = -1; puntos[0].y = 2;
	puntos[1].x = 0.2; puntos[1].y = 0.5;
	puntos[2].x = 1; puntos[2].y = -1;
	puntos[3].x = 2; puntos[3].y = 1;

	glPointSize(5.0f);
	glBegin(GL_POINTS);
		for (int c = 1; c < 5; c++){
			glMaterialfv(GL_FRONT, GL_EMISSION, color_rojo);
			glVertex2f(puntos[c-1].x, puntos[c-1].y);
			for (float i = puntos[c-1].x; i < puntos[c].x; i += 0.1) {
				Point il = interpolacionLineal(i, puntos[c-1], puntos[c]);
				//std::cout << il.x << " " << il.y << std::endl;
				glMaterialfv(GL_FRONT, GL_EMISSION, color_azul);
				glVertex2f(il.x, il.y);
			}
		}
	glEnd();

	glPopMatrix (); // restaura la matriz de modelado
}

