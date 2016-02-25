#define _USE_MATH_DEFINES
#include <cmath>

#include <cstdlib>
#include <stdio.h>

#include "Hormiga.h"

  
/// HORMIGA

namespace HORMIGA {
    /**
     * @brief Tipo enumerado para identificar los grados de libertad del modelo.
     */
	enum {
		CUERPO_RY,
		CUERPO_TX,
		CUERPO_TZ,
		CUERPO_TY,
        CABEZA,
        PINZAS,
		ANTENAS,
        PATADelantera,
        PATAMedia,
        PATATrasera
    };
};

#include <stack>

// Metodos constructores

void colorNegro(){
    GLfloat negro[]={0.15,0.0,0.0,1};
    glMaterialfv(GL_FRONT,GL_EMISSION,negro);
    glMaterialfv(GL_FRONT,GL_DIFFUSE,negro);
}


void colorGris(){
    GLfloat gris[] = {0.00002,0.000002,0.000002,1};
    glMaterialfv(GL_FRONT,GL_EMISSION,gris);
}

/*Primitivas*/

void cilindro() {
    GLUquadricObj *parte=gluNewQuadric();
    
    glPushMatrix();
    gluCylinder(parte,0.06,0.07,1.2,15,15);
    glPopMatrix();
    
}

void cuerpo() {
    // Cuerpo
    glPushMatrix();
    glScalef(3.2,1,1);
    glutSolidSphere(0.5,40,40);
    glPopMatrix();
    
    // Cola
    glPushMatrix();
    glRotatef(15,0,0,1);
    glTranslatef(-1.8,0.5,0);
    glScalef(1.5,1,1);
    glutSolidSphere(0.8,40,40);
    glPopMatrix();
}

void cabeza () {
    // Cabeza
    glPushMatrix();
    glRotatef(15,0,0,1);
    glScalef(1.5,1,1);
    glTranslatef(1,-0.5,-1);
    glutSolidSphere(0.5,40,40);
    glPopMatrix();
    
    // Ojos
    GLfloat gris[]={0.5,0.5,0.5};
    glMaterialfv(GL_FRONT,GL_EMISSION,gris);
    glMaterialfv(GL_FRONT,GL_DIFFUSE,gris);
    
    glPushMatrix();
    glTranslatef(1.3,0,-0.6);
    glScalef(1.5,1,1);
    glutSolidSphere(0.15,40,40);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(1.3,0,-1.4);
    glScalef(1.5,1,1);
    glutSolidSphere(0.15,40,40);
    glPopMatrix();
}

/*Trasformaciones*/

void t_cuerpo(const float* f) {
	glTranslatef(f[HORMIGA::CUERPO_TX],f[HORMIGA::CUERPO_TY],f[HORMIGA::CUERPO_TZ]);
	glRotatef(f[HORMIGA::CUERPO_RY],0,1,0);
}

void t_cabeza(const float* f){
    glRotatef(f[HORMIGA::CABEZA],0,1,0);
    glTranslatef(3.5,0,-1);
    glRotatef(180,0,1,0);
}


void t_antenaD(const float* f) {
	glRotatef(f[HORMIGA::ANTENAS],1,0,0);
    glRotatef(90,0.4,1,0);
    glTranslatef(1.4,0.2,0);
}

void t_antenaI(const float* f) {
	glRotatef(f[HORMIGA::ANTENAS],-1,0,0);
    glRotatef(90,0.4,1,0);
    glTranslatef(0.9,0.4,0);
}

void t_pinzaInteriorD(const float* f) {
    glTranslatef(0.7,-0.3,-1.2);
    glRotatef(90,0,1,0);
    glScalef(0.7,0.7,0.3); 
}
void t_pinzaInteriorI(const float* f) {
    glTranslatef(0.7,-0.3,-0.7);
    glRotatef(90,0,1,0);
    glScalef(0.7,0.7,0.3);
    
}
void t_pinzaExeriorD(const float* f) {
    glRotatef(f[HORMIGA::PINZAS],0,1,0);
    glTranslatef(-0.3,0,0);
    glRotatef(90,0,1,0);
    glScalef(0.9,0.9,0.3);
}
void t_pinzaExteriorI(const float* f) {
    glRotatef(f[HORMIGA::PINZAS],0,-1,0);
    glTranslatef(0.3,0,0);
    glRotatef(90,0,-1,0);
    glScalef(0.9,0.9,0.3);
}

/*Patas*/
void t_pataBaseD(const float* f){ /*Pata 1 Derecha*/
    glRotatef(f[HORMIGA::PATADelantera],0,1,0);
    glRotatef(10,0,1,0);
    glRotatef(45,1,0,0);
    glTranslatef(0.9,0,0.4);
    glScalef(1.2,1.2,0.8);
}

