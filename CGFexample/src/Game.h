#include <vector>
#include <stack>
#include "Pawn.h"
#include "Play.h"
#include "plog.h"

using namespace std;

class Game {
public:
	CGFappearance* app;
	int t0,t,over;
	int player;
	vector<Pawn> pawn;
	vector<Play> history;
	int state;
	int select_pawn;
	bool rotating;
	float ang,da;
	Game();
	void draw();
	int selectorPos[2];
	bool selected;
	int move_piece(int x, int y);
	void undo();
	void update(unsigned long t);
	void rotate();
	string to_string();
	int test_move(int x,int y);
	int end();
};
