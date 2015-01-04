#include "DataStructs.h"
class Pawn: public CGFobject {
	float f[4];
	CGFappearance* app;
	unsigned long t0;
	float ori[2];
	float vx,vy;
	float dest[2];
public:
	int owner;
	bool moving;
	void move(float x,float y);
	void update(unsigned long t);
	float pos[2];
	void draw();
	Pawn(int x, int y, int owner);
};
