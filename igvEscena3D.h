#ifndef __IGVESCENA3D
#define __IGVESCENA3D

#include <GL/glut.h>
#include "Util.h"
#include "Interpolation.h"

class igvEscena3D {
	protected:
		// Atributos
		bool ejes;
		bool sR;
		bool pause;
		bool trayec;
		bool input;
		GLfloat m[16];
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
	void pauseSwitch() { pause = !pause; };
	bool get_pause() { return pause; }
	void trayecSwitch() { trayec = !trayec; }
	void reloadInputs() { input = true; }
	GLfloat* getMatrix() { return m; }
};

#endif
