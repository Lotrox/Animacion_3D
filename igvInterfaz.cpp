#include <cstdlib>
#include <stdio.h>

#include "igvInterfaz.h"

extern igvInterfaz interfaz; // los callbacks deben ser estaticos y se requiere este objeto para acceder desde
                             // ellos a las variables de la clase

// Metodos constructores -----------------------------------

igvInterfaz::igvInterfaz () {recortar=false;}

igvInterfaz::~igvInterfaz () {}


// Metodos publicos ----------------------------------------

void igvInterfaz::crear_mundo(void) {
	// crear cámaras
	interfaz.camara.set(IGV_PARALELA, igvPunto3D(3.0,2.0,4),igvPunto3D(0,0,0),igvPunto3D(0,1.0,0),
		                                -1*3, 1*3, -1*3, 1*3, -1*3, 200);
}

void igvInterfaz::configura_entorno(int argc, char** argv,
			                              int _ancho_ventana, int _alto_ventana,
			                              int _pos_X, int _pos_Y,
													          string _titulo){
	// inicialización de las variables de la interfaz
	ancho_ventana = _ancho_ventana;
	alto_ventana = _alto_ventana;

	// inicialización de la ventana de visualización
	glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(_ancho_ventana,_alto_ventana);
  glutInitWindowPosition(_pos_X,_pos_Y);
	glutCreateWindow(_titulo.c_str());

	glEnable(GL_DEPTH_TEST); // activa el ocultamiento de superficies por z-buffer
  glClearColor(1.0,1.0,1.0,0.0); // establece el color de fondo de la ventana

	glEnable(GL_LIGHTING); // activa la iluminacion de la escena
  glEnable(GL_NORMALIZE); // normaliza los vectores normales para calculo iluminacion

	crear_mundo(); // crea el mundo a visualizar en la ventana
}

void igvInterfaz::inicia_bucle_visualizacion() {
	glutMainLoop(); // inicia el bucle de visualizacion de OpenGL
}

void igvInterfaz::set_glutKeyboardFunc(unsigned char key, int x, int y) {

	/* IMPORTANTE: en la implementación de este método hay que cambiar convenientemente el estado
	   de los objetos de la aplicación, pero no hacer llamadas directas a funciones de OpenGL */

	switch (key) {
    case 'p': // cambia el tipo de proyección de paralela a perspectiva y viceversa
		if(interfaz.camara.tipo==IGV_PARALELA){
			interfaz.camara.set(IGV_PERSPECTIVA, interfaz.camara.P0,interfaz.camara.r,interfaz.camara.V,
			60,1,1,100);
		}else{
			interfaz.camara.set(IGV_PARALELA, interfaz.camara.P0,interfaz.camara.r,interfaz.camara.V,
		                                -1*3, 1*3, -1*3, 1*3, -1*3, 200);
		}
		interfaz.camara.aplicar();

		break;
    case 'v': // cambia la posición de la cámara para mostrar las vistas planta, perfil, alzado o perspectiva
		interfaz.camara.cambiarTipoCamara();
		if(interfaz.camara.vista == PLANTA){
			interfaz.camara.set(igvPunto3D(0,6,0),igvPunto3D(0,0,0),igvPunto3D(1,0,0));
		}else if(interfaz.camara.vista == PERFIL){
			interfaz.camara.set(igvPunto3D(6,0,0),igvPunto3D(0,0,0),igvPunto3D(0,1,0));
		}else if(interfaz.camara.vista == ALZADO){
			interfaz.camara.set(igvPunto3D(0,0,6),igvPunto3D(0,0,0),igvPunto3D(0,1,0));
		}else if(interfaz.camara.vista == NINGUNO){
			interfaz.camara.set(IGV_PARALELA, igvPunto3D(3.0,2.0,4),igvPunto3D(0,0,0),igvPunto3D(0,1.0,0),
		                                -1*3, 1*3, -1*3, 1*3, -1*3, 200);
		}
		interfaz.camara.aplicar();
	  break;
    case '+': // zoom in
		interfaz.camara.zoom(0.05);
		interfaz.camara.aplicar();
		break;
    case '-': // zoom out
		interfaz.camara.zoom(-0.05);
		interfaz.camara.aplicar();
	  break;
    case 'c': // incrementar la distancia del plano cercano
		interfaz.camara.znear+=0.2;
		interfaz.camara.aplicar();
	  break;
    case 'C': // decrementar la distancia del plano cercano
		if(interfaz.camara.znear > 0.4) interfaz.camara.znear-=0.2; //Evitar el fallo de precisión al acercar el plano cercano a la cámara.
		interfaz.camara.aplicar();
	  break;
    case '9': // cambiar a formato 16:9 con deformación
		if(interfaz.camara.panoramico){ 
			interfaz.camara.panoramico = false;
		}else{
			interfaz.camara.panoramico = true;
		}
	  break;
    case '4': // cambiar a formato 4 viewports
		if(interfaz.camara.multiples){ interfaz.camara.multiples = false;
		}else{
			interfaz.camara.multiples = true;
		}
	  break;
    case 'r': // obtener sección del modelo
		
		 if(!interfaz.recortar){
            glEnable(GL_CLIP_PLANE0);
            glEnable(GL_CLIP_PLANE1);
			
		}else{
            glDisable(GL_CLIP_PLANE0);
            glDisable(GL_CLIP_PLANE1);
		}
		interfaz.recortar = !interfaz.recortar;
		break;
		case 'e': // activa/desactiva la visualizacion de los ejes
			interfaz.escena.set_ejes(interfaz.escena.get_ejes()?false:true);
	  break;
    case 27: // tecla de escape para SALIR
      exit(1);
    break;
  }
	glutPostRedisplay(); // renueva el contenido de la ventana de vision y redibuja la escena
}

