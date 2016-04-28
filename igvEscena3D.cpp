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


float limits = 2;
float X = 0, Y = 0;
const int MAX_PARTICLES = 500;
const int MIN_PARTICLES = 1;
int currentParticle = 1;
float posX[MAX_PARTICLES], posY[MAX_PARTICLES], tempC[MAX_PARTICLES];

void moveParticles(int amount_of_particles) {
	srand(time(NULL));
	float myX, myY;
	Sleep(1);
	//glColor3d(2, 0.5, 0);
	for (int i = 0; i < amount_of_particles; i++) {
		myX = rand() % 3 + 1;
		if (myX == 1 && posX[i] <= limits) {
			int mytemp = rand() % 100 + 1;
			int temp = rand() % 5 + 1;
			if (rand() % 100 > 80)
				temp *= -1;
			posX[i] -= temp*.001;
			posY[i] += mytemp*0.0001;
			tempC[i] = temp*.1;
		}
		if (myX == 2) { posX[i] += .00; posY[i] += .01; }
		if (myX == 3 && posX[i] >= -limits) {
			int temp = rand() % 5 + 1;
			int mytemp = rand() % 100 + 1;
			posX[i] -= temp*.001;
			posY[i] += mytemp*0.0002;
		}
		///////////////////////////////////////////
		if (posY[i] >= limits + 0.2 - (( rand() % 100) / 100)) {
			posY[i] = 0;
			posX[i] = 0;
		}
	}
}
void Reshape(int height, int width) {
	glViewport(0, 0, width, height);
	glClearColor(0, 0, 0, 1);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (float)height / (float)width, 1, 100);
	glMatrixMode(GL_MODELVIEW);
}
void DrawFire(void) {
	int thingy = 1;
	bool check = false;
	
	if (check == false) {
		float R, G, B;
		glPushMatrix();
		glBegin(GL_TRIANGLES);
		for (int i = 0; i < MAX_PARTICLES; i++) {
			/*R = rand() % 100 + 1;
			G = rand() % 100 + 1;
			B = rand() % 100 + 1;
			glColor3d(R*0.01, G*0.01, B*0.01);
			
			GLfloat color_rand[] = { R*0.01, G*0.01, B*0.01 };*/
			
			GLfloat color_a[] = { 1, 1, 0 };
			GLfloat color_r[] = { 1, tempC[i]*2, 0 };
			GLfloat color_n[] = { tempC[i]*2, tempC[i] / 1.1f, 0.0f };
		
			if (posY[i] >= 0) {
				if(tempC[i] < 0.1)
					glMaterialfv(GL_FRONT, GL_EMISSION, color_a);
				else
					glMaterialfv(GL_FRONT, GL_EMISSION, color_r);
			}
			if (posY[i] > 0.6) {
				if (tempC[i] < 0.4)
					glMaterialfv(GL_FRONT, GL_EMISSION, color_r);
				else 
					glMaterialfv(GL_FRONT, GL_EMISSION, color_n);
				posX[i] -= 0.004;
			}
			if(posY[i] > 1){
				glMaterialfv(GL_FRONT, GL_EMISSION, color_n);
				posX[i] -= 0.008;
			}
			glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
			float tamPoint = (limits + 0.5 - posY[i]) * 3;
			if (tamPoint < 0) tamPoint = 0;
			X = posX[i];
			Y = posY[i];
			glPointSize(tamPoint);

			glBegin(GL_POINTS);
				glVertex3f(X, Y, 0);
			glEnd();
			
		}
		glEnd();
		glPopMatrix();
		check = true;
	}
	switch (thingy) {
	case 1:
		Sleep(1);
		moveParticles(currentParticle);
		if (currentParticle != MAX_PARTICLES) {
			currentParticle++;
		}
		glutPostRedisplay();
		break;
	}
}
const GLfloat light_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };
const GLfloat mat_ambient[] = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };



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
	
	glPopMatrix();
}                              


