#include "Pawn.h"
Pawn::Pawn(int x, int y, int owner) {
	pos[0] = x;
	pos[1] = y;
	this->owner = owner;
	app = new CGFappearance();
	f[0] = 0.5;
	f[1] = 0.5;
	f[2] = 0.5;
	f[3] = 0.5;
	app->setAmbient(f);
	app->setDiffuse(f);
	app->setSpecular(f);
}

void Pawn::draw() {
	glPushMatrix();
	app->apply();
	glTranslatef(pos[0] + 0.5, 0.25, pos[1] + 0.5);
	GLUquadric *sphere = gluNewQuadric();
	gluQuadricTexture(sphere, GL_TRUE);
	gluSphere(sphere, 0.25, 10, 10);
	glPopMatrix();
}
