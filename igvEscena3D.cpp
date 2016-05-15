#include <cstdlib>                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         #include <cstdlib>
#include <stdio.h>
#include <vector>
#include <iostream>
#include "igvEscena3D.h"
#include <thread>
#include <chrono>
#include "igvTextura.h"
#include <time.h>
#include <cmath>
#include <Windows.h>
#include "Particles.h"

#define M_PI 3.14159265

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

igvEscena3D::~igvEscena3D() {
}


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

void materialNone() {
	GLfloat ninguno[] = { 1,1,1,1 };
	glMaterialfv(GL_FRONT, GL_EMISSION, ninguno);
}




void pintar_cuerpo() {
	GLfloat rojo[] = { 1,0,0 };
	GLfloat amarillo[] = { 1,1,0 };

	glMaterialfv(GL_FRONT, GL_EMISSION, rojo);
	glPushMatrix();
	glScalef(1, 3.5, 2);
	glutSolidCube(1);
	glPopMatrix();
}

void pintar_cabeza() {
	GLfloat azul[] = { 0,0,1 };
	GLfloat negro[] = { 0,0,0 };
	GLfloat amarillo[] = { 1,1,0 };

	glPushMatrix();
	glutSolidSphere(0.9, 20, 20);
	glPopMatrix();

	glMaterialfv(GL_FRONT, GL_EMISSION, azul);
	glTranslatef(0.7, 0.4, 0.4);
	glutSolidSphere(0.3, 15, 15);

	glMaterialfv(GL_FRONT, GL_EMISSION, azul);
	glTranslatef(0, 0, -0.8);
	glutSolidSphere(0.3, 15, 15);

	GLUquadricObj *cuello = gluNewQuadric();
	gluQuadricDrawStyle(cuello, GLU_FILL);

	glTranslatef(-0.7, 0, 0.4);
	glRotatef(90, 1, 0, 0);
	gluCylinder(cuello, 0.3, 0.2, 3, 20, 20);

	gluDeleteQuadric(cuello);
}


void pintar_hombros() {
	GLUquadricObj *hombros = gluNewQuadric();
	gluQuadricDrawStyle(hombros, GLU_FILL);

	glPushMatrix();
	glTranslatef(0, 0, -2);
	gluCylinder(hombros, 0.4, 0.4, 3, 20, 20);
	glPopMatrix();

	gluDeleteQuadric(hombros);
}

void pintar_brazo() {

	GLfloat negro[] = { 0,0,0 };
	GLfloat azul[] = { 0,0,1 };

	GLUquadricObj *brazo = gluNewQuadric();
	gluQuadricDrawStyle(brazo, GLU_FILL);
	glPushMatrix();
	//glTranslatef(0, 0, -1);
	gluCylinder(brazo, 0.2, 0.2, 2, 20, 20);
	glPopMatrix();

	gluDeleteQuadric(brazo);
}

void pintar_pierna() {
	GLfloat negro[] = { 0,0,0 };
	GLUquadricObj *pierna = gluNewQuadric();
	gluQuadricDrawStyle(pierna, GLU_FILL);

	glPushMatrix();
	glTranslatef(0, 1.5, 0);
	glRotatef(90, 1, 0, 0);
	gluCylinder(pierna, 0.2, 0.2, 1.5, 20, 20);
	glPopMatrix();

	gluDeleteQuadric(pierna);
}


static float cuerpo = 0;

static float cabezaEjeY = 0;
static float cabezaEjeZ = 0;

static float piernaDerecha = 0;
static float piernaIzquierda = 0;
static float brazoI = 0;
static float brazoD = 0;
static float brazo2D = 0;

void pintarFigura() {
	GLfloat azul[] = { 0,0,1 };
	GLfloat amarillo[] = { 1,1,0 };
	GLfloat negro[] = { 0,0,0 };

	glInitNames();
	/*Cuerpo*/
	glRotatef(cuerpo, 0, 1, 0);
	glTranslatef(0, 0.5, 0);
	pintar_cuerpo();

	/*Cabeza y cuello*/
	glRotatef(180, 0, 1, 0);
	glPushMatrix();
	glRotatef(cabezaEjeY, 0, 1, 0);
	glRotatef(cabezaEjeZ, 0, 0, 1);
	glTranslatef(0, 3, 0);
	pintar_cabeza();
	glPopMatrix();


	/*Brazo y mano derecha*/
	glPushMatrix();
		glTranslatef(0, 1, 0.5);
		pintar_hombros();
	
		glPushMatrix();
			
			glRotatef(brazoD, 0, 0, 1);
			//glRotatef(80, 1, 0, 0);
			glTranslatef(-1, 1, 0.7f);
			glMaterialfv(GL_FRONT, GL_EMISSION, negro);
			pintar_brazo();
			glTranslatef(0, 0, 1);
			glutSolidSphere(0.25, 20, 20);
			glTranslatef(0, 0, 0.5);
			//glRotatef(brazoD, 0, 0, 1);
			pintar_brazo();
			glTranslatef(0, 0, 1.3);
			glutSolidSphere(0.25, 20, 20);
		glPopMatrix();
	glPopMatrix();

	/*Brazo y mano izquierda*/
	glPushMatrix();
		glTranslatef(0, 0, -1.3);
		glRotatef(brazoI, 0, 0, 1);
		glRotatef(80, -1, 0, 0);
		glMaterialfv(GL_FRONT, GL_EMISSION, negro);
		pintar_brazo();
		glTranslatef(0, 0, -1);
		glutSolidSphere(0.25, 20, 20);
		glTranslatef(0, 0, -0.5);
		pintar_brazo();
		glTranslatef(0, 0, -1.3);
		glutSolidSphere(0.25, 20, 20);
	glPopMatrix();

	/*Pierna y pie derecha*/
	glPushMatrix();
	glTranslatef(0.1, -3, 0.5);
	glRotatef(piernaDerecha, 0, 0, 1);
	glMaterialfv(GL_FRONT, GL_EMISSION, negro);
	pintar_pierna();
	glScalef(2, 1, 1.5);
	glMaterialfv(GL_FRONT, GL_EMISSION, azul);
	glutSolidSphere(0.25, 20, 20);
	glPopMatrix();

	/*Pierna y pie izquierda*/
	glPushMatrix();
	glTranslatef(0.1, -3, -0.5);
	glRotatef(piernaIzquierda, 0, 0, 1);
	glMaterialfv(GL_FRONT, GL_EMISSION, negro);
	pintar_pierna();
	glScalef(2, 1, 1.5);
	glMaterialfv(GL_FRONT, GL_EMISSION, azul);
	glutSolidSphere(0.25, 20, 20);
	glPopMatrix();
}

