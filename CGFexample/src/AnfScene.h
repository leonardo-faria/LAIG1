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
	void drawNode(Graph::Node* n,string appearencerefID);

	Globals globals;
	Textures textures;
	Graph graph;

	map<string, Appearence> appearances;
	vector<Light> lights;
	map<string,Camera*> cameras;
	string initial;
	~AnfScene();
};

#endif
