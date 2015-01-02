#include "DataStructs.h"
class Pawn: public CGFobject  {
	int owner;
	float f[4];
	CGFappearance* app;
public:
	int pos[2];
	void draw();
	Pawn(int x,int y,int owner);
};
