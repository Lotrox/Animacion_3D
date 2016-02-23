#ifndef __IGVESCENA3D
#define __IGVESCENA3D

#include <GL/glut.h>
#include "Util.h"
#include "Interpolation.h"
#include "Hormiga.h"

class igvEscena3D {
	protected:
		// Atributos
		bool ejes;
		bool sR;
		Point3D* points;
		float* frames;
		Hormiga ant;
	public:
		// Constructores por defecto y destructor
		igvEscena3D();
		~igvEscena3D();

		// Metodos estáticos

		// Métodos
		// método con las llamadas OpenGL para visualizar la escena
    void visualizar();	
	bool get_ejes() {return ejes;};
	void set_ejes(bool _ejes){ejes = _ejes;};
	void slerpRand() { sR = !sR; };
};

#endif
