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
	player = 1;
	da = 0;
	ang = 0;
	over = 0;
	float f[4];
	f[0] = 1;
	f[1] = 1;
	f[2] = 1;
	f[3] = 1;
	app = new CGFappearance();
	app->setAmbient(f);
	app->setDiffuse(f);
	app->setSpecular(f);
	app->setTexture("target.jpeg");

}

void Game::draw() {
	glPushMatrix();
	glRotatef(ang + da, 0, 1, 0);
	glPushMatrix();
	glRotatef(225, 0, 1, 0);
	glScalef(3.0, 3.0, 3.0);
	glTranslatef(-2.5, 0, -2.5);
//	cout << selectorPos[0] << selectorPos[1] << endl;
//	cout << "over " << over << endl;
	if (over) {
		glPushMatrix();
		glTranslatef(0, 3, 2.5);
		glScalef(0.005, 0.005, 0.005);
		glColor3f(0.5, 0.5, 1.0);		// azul
		glutStrokeCharacter(GLUT_STROKE_ROMAN, 'P');
		glutStrokeCharacter(GLUT_STROKE_ROMAN, 'L');
		glutStrokeCharacter(GLUT_STROKE_ROMAN, 'A');
		glutStrokeCharacter(GLUT_STROKE_ROMAN, 'Y');
		glutStrokeCharacter(GLUT_STROKE_ROMAN, 'E');
		glutStrokeCharacter(GLUT_STROKE_ROMAN, 'R');
		glutStrokeCharacter(GLUT_STROKE_ROMAN, ' ');
		glutStrokeCharacter(GLUT_STROKE_ROMAN, '0' + over);
		glutStrokeCharacter(GLUT_STROKE_ROMAN, ' ');
		glutStrokeCharacter(GLUT_STROKE_ROMAN, 'W');
		glutStrokeCharacter(GLUT_STROKE_ROMAN, 'I');
		glutStrokeCharacter(GLUT_STROKE_ROMAN, 'N');
		glutStrokeCharacter(GLUT_STROKE_ROMAN, 'S');
		glPopMatrix();
		glPushMatrix();
		glTranslatef(5, 2, 2.5);
		glScalef(0.005, 0.005, 0.005);
		glRotatef(180, 0, 1, 0);
		glColor3f(0.5, 0.5, 1.0);		// azul
		glutStrokeCharacter(GLUT_STROKE_ROMAN, 'P');
		glutStrokeCharacter(GLUT_STROKE_ROMAN, 'L');
		glutStrokeCharacter(GLUT_STROKE_ROMAN, 'A');
		glutStrokeCharacter(GLUT_STROKE_ROMAN, 'Y');
		glutStrokeCharacter(GLUT_STROKE_ROMAN, 'E');
		glutStrokeCharacter(GLUT_STROKE_ROMAN, 'R');
		glutStrokeCharacter(GLUT_STROKE_ROMAN, ' ');
		glutStrokeCharacter(GLUT_STROKE_ROMAN, '0' + over);
		glutStrokeCharacter(GLUT_STROKE_ROMAN, ' ');
		glutStrokeCharacter(GLUT_STROKE_ROMAN, 'W');
		glutStrokeCharacter(GLUT_STROKE_ROMAN, 'I');
		glutStrokeCharacter(GLUT_STROKE_ROMAN, 'N');
		glutStrokeCharacter(GLUT_STROKE_ROMAN, 'S');
		glPopMatrix();

	}
//	end();
	if (state != 0 && !over)
		glPushName(-1);
	for (int i = 0; i < 5; ++i) {
		glPushMatrix();
		if (state != 0 && !over)
			glLoadName(i);

		for (int j = 0; j < 5; ++j) {
			glPushMatrix();
			glTranslatef(i, 0, j);
			glRotatef(90, 1, 0, 0);
			glTranslatef(0.1, 0.1, 0);
			glScalef(0.8, 0.8, 0.8);
			if (state != 0 && !over)
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

			if (state != 0 && !over)
				glPopName();
			glPopMatrix();
		}
		glPopMatrix();
	}
	if (state != 0 && !over)
		glPopName();
	if (selected == true) {
		glPushMatrix();
		glTranslatef(selectorPos[0] + 0.5, 0.1, selectorPos[1] + 0.5);
		glRotatef(-90, 1, 0, 0);
		app->apply();
		GLUquadric *botD = gluNewQuadric();
		gluQuadricTexture(botD, GL_TRUE);
		gluDisk(botD, 0, 0.5, 20, 1);
		glPopMatrix();
	}
	if (state == 0 && !over) {
		glPushName(-1);
		for (unsigned int i = player; i < pawn.size(); ++i += 1) {
			glPushMatrix();
			glLoadName(i);
			pawn[i].draw();
			glPopMatrix();

		}
		glPopName();
		pawn[0].draw();
		for (unsigned int i = player - 1; i < pawn.size(); ++i += 1) {
			glPushMatrix();
			pawn[i].draw();
			glPopMatrix();

		}
	} else {
		for (unsigned int i = 0; i < pawn.size(); ++i) {
			glPushMatrix();
			pawn[i].draw();
			glPopMatrix();

		}
	}
	glPopMatrix();
}
int Game::move_piece(int x, int y) {
	if (test_move(x, y) == 0)
		return 1;
	Play p(select_pawn, pawn[select_pawn].pos[0], pawn[select_pawn].pos[1], x, y);
	history.push_back(p);
	pawn[select_pawn].move(x, y);
	return 0;
}

void Game::undo() {
	over=0;
	pawn[history[history.size() - 1].pawn].pos[0] = history[history.size() - 1].xi;
	pawn[history[history.size() - 1].pawn].pos[1] = history[history.size() - 1].yi;
	history.pop_back();
	if (state == 2) {
		rotate();
		if (player == 1)
			player = 2;
		else
			player = 1;
	}
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
		for (unsigned int i = 0; i < pawn.size(); i++)
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

int Game::test_move(int x, int y) {
	string str;
	str = "valida_jogada(" + to_string() + ",";
	str.push_back(pawn[select_pawn].pos[1] + '0');
	str.push_back(',');
	str.push_back(pawn[select_pawn].pos[0] + '0');
	str.push_back(',');
	str.push_back(y + '0');
	str.push_back(',');
	str.push_back(x + '0');
	str.push_back(',');
	str.push_back(pawn[select_pawn].owner + '0');
	str += ").\n";
	send((char*) str.c_str(), str.size());
	char res[256];
	int n = recieve(res);
	if (res[0] == '0')
		return 0;
	else
		return 1;
}

string Game::to_string() {
	string str = "[";
	for (int i = 0; i < 5; ++i) {
		str.push_back('[');
		for (int j = 0; j < 5; ++j) {
			str.push_back('0');
			if (j != 4)
				str.push_back(',');
		}
		str += "]";
		if (i != 4)
			str += ",";
	}
	str += "]";
	for (unsigned int i = 0; i < pawn.size(); ++i) {
		str[2 + pawn[i].pos[0] * 2 + pawn[i].pos[1] * 12] = pawn[i].owner + '0';
	}
	return str;
}

int Game::end() {
	cout << history[history.size() - 1].xf << endl;
	if (history[history.size() - 1].yf == 0)
		over = 1;
	if (history[history.size() - 1].yf == 4)
		over = 2;
	return 0;
}
