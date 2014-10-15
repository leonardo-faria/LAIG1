#ifndef LightingScene_H
#define LightingScene_H

#include "CGFscene.h"
#include "CGFappearance.h"
#include "tinyxml.h"
#include "tinystr.h"
#include "DataStructs.h"
class AnfScene: public CGFscene {
public:
	AnfScene(string file);
	void init();
	void display();
	void drawNode(Graph::Node* n);

	Globals globals;
	Cameras cameras;
	Lights lights;
	Textures textures;
	Graph graph;

	map<string, Appearence> appearances;

	map<string, CGFlight> light;
	CGFlight* light0;
	~AnfScene();
};

#endif