void t_pataBaseI(const float* f){ /*Pata 2 Izquierda*/
	glRotatef(2*f[HORMIGA::PATADelantera],0,1,0);
    glScalef(1,1,-1);
    t_pataBaseD(f);
}
void t_pataBaseD2(const float* f){ /*Pata 3 Dereceha*/
    glRotatef(f[HORMIGA::PATAMedia],0,1,0);
    glRotatef(45,1,0,0);
    glTranslatef(0,-0.08,0.3);
    glScalef(1.2,1.2,0.8);
}
void t_pataBaseI2(const float* f){ /*Pata 4 Izquierda*/
	glRotatef(2*f[HORMIGA::PATAMedia],0,1,0);
    glScalef(1,1,-1);
    t_pataBaseD2(f);
}
void t_pataBaseD3(const float* f){ /*Pata 5 Dereceha*/
    glRotatef(f[HORMIGA::PATATrasera],0,1,0);
    glRotatef(-10,0,1,0);
    glRotatef(45,1,0,0);
    glTranslatef(-1,-0.05,0.3);
    glScalef(1.2,1.2,0.8);
}
void t_pataBaseI3(const float* f){ /*Pata 6 Izquierda*/
    glRotatef(2*f[HORMIGA::PATATrasera],0,1,0);
	glScalef(1,1,-1);
    t_pataBaseD3(f);
}

void t_pataPie(const float* f){ /*Exterior de pierna*/
    glTranslatef(0,0,1.1);
    glRotatef(30,1,0,0);
    glScalef(1,1,0.8);
}



Hormiga::Hormiga ():miHormiga(10) {
		
	miHormiga.setGL(HORMIGA::CUERPO_RY, 0);
	miHormiga.setGL(HORMIGA::CUERPO_TX, 0);
	miHormiga.setGL(HORMIGA::CUERPO_TY, 0);
	miHormiga.setGL(HORMIGA::CUERPO_TZ, 0);
    miHormiga.setGL(HORMIGA::CABEZA, 0);
    miHormiga.setGL(HORMIGA::PINZAS, 0);
    miHormiga.setGL(HORMIGA::PATADelantera, 0);
    miHormiga.setGL(HORMIGA::PATAMedia, 0);
    miHormiga.setGL(HORMIGA::PATATrasera, 0);
	miHormiga.setGL(HORMIGA::ANTENAS, 0);

	 void* r = miHormiga.insertar(t_cuerpo,colorNegro,cuerpo); /*Insertamos el cuerpo de la hormiga*/
		void* r1 = miHormiga.insertar(t_cabeza,0,cabeza,r); /*Insertamos la cabeza*/
					miHormiga.insertar(t_antenaD,colorGris,cilindro,r1); /*Insertamos antena derecha*/
					miHormiga.insertar(t_antenaI,0,cilindro,r1); /*Insertamos antena izquierda*/
					void* r2 = miHormiga.insertar(t_pinzaInteriorD,0,cilindro,r1); /*Insertamos pinza de la boca interior derecha*/
		miHormiga.insertar(t_pinzaExeriorD,0,cilindro,r2); /*Insertamos pinza de la boca exterior derecha*/
					r2 = miHormiga.insertar(t_pinzaInteriorI,0,cilindro,r1); /*Insertamos pinza de la boca interior izquierda*/
		miHormiga.insertar(t_pinzaExteriorI,0,cilindro,r2); /*Insertamos pinza de la boca exterior izquierda*/
		r1 = miHormiga.insertar(t_pataBaseD,0,cilindro,r); /*Insertarmos la pata 1 inferior*/
					miHormiga.insertar(t_pataPie,0,cilindro,r1); /*Insertamos la pata 1 exterior*/
		r1 = miHormiga.insertar(t_pataBaseI,0,cilindro,r); /*Insertarmos la pata 2 inferior*/
					miHormiga.insertar(t_pataPie,0,cilindro,r1); /*Insertamos la pata 2 exterior*/
		r1 = miHormiga.insertar(t_pataBaseD2,0,cilindro,r); /*Insertarmos la pata 3 inferior*/
					miHormiga.insertar(t_pataPie,0,cilindro,r1); /*Insertamos la pata 3 exterior*/
		r1 = miHormiga.insertar(t_pataBaseI2,0,cilindro,r); /*Insertarmos la pata 4 inferior*/
					miHormiga.insertar(t_pataPie,0,cilindro,r1); /*Insertamos la pata 4 exterior*/
		r1 = miHormiga.insertar(t_pataBaseD3,0,cilindro,r); /*Insertarmos la pata 5 inferior*/
					miHormiga.insertar(t_pataPie,0,cilindro,r1); /*Insertamos la pata 5 exterior*/
		r1 = miHormiga.insertar(t_pataBaseI3,0,cilindro,r); /*Insertarmos la pata 6 inferior*/
					miHormiga.insertar(t_pataPie,0,cilindro,r1); /*Insertamos la pata 6 exterior*/


}

Hormiga::~Hormiga() {
}


