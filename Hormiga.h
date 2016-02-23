#ifndef __HORMIGA
#define __HORMIGA

#include <GL/glut.h>
#include "igvGrafoEscena.h"

class Hormiga {
	protected:
    igvGrafoEscena miHormiga;
   
	public:
		Hormiga();
		~Hormiga();

    void visualizar(float size = 1);
     
    
    void rotarCuerpo(const float& valor);
    
    void MoverCabeza(float valor);
   
    void MoverPinzas(float valor);
 
	void MoverAntenas(float valor);

	void MoverPatas(float valor);

	void trasladarCuerpo(float x, float y, float z);
    
	void moverCuerpo(const float &t);
};

#endif
