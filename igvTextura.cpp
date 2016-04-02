
#include "igvTextura.h"

// Metodos constructores y destructor

igvTextura::igvTextura() {
	ancho = 64;
  alto = 64;
	GLubyte textura[64][64][4];

  // almacena en el array textura una imagen con un tablero de ajedrez
	unsigned int i, j, c;
  for (i = 0; i < alto ; i++) {
    for (j = 0; j < ancho ; j++) {
		  c = (((i&0x8) == 0) ^ (((j&0x8)) == 0)) * 255;
		  textura[i][j][0] = (GLubyte) c;
		  textura[i][j][1] = (GLubyte) c;
		  textura[i][j][2] = (GLubyte) c;
		  textura[i][j][3] = (GLubyte) 255;
	  }
	}

	/* Apartado G: A�adir aqu� el c�digo para cargar la textura de ajedrez */
  /*
	- Generar el identificador de textura y asignarlo al atributo idTextura
  - Especificar la textura
  - Modo de aplicaci�n de la textura
	- Par�metros de la textura: repetici�n y filtros
	*/

    glGenTextures(1, &idTextura); 
	glBindTexture(GL_TEXTURE_2D, idTextura);
 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ancho, alto, 0, GL_RGBA, GL_UNSIGNED_BYTE, textura);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

}

igvTextura::igvTextura(char *fichero) {
  BITMAPINFO *BitmapInfo; /* Bitmap information */
  GLubyte *imagen;

  // carga en memoria el BMP almacenado en el fichero
	imagen=LoadDIBitmap(fichero,&BitmapInfo);

  alto=(*BitmapInfo).bmiHeader.biHeight;
  ancho=(*BitmapInfo).bmiHeader.biWidth;

	/* Apartado H: A�adir aqu� el c�digo para cargar como textura OpenGL el BMP imagen */
  /*
	- Generar el identificador de textura y asignarlo al atributo idTextura
  - Especificar la textura, asign�dole como textura el array imagen
  - Modo de aplicaci�n de la textura
	- Par�metros de la textura: repetici�n y filtros
	*/
	glGenTextures(1, &idTextura); 
    glBindTexture(GL_TEXTURE_2D, idTextura); 

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ancho, alto, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, imagen); 
  
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_MODULATE); 
    glTexParameteri (GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT); 
    glTexParameteri (GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);



	delete BitmapInfo;
	delete imagen;
}

void igvTextura::aplicar(void) {
  glBindTexture(GL_TEXTURE_2D, idTextura);
}

igvTextura::~igvTextura() {
	 glDeleteTextures(1, &idTextura); 
}