void Hormiga::visualizar(float* matrix, float size) {
    // crear luces
    GLfloat luz0[4]={5.0,5.0,5.0,1}; // luz puntual
    glLightfv(GL_LIGHT0,GL_POSITION,luz0); // la luz se coloca aquÌ si permanece fija y no se mueve con la escena
    glEnable(GL_LIGHT0);
	glPushMatrix(); // guarda la matriz de modelado

	glMultMatrixf(matrix);
	glScaled(size, size, size);
	
    miHormiga.visualizar();
    
    glPopMatrix (); // restaura la matriz de modelado
}



/*Rotaciones y traslaciones*/

void Hormiga::rotarCuerpo(const float& valor){
	miHormiga.setGL (HORMIGA::CUERPO_RY, miHormiga.getGL(HORMIGA::CUERPO_RY)+valor);
}

void Hormiga::moverCuerpo(const float& t) {
	miHormiga.setGL( HORMIGA::CUERPO_TX, miHormiga.getGL(HORMIGA::CUERPO_TX) + t * cos(miHormiga.getGL(HORMIGA::CUERPO_RY) * M_PI/180 ) );
	miHormiga.setGL ( HORMIGA::CUERPO_TZ, miHormiga.getGL(HORMIGA::CUERPO_TZ) - t * sin(miHormiga.getGL(HORMIGA::CUERPO_RY) * M_PI/180 ) );
}

void Hormiga::trasladarCuerpo(float x, float y, float z) {
	miHormiga.setGL(HORMIGA::CUERPO_TX, x);
	miHormiga.setGL(HORMIGA::CUERPO_TY, y);
	miHormiga.setGL(HORMIGA::CUERPO_TZ, z);
}

void Hormiga::MoverCabeza(float valor){
	static int oscilacion = 1;

	miHormiga.setGL(HORMIGA::CABEZA, miHormiga.getGL(HORMIGA::CABEZA) + valor*oscilacion);
    if (miHormiga.getGL(HORMIGA::CABEZA) < -7 ){
		miHormiga.setGL(HORMIGA::CABEZA, -7);
		oscilacion*=-1;
	}
    if (miHormiga.getGL(HORMIGA::CABEZA) > 7 ){
		miHormiga.setGL(HORMIGA::CABEZA, 7);
		oscilacion*=-1;
	}
}

void Hormiga::MoverPinzas(float valor){
	
    miHormiga.setGL(HORMIGA::PINZAS, miHormiga.getGL(HORMIGA::PINZAS) + valor);	
	if (miHormiga.getGL(HORMIGA::PINZAS) < -80) miHormiga.setGL(HORMIGA::PINZAS, -80);
    if (miHormiga.getGL(HORMIGA::PINZAS) > 10 ) miHormiga.setGL(HORMIGA::PINZAS, 10);

}

void Hormiga::MoverAntenas(float valor){
	static int oscilacion = 1;
	miHormiga.setGL(HORMIGA::ANTENAS, miHormiga.getGL(HORMIGA::ANTENAS) + valor*oscilacion);	
	if (miHormiga.getGL(HORMIGA::ANTENAS) < -2){
		miHormiga.setGL(HORMIGA::ANTENAS, -2);
		oscilacion*=-1;
	}
    if (miHormiga.getGL(HORMIGA::ANTENAS) > 2 ){
		miHormiga.setGL(HORMIGA::ANTENAS, 2);
		oscilacion*=-1;
	}
}

void Hormiga::MoverPatas(float valor){
	static int oscilacion = 1;
	miHormiga.setGL(HORMIGA::PATADelantera, miHormiga.getGL(HORMIGA::PATADelantera) + valor*oscilacion);
    if (miHormiga.getGL(HORMIGA::PATADelantera) < -10){
		miHormiga.setGL(HORMIGA::PATADelantera, -10 );
		oscilacion*=-1;
	}
    if (miHormiga.getGL(HORMIGA::PATADelantera) > 10 ){ 
		miHormiga.setGL(HORMIGA::PATADelantera, 10);
		oscilacion*=-1;
	}
	
	miHormiga.setGL(HORMIGA::PATAMedia, miHormiga.getGL(HORMIGA::PATAMedia) - valor*oscilacion);
	if (miHormiga.getGL(HORMIGA::PATAMedia) < -10){
		miHormiga.setGL(HORMIGA::PATAMedia, -10 );
		oscilacion*=-1;
	}
    if (miHormiga.getGL(HORMIGA::PATAMedia) > 10 ){
		miHormiga.setGL(HORMIGA::PATAMedia, 10);
		oscilacion*=-1;
	}
	
	miHormiga.setGL(HORMIGA::PATATrasera, miHormiga.getGL(HORMIGA::PATATrasera) + valor*oscilacion);
    if (miHormiga.getGL(HORMIGA::PATATrasera) < -10){
		miHormiga.setGL(HORMIGA::PATATrasera, -10 );
		oscilacion*=-1;
	}
    if (miHormiga.getGL(HORMIGA::PATATrasera) > 10 ){
		miHormiga.setGL(HORMIGA::PATATrasera, 10);
		oscilacion*=-1;
	}
}

