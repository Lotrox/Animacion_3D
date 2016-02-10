#ifndef __IGVESCENA3D
#define __IGVESCENA3D

#include <GL/glut.h>

class igvEscena3D {
	protected:
		// Atributos

		bool ejes;

	public:
		// Constructores por defecto y destructor
		igvEscena3D();
		~igvEscena3D();

		// Metodos est�ticos

		// M�todos
		// m�todo con las llamadas OpenGL para visualizar la escena
    void visualizar();

		bool get_ejes() {return ejes;};
		void set_ejes(bool _ejes){ejes = _ejes;};
};

#endif
