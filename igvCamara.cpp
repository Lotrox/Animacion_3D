#include <GL/glut.h>
#include <math.h>

#include "igvCamara.h"


// Metodos constructores

igvCamara::igvCamara() { vista = NINGUNO; panoramico = false; multiples = false; estereographic = true; IOD = -0.25; }

igvCamara::~igvCamara () {}

igvCamara::igvCamara(tipoCamara _tipo, igvPunto3D _P0, igvPunto3D _r, igvPunto3D _V) {
	P0 = _P0;
	r = _r;
	V = _V;

	tipo = _tipo;
}


// Metodos publicos 
void igvCamara::set(igvPunto3D _P0, igvPunto3D _r, igvPunto3D _V) {
	P0 = _P0;
	r = _r;
	V = _V;
}
void igvCamara::set(tipoCamara _tipo, igvPunto3D _P0, igvPunto3D _r, igvPunto3D _V,
			                                double _xwmin, double _xwmax, double _ywmin, double _ywmax, double _znear, double _zfar) {
	tipo = _tipo;

	P0 = _P0;
	r = _r;
	V = _V;

	xwmin = _xwmin;
	xwmax = _xwmax;
	ywmin = _ywmin;
	ywmax = _ywmax;
	znear = _znear;
	zfar = _zfar;
}

void igvCamara::set(tipoCamara _tipo, igvPunto3D _P0, igvPunto3D _r, igvPunto3D _V,
			                         double _angulo, double _raspecto, double _znear, double _zfar) {
	tipo = _tipo;

	P0 = _P0;
	r = _r;
	V = _V;

	angulo = _angulo;
	raspecto = _raspecto;
	znear = _znear;
	zfar = _zfar;
}

void igvCamara::aplicar(void) {
	
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();

	if (tipo==IGV_PARALELA) {
		glOrtho(xwmin, xwmax, ywmin, ywmax, znear, zfar);
	}
	if (tipo==IGV_FRUSTUM) {
		glFrustum(xwmin, xwmax, ywmin, ywmax, znear, zfar);
	}
	if (tipo==IGV_PERSPECTIVA) {
		gluPerspective(angulo,raspecto,znear,zfar);
	}
	
	if (tipo == IGV_FRUSTUM) glTranslatef(IOD / 2, 0.0, 0.0);        //translate to cancel parallax
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(P0[X] + (IOD / 2), P0[Y], P0[Z], r[X] + (IOD / 2), r[Y], r[Z], V[X], V[Y], V[Z]);
	/*if(tipo!=IGV_FRUSTUM)*/
}


void igvCamara::zoom(double factor) {
	if(tipo == IGV_PERSPECTIVA){

		if( (180 > (angulo - angulo * factor)) && angulo > 0.00001) angulo -= angulo * factor; 

	}else if(tipo == IGV_PARALELA){
		
		xwmin -= xwmin * factor;
		xwmax -= xwmax * factor;
		ywmin -= ywmin * factor;
		ywmax -= ywmax * factor;

	}

}

void igvCamara::cambiarTipoCamara(){
	if(vista == NINGUNO){
		vista = PLANTA;
	}else if(vista == PLANTA){
		vista = PERFIL;
	}else if(vista == PERFIL){
		vista = ALZADO;
	}else if(vista == ALZADO){
		vista = NINGUNO;
	}
}