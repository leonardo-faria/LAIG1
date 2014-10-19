#include "CGFinterface.h"
#include "AnfScene.h"
class Interface: public CGFinterface {
public:
	Interface();
	void processGUI(GLUI_Control *ctrl);
	void initGUI();
};

