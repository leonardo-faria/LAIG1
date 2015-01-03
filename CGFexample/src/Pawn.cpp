#include "Pawn.h"
Pawn::Pawn(int x, int y, int owner) {
	pos[0] = x;
	pos[1] = y;
	this->owner = owner;
	app = new CGFappearance();
	f[0] = 0.5;
	f[1] = 0.5;
	f[2] = 0.5;
	f[3] = 1;
	app->setAmbient(f);
	app->setDiffuse(f);
	app->setSpecular(f);
	t0 = 0;
	moving = false;
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

void Pawn::update(unsigned long t) {
	if (!moving)
		return;
	if (t0 == 0)
		t0 = t;
	unsigned long dt = t - t0;
	if (dt < 500) {
		pos[0] = ori[0] + (dt / 500.0) * vx;
		pos[1] = ori[1] + (dt / 500.0) * vy;
	} else {
		moving = false;
		pos[0] = dest[0];
		pos[1] = dest[1];
	}
}

void Pawn::move(float x, float y) {
	t0 = 0;
	moving = true;
	ori[0] = pos[0];
	ori[1] = pos[1];
	vx = x - pos[0];
	vy = y - pos[1];
	dest[0] = x;
	dest[1] = y;
}
