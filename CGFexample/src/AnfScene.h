#ifndef LightingScene_H
#define LightingScene_H

#include "CGFscene.h"
#include "CGFappearance.h"
#include "tinyxml.h"
#include "tinystr.h"
#include "Game.h"
#include "DataStructs.h"
class AnfScene: public CGFscene {
public:
	AnfScene(char* file);
	void init();
	void display();
	void drawNode(Graph::Node* n, string appearencerefID, bool init);
	void createList(Graph::Node* n, string appearencerefID);
	void update(unsigned long t);
	Globals globals;
	Textures textures;
	Graph graph;
	void drawFlag(Graph::Node::Flag* f);
	map<string, Animation*> anim;
	vector<FlagShadder*> shaders;
	int velocity;
	map<string, Appearence> appearances;
	vector<Light> lights;
	map<string, Camera*> cameras;
	vector<string> camera_id;
	vector<string> light_id;
	int initial;
	int listNumber;
	unsigned long t0, t;
	Game* game;
	CGFappearance* defapp;
	~AnfScene();
};

#endif
