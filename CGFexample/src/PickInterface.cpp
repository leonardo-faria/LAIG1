#include "PickInterface.h"
#include "AnfScene.h"
#include "CGFapplication.h"

// buffer to be used to store the hits during picking
#define BUFSIZE 256
GLuint selectBuf[BUFSIZE];

void PickInterface::processMouse(int button, int state, int x, int y) {
	CGFinterface::processMouse(button, state, x, y);

	// do picking on mouse press (GLUT_DOWN)
	// this could be more elaborate, e.g. only performing picking when there is a click (DOWN followed by UP) on the same place
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		performPicking(x, y);
}

void PickInterface::performPicking(int x, int y) {
	// Sets the buffer to be used for selection and activate selection mode
	glSelectBuffer(BUFSIZE, selectBuf);
	glRenderMode(GL_SELECT);

	// Initialize the picking name stack
	glInitNames();

	// The process of picking manipulates the projection matrix
	// so we will be activating, saving and manipulating it
	glMatrixMode(GL_PROJECTION);

	//store current projmatrix to restore easily in the end with a pop
	glPushMatrix();

	//get the actual projection matrix values on an array of our own to multiply with pick matrix later
	GLfloat projmat[16];
	glGetFloatv(GL_PROJECTION_MATRIX, projmat);

	// reset projection matrix
	glLoadIdentity();

	// get current viewport and use it as reference for 
	// setting a small picking window of 5x5 pixels around mouse coordinates for picking
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	// this is multiplied in the projection matrix
	gluPickMatrix((GLdouble) x, (GLdouble) (CGFapplication::height - y), 5.0, 5.0, viewport);

	// multiply the projection matrix stored in our array to ensure same conditions as in normal render
	glMultMatrixf(projmat);

	// force scene drawing under this mode
	// only the names of objects that fall in the 5x5 window will actually be stored in the buffer
	scene->display();

	// restore original projection matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glFlush();

	// revert to render mode, get the picking results and process them
	GLint hits;
	hits = glRenderMode(GL_RENDER);
	processHits(hits, selectBuf);
}

void PickInterface::processHits(GLint hits, GLuint buffer[]) {
	GLuint *ptr = buffer;
	GLuint mindepth = 0xFFFFFFFF;
	GLuint *selected = NULL;
	GLuint nselected;

	// iterate over the list of hits, and choosing the one closer to the viewer (lower depth)
	for (int i = 0; i < hits; i++) {
		int num = *ptr;
		ptr++;
		GLuint z1 = *ptr;
		ptr++;
		ptr++;
		if (z1 < mindepth && num > 0) {
			mindepth = z1;
			selected = ptr;
			nselected = num;
		}
		for (int j = 0; j < num; j++)
			ptr++;
	}

	// if there were hits, the one selected is in "selected", and it consist of nselected "names" (integer ID's)
	if (selected != NULL) {
		// this should be replaced by code handling the picked object's ID's (stored in "selected"), 
		// possibly invoking a method on the scene class and passing "selected" and "nselected"
//		for (int i = 0; i < nselected; i++) {
//		}

		if (((AnfScene *) scene)->game->state == 0) {
			((AnfScene *) scene)->game->selectorPos[0] = ((AnfScene *) scene)->game->pawn[selected[0]].pos[0];
			((AnfScene *) scene)->game->selectorPos[1] = ((AnfScene *) scene)->game->pawn[selected[0]].pos[1];
			((AnfScene *) scene)->game->select_pawn = selected[0];
			((AnfScene *) scene)->game->state = 1;
			((AnfScene *) scene)->game->selected = true;
		} else if (((AnfScene *) scene)->game->state == 1) {
			((AnfScene *) scene)->game->move_piece(selected[0], selected[1]);
			((AnfScene *) scene)->game->selectorPos[0] = ((AnfScene *) scene)->game->pawn[0].pos[0];
			((AnfScene *) scene)->game->selectorPos[1] = ((AnfScene *) scene)->game->pawn[0].pos[1];
			((AnfScene *) scene)->game->select_pawn = 0;
			((AnfScene *) scene)->game->state = 2;
		} else {
			((AnfScene *) scene)->game->move_piece(selected[0], selected[1]);
			((AnfScene *) scene)->game->state = 0;
			((AnfScene *) scene)->game->selected = false;

			if (((AnfScene *) scene)->game->player == 0)
				((AnfScene *) scene)->game->player = 1;
			else
				((AnfScene *) scene)->game->player = 0;
			((AnfScene *) scene)->game->rotate();
		}
		cout << "State " << ((AnfScene *) scene)->game->state << endl;

	} else
		printf("Nothing selected while picking \n");
}

