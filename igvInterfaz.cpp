#include <cstdlib>
#include <stdio.h>

#include "igvInterfaz.h"
#include "Util.h"

#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat)
#include <opencv2/highgui/highgui.hpp>  // Video write
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;


extern igvInterfaz interfaz; // los callbacks deben ser estaticos y se requiere este objeto para acceder desde
                             // ellos a las variables de la clase

// Metodos constructores -----------------------------------

igvInterfaz::igvInterfaz() { recortar = false; travel = true; }

igvInterfaz::~igvInterfaz () {}


// Metodos publicos ----------------------------------------

void igvInterfaz::crear_mundo(void) {
	// crear cámaras
	interfaz.camara.set(IGV_PERSPECTIVA, interfaz.camara.P0, interfaz.camara.r, interfaz.camara.V,
		60, 1, 1, 100);
	util::LoadTraveling();
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
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glEnable(GLUT_MULTISAMPLE);
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


static bool photo = false;
void igvInterfaz::set_glutKeyboardFunc(unsigned char key, int x, int y) {

	/* IMPORTANTE: en la implementación de este método hay que cambiar convenientemente el estado
	   de los objetos de la aplicación, pero no hacer llamadas directas a funciones de OpenGL */

	switch (key) {
	case 'f': 
		photo = true;
	case 't':
		interfaz.travel = !interfaz.travel;
		break;
	case 'l':
		interfaz.escena.linealSwitch();
		break;
	case 'r':
		interfaz.escena.reloadInputs();
		util::LoadTraveling();
		break;
	case '8': // cambia el tipo de proyección de paralela a perspectiva y viceversa
		if (interfaz.camara.tipo == IGV_PARALELA) {
			interfaz.camara.set(IGV_PERSPECTIVA, interfaz.camara.P0, interfaz.camara.r, interfaz.camara.V,
				60, 1, 1, 100);
		}
		else {
			interfaz.camara.set(IGV_PARALELA, igvPunto3D(3.0, 2.0, 4), igvPunto3D(0, 0, 0), igvPunto3D(0, 1.0, 0),
				-16, 16, -9, 9, -200, 200);
		}
		interfaz.camara.aplicar();

		break;
	case 'v': // cambia la posición de la cámara para mostrar las vistas planta, perfil, alzado o perspectiva
		if (!interfaz.camara.estereographic) {
			interfaz.travel = false;
			interfaz.camara.cambiarTipoCamara();
			if (interfaz.camara.vista == PLANTA) {
				interfaz.camara.set(igvPunto3D(0, 16, 0), igvPunto3D(0, 0, 0), igvPunto3D(1, 0, 0));
			}
			else if (interfaz.camara.vista == PERFIL) {
				interfaz.camara.set(igvPunto3D(16, 0, 0), igvPunto3D(0, 0, 0), igvPunto3D(0, 1, 0));
			}
			else if (interfaz.camara.vista == ALZADO) {
				interfaz.camara.set(igvPunto3D(0, 0, 16), igvPunto3D(0, 0, 0), igvPunto3D(0, 1, 0));
			}
			else if (interfaz.camara.vista == NINGUNO) {
				interfaz.camara.set(IGV_PERSPECTIVA, interfaz.camara.P0, interfaz.camara.r, interfaz.camara.V,
					60, 1, 1, 100);
				interfaz.travel = true;
			}
			interfaz.camara.aplicar();
		}
	  break;
    case '+': // zoom in
		interfaz.camara.zoom(0.05);
		interfaz.camara.aplicar();
		break;
    case '-': // zoom out
		interfaz.camara.zoom(-0.05);
		interfaz.camara.aplicar();
	  break;
	case ',': // 
		interfaz.camara.IOD += 0.1;
		cout << interfaz.camara.IOD << endl;
		break;
	case '.': //
		interfaz.camara.IOD -= 0.1;
		cout << interfaz.camara.IOD << endl;
		break;
    case 'c': // incrementar la distancia del plano cercano
		interfaz.camara.znear+=0.2;
		interfaz.camara.aplicar();
	  break;
    case 'C': // decrementar la distancia del plano cercano
		if(interfaz.camara.znear > 0.4) interfaz.camara.znear-=0.2; //Evitar el fallo de precisión al acercar el plano cercano a la cámara.
		interfaz.camara.aplicar();
	  break;
	case '3':
		if (!interfaz.camara.multiples) {
			interfaz.camara.estereographic = !interfaz.camara.estereographic;
		}
		break;
    case '9': // cambiar a formato 16:9 con deformación
		if(interfaz.camara.panoramico){ 
			interfaz.camara.panoramico = false;
		}else{
			interfaz.camara.panoramico = true;
		}
	  break;
    case '4': // cambiar a formato 4 viewports
		
		if(interfaz.camara.multiples){ 
			interfaz.camara.estereographic = true;
			interfaz.camara.multiples = false;
			interfaz.camara.set(IGV_PERSPECTIVA, interfaz.camara.P0, interfaz.camara.r, interfaz.camara.V,
				60, 1, 1, 100);
		}else{
			interfaz.camara.estereographic = false;
			interfaz.camara.multiples = true;
			interfaz.camara.set(IGV_PARALELA, igvPunto3D(3.0, 2.0, 4), igvPunto3D(0, 0, 0), igvPunto3D(0, 1.0, 0),
				-16, 16, -9, 9, -200, 200);
		}
		interfaz.camara.aplicar();
	  break;
    case 'p': // obtener sección del modelo
		if (interfaz.escena.get_pause()) interfaz.travel = true;
		else interfaz.travel = false;
		interfaz.escena.pauseSwitch();
		break;
	case 'P': // obtener sección del modelo
		interfaz.escena.pauseSwitch();
		break;
	case 'e': // activa/desactiva la visualizacion de los ejes
			interfaz.escena.set_ejes(interfaz.escena.get_ejes()?false:true);
	  break;
	case 'E': // activa/desactiva la visualizacion de los ejes
		interfaz.escena.trayecSwitch();
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

#define DTR 0.0174532925

void igvInterfaz::set_glutDisplayFunc() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // borra la ventana y el z-buffer

	/*Modificar las coordenadas y vector dirección de la cámara para realizar el traveling.*/
	static Point3D r;
	if (interfaz.travel) {
		static float lambda = 0;
		static int keyFrame = 0;

		if (lambda > 1) {
			lambda = 0;
			if (keyFrame + 2 < util::TAM_T) keyFrame++;
			else keyFrame = 0;
		};
		lambda += 1.0 / (util::kFramesT[keyFrame + 1] - util::kFramesT[keyFrame]);
		int ini, end;
		if (keyFrame == 0) ini = keyFrame;
		else ini = keyFrame - 1;
		if (keyFrame + 1 == util::TAM_T - 1) end = util::TAM_T - 1;
		else end = keyFrame + 2;
		r = HermiteInterpolate(util::pointsT[ini], util::pointsT[keyFrame], util::pointsT[keyFrame + 1], util::pointsT[end], lambda, 0, 0);

	}

	if (interfaz.camara.estereographic) { //Viewports para la visualización del par estéreo.
		interfaz.camara.tipo = IGV_FRUSTUM;

		double fovy = 25;                                          //field of view in y-axis
		double aspect = double(interfaz.ancho_ventana / 2) / double(interfaz.alto_ventana / 2);  //screen aspect ratio
		double nearZ = 1;                                        //near clipping plane
		double farZ = 40.0;                                        //far clipping plane
		double screenZ = 40.0;                                     //screen projection plane

		double top = nearZ*tan(DTR*fovy / 2);                    //sets top of frustum based on fovy and near clipping plane
		double right = aspect*top;                             //sets right of frustum based on aspect ratio
		double frustumshift = (interfaz.camara.IOD / 2)*nearZ / screenZ;

		/*Test*/
		Point3D v(interfaz.escena.getMatrix()[12] - r.x, interfaz.escena.getMatrix()[13] - r.y, interfaz.escena.getMatrix()[14] - r.z);
		Point3D crossDot;
		crossDot = v.crossProduct(Point3D(0, 1, 0));
		crossDot.Normalize();
		crossDot.x *= interfaz.camara.IOD / 2.0;
		crossDot.y *= interfaz.camara.IOD / 2.0;
		crossDot.z *= interfaz.camara.IOD / 2.0;
		//cout << crossDot.x << " " << crossDot.y << " " << crossDot.z << endl;


		//Imagen izquierda
		interfaz.camara.ywmax = top;
		interfaz.camara.ywmin = -top;
		interfaz.camara.xwmin = -right + frustumshift;
		interfaz.camara.xwmax = right + frustumshift;
		interfaz.camara.znear = nearZ;
		interfaz.camara.zfar = farZ;
		interfaz.camara.IOD *= -1;
		interfaz.camara.set(igvPunto3D(r.x + crossDot.x, r.y + crossDot.y, r.z + crossDot.z), igvPunto3D(interfaz.escena.getMatrix()[12] + crossDot.x, interfaz.escena.getMatrix()[13] + crossDot.y, interfaz.escena.getMatrix()[14] + crossDot.z), igvPunto3D(0, 1, 0));
		glViewport(0, 0, interfaz.ancho_ventana / 2, interfaz.alto_ventana);
		interfaz.camara.aplicar();
		interfaz.escena.visualizar();



		//Imagen derecha
		interfaz.camara.ywmax = top;
		interfaz.camara.ywmin = -top;
		interfaz.camara.xwmin = -right - frustumshift;
		interfaz.camara.xwmax = right - frustumshift;
		interfaz.camara.znear = nearZ;
		interfaz.camara.zfar = farZ;
		interfaz.camara.IOD = abs((interfaz.camara.IOD));
		interfaz.camara.set(igvPunto3D(r.x - crossDot.x, r.y - crossDot.y, r.z - crossDot.z), igvPunto3D(interfaz.escena.getMatrix()[12] - crossDot.x, interfaz.escena.getMatrix()[13] - crossDot.y, interfaz.escena.getMatrix()[14] - crossDot.z), igvPunto3D(0, 1, 0));
		glViewport(interfaz.ancho_ventana / 2, 0, interfaz.ancho_ventana / 2, interfaz.alto_ventana);
		interfaz.camara.aplicar();
		interfaz.escena.visualizar();
	}
	else {
		interfaz.camara.tipo = IGV_PERSPECTIVA;
		interfaz.camara.set(igvPunto3D(r.x, r.y, r.z), igvPunto3D(interfaz.escena.getMatrix()[12], interfaz.escena.getMatrix()[13], interfaz.escena.getMatrix()[14]), igvPunto3D(0, 1, 0));
		interfaz.camara.aplicar();
	}


	// se establece el viewport
	if (!interfaz.camara.estereographic) {
		if (!interfaz.camara.panoramico) {
			glViewport(0, 0, interfaz.get_ancho_ventana(), interfaz.get_alto_ventana());
		}
		else if (interfaz.camara.panoramico) {
			glViewport(0, (interfaz.alto_ventana - interfaz.ancho_ventana*(9.0 / 16.0)) / 2, interfaz.ancho_ventana, interfaz.ancho_ventana*(9.0 / 16.0));
		}
	}



	if (interfaz.camara.multiples && !interfaz.camara.panoramico) {
		glViewport(0, interfaz.alto_ventana / 2, interfaz.ancho_ventana / 2, interfaz.alto_ventana / 2);
		interfaz.camara.set(igvPunto3D(3.0, 2.0, 4), igvPunto3D(0, 0, 0), igvPunto3D(0, 1.0, 0));
		interfaz.camara.aplicar();
		interfaz.escena.visualizar();

		glViewport(interfaz.ancho_ventana / 2, interfaz.alto_ventana / 2, interfaz.ancho_ventana / 2, interfaz.alto_ventana / 2);
		interfaz.camara.set(igvPunto3D(0, 5, 0), igvPunto3D(0, 0, 0), igvPunto3D(1, 0, 0));
		interfaz.camara.aplicar();
		interfaz.escena.visualizar();

		glViewport(0, 0, interfaz.ancho_ventana / 2, interfaz.alto_ventana / 2);
		interfaz.camara.set(igvPunto3D(5, 0, 0), igvPunto3D(0, 0, 0), igvPunto3D(0, 1, 0));
		interfaz.camara.aplicar();
		interfaz.escena.visualizar();

		glViewport(interfaz.ancho_ventana / 2, 0, interfaz.ancho_ventana / 2, interfaz.alto_ventana / 2);
		interfaz.camara.set(igvPunto3D(0, 0, 5), igvPunto3D(0, 0, 0), igvPunto3D(0, 1, 0));
		interfaz.camara.aplicar();
		interfaz.escena.visualizar();
	}
	if (interfaz.camara.multiples && interfaz.camara.panoramico) {
		glViewport(0, interfaz.alto_ventana / 2, interfaz.ancho_ventana / 2, interfaz.ancho_ventana / 2 * (9.0 / 16.0));
		interfaz.camara.set(igvPunto3D(3.0, 2.0, 4), igvPunto3D(0, 0, 0), igvPunto3D(0, 1.0, 0));
		interfaz.camara.aplicar();
		interfaz.escena.visualizar();

		glViewport(interfaz.get_ancho_ventana() / 2.0, interfaz.get_alto_ventana() / 2.0, interfaz.get_ancho_ventana() / 2.0, interfaz.get_ancho_ventana() / 2.0*(9.0 / 16.0));
		interfaz.camara.set(igvPunto3D(0, 5, 0), igvPunto3D(0, 0, 0), igvPunto3D(1, 0, 0));
		interfaz.camara.aplicar();
		interfaz.escena.visualizar();

		glViewport(0, (interfaz.get_alto_ventana() - interfaz.get_ancho_ventana()*(9.0 / 16.0)) / 2.0, interfaz.get_ancho_ventana() / 2.0, interfaz.get_ancho_ventana() / 2.0*(9.0 / 16.0));
		interfaz.camara.set(igvPunto3D(5, 0, 0), igvPunto3D(0, 0, 0), igvPunto3D(0, 1, 0));
		interfaz.camara.aplicar();
		interfaz.escena.visualizar();

		glViewport(interfaz.get_ancho_ventana() / 2.0, (interfaz.get_alto_ventana() - interfaz.get_ancho_ventana()*(9.0 / 16.0)) / 2.0, interfaz.get_ancho_ventana() / 2.0, interfaz.get_ancho_ventana() / 2.0*(9.0 / 16.0));
		interfaz.camara.set(igvPunto3D(0, 0, 5), igvPunto3D(0, 0, 0), igvPunto3D(0, 1, 0));
		interfaz.camara.aplicar();
		interfaz.escena.visualizar();
	}
	//visualiza la escena

	if (!interfaz.camara.estereographic) interfaz.escena.visualizar();
	// refresca la ventana
	glutSwapBuffers(); // se utiliza, en vez de glFlush(), para evitar el parpadeo
	
	static VideoWriter* writer = new VideoWriter();
	if (photo) {
		/*photo = false;
		cv::Mat buffer(interfaz.get_alto_ventana(), interfaz.get_ancho_ventana(), CV_8UC3);
		glPixelStorei(GL_PACK_ALIGNMENT, (buffer.step & 3) ? 1 : 4);
		glPixelStorei(GL_PACK_ROW_LENGTH, buffer.step / buffer.elemSize());
		glReadPixels(0, 0, buffer.cols, buffer.rows, GL_BGR_EXT, GL_UNSIGNED_BYTE, buffer.data);
		cv::Mat flipped(buffer);
		cv::flip(buffer, flipped, 0);
		// Write to file
		cv::imwrite("screenshot.bmp", buffer);*/
		static int g = 0;
		g++;
		if (g == 1) {
			writer->open("recorded.avi", CV_FOURCC('D', 'I', 'V', 'X'), 100, cv::Size(interfaz.get_ancho_ventana(), interfaz.get_alto_ventana()), 1);
		}
		
		if (g < 500) {
			cv::Mat frame(interfaz.get_alto_ventana(), interfaz.get_ancho_ventana(), CV_8UC3);
			glPixelStorei(GL_PACK_ALIGNMENT, (frame.step & 3) ? 1 : 4);
			glPixelStorei(GL_PACK_ROW_LENGTH, frame.step / frame.elemSize());
			glReadPixels(0, 0, frame.cols, frame.rows, GL_RGB, GL_UNSIGNED_BYTE, frame.data);
			cv::Mat flipped(frame);
			cv::flip(frame, flipped, 0);
			writer->write(frame);
		}
		if (g > 500) {
			writer->release();
			photo = false;
		}
	}
}

void igvInterfaz::inicializa_callbacks() {
	glutKeyboardFunc(set_glutKeyboardFunc);
	glutReshapeFunc(set_glutReshapeFunc);
	glutDisplayFunc(set_glutDisplayFunc);
}
