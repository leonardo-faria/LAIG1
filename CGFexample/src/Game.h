#include <vector>
#include <stack>
#include "Pawn.h"

using namespace std;

class Game {
	stack<Game> history;
public:
	vector<Pawn> pawn;
	int state;
	int select_pawn;
	Game();
	void draw();
	int selectorPos[2];
	bool selected;
	int move_piece(int x, int y);
	void undo();
};