void igvInterfaz::set_glutReshapeFunc(int w, int h) {
  // dimensiona el viewport al nuevo ancho y alto de la ventana
  // guardamos valores nuevos de la ventana de visualizacion
  interfaz.set_ancho_ventana(w);
  interfaz.set_alto_ventana(h);

	// establece los parámetros de la cámara y de la proyección
	interfaz.camara.aplicar();
}

void igvInterfaz::set_glutDisplayFunc() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // borra la ventana y el z-buffer

	// se establece el viewport
	if(!interfaz.camara.panoramico){ 
		glViewport(0, 0, interfaz.get_ancho_ventana(), interfaz.get_alto_ventana());
	}else{
		glViewport(0, (interfaz.alto_ventana-interfaz.ancho_ventana*(9.0/16.0))/2, interfaz.ancho_ventana, interfaz.ancho_ventana*(9.0/16.0));
	}

	if(interfaz.camara.multiples && !interfaz.camara.panoramico){
		glViewport(0,interfaz.alto_ventana/2,interfaz.ancho_ventana/2,interfaz.alto_ventana/2);
		interfaz.camara.set(igvPunto3D(3.0,2.0,4),igvPunto3D(0,0,0),igvPunto3D(0,1.0,0));
		interfaz.camara.aplicar();
		interfaz.escena.visualizar();

		glViewport(interfaz.ancho_ventana/2,interfaz.alto_ventana/2,interfaz.ancho_ventana/2,interfaz.alto_ventana/2);
		interfaz.camara.set(igvPunto3D(0,5,0),igvPunto3D(0,0,0),igvPunto3D(1,0,0));
		interfaz.camara.aplicar();
		interfaz.escena.visualizar();

		glViewport(0,0,interfaz.ancho_ventana/2,interfaz.alto_ventana/2);
		interfaz.camara.set(igvPunto3D(5,0,0),igvPunto3D(0,0,0),igvPunto3D(0,1,0));
		interfaz.camara.aplicar();
		interfaz.escena.visualizar();

		glViewport(interfaz.ancho_ventana/2,0,interfaz.ancho_ventana/2,interfaz.alto_ventana/2);
		interfaz.camara.set(igvPunto3D(0,0,5),igvPunto3D(0,0,0),igvPunto3D(0,1,0));
		interfaz.camara.aplicar();
		interfaz.escena.visualizar();
	}
	if(interfaz.camara.multiples && interfaz.camara.panoramico){
		glViewport(0,interfaz.alto_ventana/2,interfaz.ancho_ventana/2,interfaz.ancho_ventana/2*(9.0/16.0));
		interfaz.camara.set(igvPunto3D(3.0,2.0,4),igvPunto3D(0,0,0),igvPunto3D(0,1.0,0));
		interfaz.camara.aplicar();
		interfaz.escena.visualizar();

		glViewport(interfaz.get_ancho_ventana()/2.0,interfaz.get_alto_ventana()/2.0,interfaz.get_ancho_ventana()/2.0,interfaz.get_ancho_ventana()/2.0*(9.0/16.0));
		interfaz.camara.set(igvPunto3D(0,5,0),igvPunto3D(0,0,0),igvPunto3D(1,0,0));
		interfaz.camara.aplicar();
		interfaz.escena.visualizar();

		glViewport(0,(interfaz.get_alto_ventana()-interfaz.get_ancho_ventana()*(9.0/16.0))/2.0,interfaz.get_ancho_ventana()/2.0,interfaz.get_ancho_ventana()/2.0*(9.0/16.0));
		interfaz.camara.set(igvPunto3D(5,0,0),igvPunto3D(0,0,0),igvPunto3D(0,1,0));
		interfaz.camara.aplicar();
		interfaz.escena.visualizar();

		glViewport(interfaz.get_ancho_ventana()/2.0,(interfaz.get_alto_ventana()-interfaz.get_ancho_ventana()*(9.0/16.0))/2.0,interfaz.get_ancho_ventana()/2.0,interfaz.get_ancho_ventana()/2.0*(9.0/16.0));
		interfaz.camara.set(igvPunto3D(0,0,5),igvPunto3D(0,0,0),igvPunto3D(0,1,0));
		interfaz.camara.aplicar();
		interfaz.escena.visualizar();
	}
	//visualiza la escena
	interfaz.escena.visualizar();

	// refresca la ventana
	glutSwapBuffers(); // se utiliza, en vez de glFlush(), para evitar el parpadeo
}

void igvInterfaz::inicializa_callbacks() {
	glutKeyboardFunc(set_glutKeyboardFunc);
	glutReshapeFunc(set_glutReshapeFunc);
	glutDisplayFunc(set_glutDisplayFunc);
}
