#include <cstdlib>
#include <stdio.h>
#include <vector>
#include <iostream>
#include "igvEscena3D.h"
#include "Util.h"

// Metodos constructores 

igvEscena3D::igvEscena3D () {ejes = true;}

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
    

	Point* points = util::inputPoints();
	for (int i = 0; i < util::TAM; i++) cout << points[i].x << " " << points[i].y << endl;

	/*points[0].x = -2; points[0].y = 1.2;
	points[1].x = 0.2; points[1].y = 0.8;
	points[2].x = 1; points[2].y = -1.8;
	points[3].x = 2; points[3].y = -0.1;*/

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
	glEnd();

	glPopMatrix (); // restaura la matriz de modelado
}

