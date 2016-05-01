

/* Configuración de animación de partículas: Fuego */

float limits = 2; // Límite de altura en el eje Y que alcalzarán las partículas nates de desaparecer.
float scaleF = 2.8; // Escalado aplicado a las partículas
const int MAX_PARTICLES = 800; // Número máximo de particulas que contendrá la estructura de datos.
const int MIN_PARTICLES = 1; // Número mínimo de particulas que contendrá la estructura de datos.

float X = 0, Y = 0;
int currentParticle = 1;
float posX[MAX_PARTICLES], posY[MAX_PARTICLES], tempC[MAX_PARTICLES];

void moveParticles(int amount_of_particles) {
	srand(time(NULL));
	float myX;
	Sleep(1);
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
		if (posY[i] >= limits + 0.2 - ((rand() % 100) / 100)) {
			posY[i] = 0;
			posX[i] = 0;
		}
	}
}

void DrawFire(void) {
	int thingy = 1;
	bool check = false;

	if (check == false) {
		glPushMatrix();
		glBegin(GL_TRIANGLES);
		for (int i = 0; i < MAX_PARTICLES; i++) {
			/* Diferentes tonos de colores del fuego: Amarillo-Rojo-Negro */
			GLfloat color_a[] = { 1, 1, 0 };
			GLfloat color_r[] = { 1, tempC[i] * 2, 0 };
			GLfloat color_n[] = { tempC[i], tempC[i], tempC[i] };

			if (posY[i] >= 0) {
				if (tempC[i] < 0.05)
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
			if (posY[i] > 1) {
				glMaterialfv(GL_FRONT, GL_EMISSION, color_n);
				posX[i] -= 0.008;
			}
			glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

			float tamPoint = (limits + 0.5 - posY[i]) * scaleF;
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