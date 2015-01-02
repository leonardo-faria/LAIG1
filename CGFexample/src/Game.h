#include <vector>
#include <stack>
#include "Pawn.h"
#include "Play.h"

using namespace std;

class Game {
public:
	vector<Pawn> pawn;
	vector<Play> history;
	int state;
	int select_pawn;
	Game();
	void draw();
	int selectorPos[2];
	bool selected;
	int move_piece(int x, int y);
	void undo();
};
