#include "Interface.h"

Interface::Interface() {
}

void Interface::initGUI() {
	GLUI_Panel *geral = addPanel("Interface", 1);
	GLUI_Panel *mode = addPanelToPanel(geral, "Modo", 1);
	GLUI_RadioGroup *modelist = addRadioGroupToPanel(mode,
			&(((AnfScene *) scene)->globals.drawing.mode));

	addRadioButtonToGroup(modelist, "Fill");
	addRadioButtonToGroup(modelist, "Line");
	addRadioButtonToGroup(modelist, "Point");

	addColumnToPanel(geral);

	GLUI_Panel *lightspanel = addPanelToPanel(geral, "Luzes", 1);
	for (int i = 0; i < ((AnfScene*) scene)->light_id.size(); ++i) {
		if (((AnfScene*) scene)->lights[i].enabled)
			addCheckboxToPanel(lightspanel,
					(char*) ((AnfScene*) scene)->light_id[i].c_str(), NULL, i)->set_int_val(
					1);
		else
			addCheckboxToPanel(lightspanel,
					(char*) ((AnfScene*) scene)->light_id[i].c_str(), NULL, i)->set_int_val(
					0);
	}

	addColumnToPanel(geral);

	GLUI_Panel *camerasPanel = addPanelToPanel(geral, "Camaras", 1);

	GLUI_RadioGroup *cameraList = addRadioGroupToPanel(camerasPanel,
			&(((AnfScene *) scene)->initial));

	for (unsigned int i = 0; i < ((AnfScene*) scene)->camera_id.size(); ++i) {
		if (i == ((AnfScene*) scene)->initial)
			addRadioButtonToGroup(cameraList,
					(char*) ((AnfScene*) scene)->camera_id[i].c_str())->set_int_val(
					1);
		else
			addRadioButtonToGroup(cameraList,
					(char*) ((AnfScene*) scene)->camera_id[i].c_str());
	}
	addRadioButtonToGroup(cameraList,"Free Camera");
//
}

void Interface::processGUI(GLUI_Control *ctrl) {
	if (ctrl->user_id >= 0)
		if (ctrl->get_int_val() == 1) {
			((AnfScene *) scene)->lights[ctrl->user_id].enabled = true;
			((AnfScene *) scene)->lights[ctrl->user_id].cgfl->enable();
		} else {
			((AnfScene *) scene)->lights[ctrl->user_id].enabled = false;
			((AnfScene *) scene)->lights[ctrl->user_id].cgfl->disable();

		}

}
