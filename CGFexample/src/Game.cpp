#include "Game.h"
//TODO aparencias do tabuleiro
Game::Game() {
	Pawn n(2, 2, 3);
	pawn.push_back(n);
	for (int i = 0; i < 5; ++i) {
		Pawn p1(i, 0, 1);
		Pawn p2(i, 4, 2);
		pawn.push_back(p1);
		pawn.push_back(p2);
	}
	selectorPos[0] = 0;
	selectorPos[1] = 0;
	selected = false;
	state = 0;
	player = 0;
	da = 0;
	ang = 0;
}

void Game::draw() {
	glPushMatrix();
	glRotatef(ang+da, 0, 1, 0);
	glTranslatef(-2.5, 0, -2.5);
//	cout << selectorPos[0] << selectorPos[1] << endl;
	if (state != 0)
		glPushName(-1);
	for (int i = 0; i < 5; ++i) {
		glPushMatrix();
		if (state != 0)
			glLoadName(i);

		for (int j = 0; j < 5; ++j) {
			glPushMatrix();
			glTranslatef(i, 0, j);
			glRotatef(90, 1, 0, 0);
			glTranslatef(0.1, 0.1, 0);
			glScalef(0.8, 0.8, 0.8);
			if (state != 0)
				glPushName(j);

			glBegin(GL_QUADS);
			glTexCoord2d(0, 0);
			glVertex3d(0, 0, 0);
			glTexCoord2d(0, 1);
			glVertex3d(0, 1, 0);
			glTexCoord2d(1, 1);
			glVertex3d(1, 1, 0);
			glTexCoord2d(1, 0);
			glVertex3d(1, 0, 0);

			glEnd();

			if (state != 0)
				glPopName();
			glPopMatrix();
		}
		glPopMatrix();
	}
	if (state != 0)
		glPopName();
	if (selected == true) {
		glPushMatrix();
		glTranslatef(selectorPos[0] + 0.5, 0.1, selectorPos[1] + 0.5);
		glRotatef(-90, 1, 0, 0);
		GLUquadric *botD = gluNewQuadric();
		gluQuadricTexture(botD, GL_TRUE);
		gluDisk(botD, 0, 0.5, 20, 1);
		glPopMatrix();
	}
	if (state == 0)
		glPushName(-1);
	for (unsigned int i = 0; i < pawn.size(); ++i) {
		glPushMatrix();
		if (state == 0)
			glLoadName(i);
		pawn[i].draw();
		glPopMatrix();
	}
	if (state == 0)
		glPopName();
	glPopMatrix();
}

int Game::move_piece(int x, int y) {
	Play p(select_pawn, pawn[select_pawn].pos[0], pawn[select_pawn].pos[1], x, y);
	history.push_back(p);
	pawn[select_pawn].move(x, y);
	return 0;
}

void Game::undo() {
	pawn[history[history.size() - 1].pawn].pos[0] = history[history.size() - 1].xi;
	pawn[history[history.size() - 1].pawn].pos[1] = history[history.size() - 1].yi;
	history.pop_back();
	if (state != 2) {
		select_pawn = 0;
		selectorPos[0] = pawn[0].pos[0];
		selectorPos[1] = pawn[0].pos[1];
		state = 2;
		selected = true;
	} else {
		selected = false;
		state = 0;
	}
}

void Game::rotate() {
	rotating = true;
	t0 = 0;
}

void Game::update(unsigned long t) {
	for (unsigned int i = 0; i < pawn.size(); i++)
		pawn[i].update(t);
	if (rotating) {
		for (int i = 0; i < pawn.size(); i++)
			if (pawn[i].moving)
				return;
		if (t0 == 0)
			t0 = t;
		unsigned long dt = t - t0;
		if (dt < 1000)
			da = 180 * dt / 1000.0;
		else {
			rotating = false;
			ang = ang + da;
			da = 0;
		}
	}
}