void brazo(void) {
	GLfloat negro[] = { 0,0,0 };
	glMaterialfv(GL_FRONT, GL_EMISSION, negro);
	glRotatef(90, 0, 1, 0);
	glPushMatrix();

	glRotatef(brazo2D, 1, 0, 0);
	
	pintar_brazo();
	glTranslatef(0, 0, 2);
	glutSolidSphere(0.25, 20, 20);
	glPushMatrix();
	glRotatef(brazoD, 1, 0, 0);
	
	pintar_brazo();
	//glTranslatef(0, 0, 1.3);
	//glutSolidSphere(0.25, 20, 20);
	glPopMatrix();
	glPopMatrix();
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
	if (video) for (int i = 0; i < TAM_V; i++) {
		video = false;
		velocity[i] *= 1.5;
	}
	// se pintan los ejes
	if (ejes) pintar_ejes();

	static bool b = true;

	if (input) { //Recargar parametros de entrada.
		input = false;
		LoadInputs();
		LoadSpeed();
		bufferHermite = NULL;
	}
	static int currentFrame = 0;

	static auto beginTime = chrono::high_resolution_clock::now();

	auto duracion = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - beginTime);
	beginTime = chrono::high_resolution_clock::now();

	int sleepTime = (int)((1000 / 60) - duracion.count());


	if (sleepTime > 0) {
		Sleep(sleepTime);
	}

	if (lineal) interpolacionLineal(); //Funcion LERP.
	else interpolacionCurva(); //Funcion curva con Hermite.
	interpolacionEsferica(); //Funcion SLERP.
	m[12] = r.x; m[13] = r.y; m[14] = r.z; //Traslación desde la matriz.

	glPushMatrix();
	glMultMatrixf(m);
	//escaladoNoUniforme(); //Funcion de escalado no uniforme.
	glPushMatrix();
	materialNone();
	glTranslatef(1.55, 0.5, 0);
	DrawFire();
	materialNone();
	glPopMatrix();
	glMaterialfv(GL_FRONT, GL_EMISSION, color_negro);
	glEnable(GL_TEXTURE_2D);
	igvTextura tete("textures/tetera.bmp");
	tete.aplicar();
	glutSolidTeapot(1); //Visualización del modelo.
	materialNone();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	glEnable(GL_TEXTURE_2D);
	igvTextura background("textures/cielo.bmp");
	background.aplicar();
	glBegin(GL_QUADS);
	glTexCoord2f(1, 0);
	glVertex3f(-15.5, -20, -12);
	glTexCoord2f(1, 1);
	glVertex3f(-15.5, 20, -12);
	glTexCoord2f(0, 1);
	glVertex3f(50, 20, -12);
	glTexCoord2f(0, 0);
	glVertex3f(50, -20, -12);
	glEnd();
	glBegin(GL_QUADS);
	glTexCoord2f(1, 0);
	glVertex3f(-15, -20, -12.5);
	glTexCoord2f(1, 1);
	glVertex3f(-15, 20, -12.5);
	glTexCoord2f(0, 1);
	glVertex3f(-15, 20, 50);
	glTexCoord2f(0, 0);
	glVertex3f(-15, -20, 50);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	//glRotatef(90, 0, 1, 0);
	//glTranslated(0, 2, 0);

	glPopMatrix();

	float x = 0.8; float y = 2;
	glBegin(GL_POINTS);
	glPointSize(18.0);
	glVertex3f(x, y, 0);
	glEnd();
	
	
	float angle_O = acos((x*x + y*y - 2 * 2 - 2 * 2) / (2.0f * 2 * 2));
	float angle = angle_O * 180.0 / M_PI;

	cout << angle << endl;
	if (angle > 0) {
		if (brazoD <= angle) 
			brazoD += 0.2f;
	}else {
		if (brazoD >= angle)
			brazoD -= 0.2f;
	}

	float angle2 = atan(- (2 * sin(angle_O))*x + (2 + 2 * cos(angle_O)*y) / ( (2 * sin(angle_O))*y + (2 + 2 * cos(angle_O))*x ));
	angle2 = angle2 * 180.0 / M_PI;
	if (angle2 < 0)
		angle2 = angle2 - 90;
	cout << angle2 << endl;
	if (angle2 > 0) {
		if (brazo2D <= angle2)
			brazo2D += 0.2f;
	}
	else {
		if (brazo2D >= angle2)
			brazo2D -= 0.2f;
	}
	
	brazo();
}                              