PickInterface::PickInterface() {
}

void PickInterface::initGUI() {
	GLUI_Panel *geral = addPanel("Interface", 1);
	GLUI_Panel *mode = addPanelToPanel(geral, "Modo", 1);
	GLUI_RadioGroup *modelist = addRadioGroupToPanel(mode, &(((AnfScene *) scene)->globals.drawing.mode));

	addRadioButtonToGroup(modelist, "Fill");
	addRadioButtonToGroup(modelist, "Line");
	addRadioButtonToGroup(modelist, "Point");

	addColumnToPanel(geral);

	GLUI_Panel *lightspanel = addPanelToPanel(geral, "Luzes", 1);
	for (int i = 0; i < ((AnfScene*) scene)->light_id.size(); ++i) {
		if (((AnfScene*) scene)->lights[i].enabled)
			addCheckboxToPanel(lightspanel, (char*) ((AnfScene*) scene)->light_id[i].c_str(), NULL, i)->set_int_val(1);
		else
			addCheckboxToPanel(lightspanel, (char*) ((AnfScene*) scene)->light_id[i].c_str(), NULL, i)->set_int_val(0);
	}

	addColumnToPanel(geral);

	GLUI_Panel *camerasPanel = addPanelToPanel(geral, "Camaras", 1);

	GLUI_RadioGroup *cameraList = addRadioGroupToPanel(camerasPanel, &(((AnfScene *) scene)->initial));

	for (unsigned int i = 0; i < ((AnfScene*) scene)->camera_id.size(); ++i) {
		if (i == ((AnfScene*) scene)->initial)
			addRadioButtonToGroup(cameraList, (char*) ((AnfScene*) scene)->camera_id[i].c_str())->set_int_val(1);
		else
			addRadioButtonToGroup(cameraList, (char*) ((AnfScene*) scene)->camera_id[i].c_str());
	}
	addRadioButtonToGroup(cameraList, (char*) "Free Camera");

	addColumnToPanel(geral);
	GLUI_Panel *shaderPanel = addPanelToPanel(geral, (char*) "Velocity", 1);
	addSpinnerToPanel(shaderPanel, (char*) "velocity", 2, &(((AnfScene*) scene)->velocity), 150);

	addColumnToPanel(geral);
	GLUI_Panel *undoPanel = addPanelToPanel(geral, (char*) "Game", 1);
	addButtonToPanel(undoPanel, (char*) "undo", 32);

}

void PickInterface::processGUI(GLUI_Control *ctrl) {
	if (ctrl->user_id >= 0)
		if (ctrl->user_id == 32) {
			if (!((AnfScene *) scene)->game->history.empty())
				((AnfScene *) scene)->game->undo();
		} else if (ctrl->user_id != 150) {
			if (ctrl->get_int_val() == 1) {
				((AnfScene *) scene)->lights[ctrl->user_id].enabled = true;
				((AnfScene *) scene)->lights[ctrl->user_id].cgfl->enable();
			} else {
				((AnfScene *) scene)->lights[ctrl->user_id].enabled = false;
				((AnfScene *) scene)->lights[ctrl->user_id].cgfl->disable();

			}
		}

}

