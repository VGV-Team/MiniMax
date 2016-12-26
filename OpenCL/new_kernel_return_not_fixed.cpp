
#define DEPTH_TO_CALC 2

#define Figure_Peasant 'p'
#define Figure_Rook 'r'
#define Figure_Knight 'h'
#define Figure_Bishop 'b'
#define Figure_Queen 'q'
#define Figure_King 'k'
#define Figure_Empty '.'

struct Figure
{
	char type;
	int x;
	int y;
	bool playerFigure;
	bool alive;
	bool firstMove;
};

struct Move
{
	char board[8][8];
	struct Figure figure;
	int newX, newY;
	int oldX, oldY;
	int newLocationFigure;
	char oldFigureType;
	int points;
	bool fatalMove;
};

struct MinimaxReturn
{
	int value;
	struct Move bestMove;
};

struct Element
{
	char board[8][8];
	struct Figure figures[32];
	int depth;
	bool AI;
	int cost;
	struct Move firstMove;
	int costHistory[DEPTH_TO_CALC];
};

void copyHistory(int history[], int newHistory[]);
void copyBoard(char board[8][8], char newBoard[8][8]);
void copyFigures(struct Figure figures[32], struct Figure newFigures[32]);

//struct Element elArr[DEPTH_TO_CALC * 100];
//int lastInd;

void PushFront(char board[8][8], struct Figure figures[32], int depth, bool AI, int cost, struct Move firstMove, int costHistory[], int *lastInd, struct Element elArr[DEPTH_TO_CALC * 100])
{
	for (int i = *lastInd; i > 0; i--) elArr[i] = elArr[i - 1];
	copyBoard(board, elArr[0].board);
	copyFigures(figures, elArr[0].figures);
	elArr[0].depth = depth;
	elArr[0].AI = AI;
	elArr[0].cost = cost;
	elArr[0].firstMove = firstMove;
	copyHistory(costHistory, elArr[0].costHistory);
	(*lastInd)++;
}

struct Element PopFront(int *lastInd, struct Element elArr[DEPTH_TO_CALC * 100])
{
	struct Element tmp = elArr[0];
	for (int i = 0; i< *lastInd - 1; i++) elArr[i] = elArr[i + 1];
	(*lastInd)--;
	return tmp;
}

int Count(int *lastInd, struct Element elArr[DEPTH_TO_CALC * 100])
{
	return *lastInd;
}


void copyBoard(char board[8][8], char newBoard[8][8])
{
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			newBoard[i][j] = board[i][j];
		}
	}
}

void copyFigures(struct Figure figures[32], struct Figure newFigures[32])
{
	for (int i = 0; i < 32; i++)
	{
		struct Figure f = figures[i];
		newFigures[i] = f;
	}
}

void addToMoves(struct Move moves[100], int* movesIndex, int newX, int newY, char board[8][8], struct Figure fig)
{
	struct Move m;
	m.figure = fig;
	m.newX = newX;
	m.newY = newY;
	m.oldX = fig.x;
	m.oldY = fig.y;
	m.oldFigureType = fig.type;
	m.fatalMove = false;

	// if peasant reached the end of the board he is promoted to a queen
	if (m.figure.type == Figure_Peasant)
	{
		if ((m.figure.playerFigure && m.newX == 0) || (!m.figure.playerFigure && m.newX == 7))
		{
			m.figure.type = Figure_Queen;
		}
	}

	if (board[newX][newY] == Figure_Empty) m.newLocationFigure = -1;
	else m.newLocationFigure = board[newX][newY];
	moves[(*movesIndex)] = m;
	(*movesIndex)++;
}

bool isFriendly(char board[8][8], struct Figure f, int x, int y, struct Figure figures[32])
{
	if (board[x][y] == Figure_Empty) return false;
	if (figures[board[x][y]].playerFigure == f.playerFigure) return true;
	else return false;
}

//Check if figure is under attack
bool isUnderAttack(struct Figure f, struct Figure figures[32], char board[8][8])
{
	int x, y;
	//check up
	x = f.x - 1;
	y = f.y;
	while (x >= 0 && board[x][y] == Figure_Empty) x--;
	if (x >= 0)
	{
		struct Figure attacker = figures[board[x][y]];
		if (attacker.playerFigure != f.playerFigure)
		{
			if (attacker.type == Figure_Rook || attacker.type == Figure_Queen) return true;
			if (x == f.x - 1 && attacker.type == Figure_King) return true;
		}
	}
	//check down
	x = f.x + 1;
	y = f.y;
	while (x < 8 && board[x][y] == Figure_Empty) x++;
	if (x < 8)
	{
		struct Figure attacker = figures[board[x][y]];
		if (attacker.playerFigure != f.playerFigure)
		{
			if (attacker.type == Figure_Rook || attacker.type == Figure_Queen) return true;
			if (x == f.x + 1 && attacker.type == Figure_King) return true;
		}

	}
	//check right
	x = f.x;
	y = f.y + 1;
	while (y < 8 && board[x][y] == Figure_Empty) y++;
	if (y < 8)
	{
		struct Figure attacker = figures[board[x][y]];
		if (attacker.playerFigure != f.playerFigure)
		{
			if (attacker.type == Figure_Rook || attacker.type == Figure_Queen) return true;
			if (y == f.y + 1 && attacker.type == Figure_King) return true;
		}

	}
	//check left
	x = f.x;
	y = f.y - 1;
	while (y >= 0 && board[x][y] == Figure_Empty) y--;
	if (y >= 0)
	{
		struct Figure attacker = figures[board[x][y]];
		if (attacker.playerFigure != f.playerFigure)
		{
			if (attacker.type == Figure_Rook || attacker.type == Figure_Queen) return true;
			if (y == f.y - 1 && attacker.type == Figure_King) return true;
		}
	}
	//check diagonal up right
	x = f.x - 1;
	y = f.y + 1;
	while (y < 8 && x >= 0 && board[x][y] == Figure_Empty) { x--; y++; }
	if (y < 8 && x >= 0)
	{
		struct Figure attacker = figures[board[x][y]];
		if (attacker.playerFigure != f.playerFigure)
		{
			if (attacker.type == Figure_Bishop || attacker.type == Figure_Queen) return true;
			if (y == f.y + 1 && x == f.x - 1 && attacker.type == Figure_King) return true;
		}
	}
	//check diagonal down right
	x = f.x + 1;
	y = f.y + 1;
	while (y < 8 && x < 8 && board[x][y] == Figure_Empty) { x++; y++; }
	if (y < 8 && x < 8)
	{
		struct Figure attacker = figures[board[x][y]];
		if (attacker.playerFigure != f.playerFigure)
		{
			if (attacker.type == Figure_Bishop || attacker.type == Figure_Queen) return true;
			if (y == f.y + 1 && x == f.x + 1 && attacker.type == Figure_King) return true;
		}
	}
	//check diagonal down left
	x = f.x + 1;
	y = f.y - 1;
	while (y >= 0 && x < 8 && board[x][y] == Figure_Empty) { x++; y--; }
	if (y >= 0 && x < 8)
	{
		struct Figure attacker = figures[board[x][y]];
		if (attacker.playerFigure != f.playerFigure)
		{
			if (attacker.type == Figure_Bishop || attacker.type == Figure_Queen) return true;
			if (y == f.y - 1 && x == f.x + 1 && attacker.type == Figure_King) return true;
		}
	}
	//check diagonal up left
	x = f.x - 1;
	y = f.y - 1;
	while (y >= 0 && x >= 0 && board[x][y] == Figure_Empty) { x--; y--; }
	if (y >= 0 && x >= 0)
	{
		struct Figure attacker = figures[board[x][y]];
		if (attacker.playerFigure != f.playerFigure)
		{
			if (attacker.type == Figure_Bishop || attacker.type == Figure_Queen) return true;
			if (y == f.y - 1 && x == f.x - 1 && attacker.type == Figure_King) return true;
		}
	}

	//check horses
	x = f.x;
	y = f.y;

	//up right
	if (x - 2 >= 0 && y + 1 < 8)
	{
		struct Figure attacker = figures[board[x - 2][y + 1]];
		if (attacker.playerFigure != f.playerFigure && attacker.type == Figure_Knight) return true;
	}
	if (x - 1 >= 0 && y + 2 < 8)
	{
		struct Figure attacker = figures[board[x - 1][y + 2]];
		if (attacker.playerFigure != f.playerFigure && attacker.type == Figure_Knight) return true;
	}
	//down right
	if (x + 1 < 8 && y + 2 < 8)
	{
		struct Figure attacker = figures[board[x + 1][y + 2]];
		if (attacker.playerFigure != f.playerFigure && attacker.type == Figure_Knight) return true;
	}
	if (x + 2 < 8 && y + 1 < 8)
	{
		struct Figure attacker = figures[board[x + 2][y + 1]];
		if (attacker.playerFigure != f.playerFigure && attacker.type == Figure_Knight) return true;
	}
	//down left
	if (x + 2 < 8 && y - 1 >= 0)
	{
		struct Figure attacker = figures[board[x + 2][y - 1]];
		if (attacker.playerFigure != f.playerFigure && attacker.type == Figure_Knight) return true;
	}
	if (x + 1 < 8 && y - 2 >= 0)
	{
		struct Figure attacker = figures[board[x + 1][y - 2]];
		if (attacker.playerFigure != f.playerFigure && attacker.type == Figure_Knight) return true;
	}
	//up left
	if (x - 1 >= 0 && y - 2 >= 0)
	{
		struct Figure attacker = figures[board[x - 1][y - 2]];
		if (attacker.playerFigure != f.playerFigure && attacker.type == Figure_Knight) return true;
	}
	if (x - 2 >= 0 && y - 1 >= 0)
	{
		struct Figure attacker = figures[board[x - 2][y - 1]];
		if (attacker.playerFigure != f.playerFigure && attacker.type == Figure_Knight) return true;
	}

	//check peasants
	x = f.x;
	y = f.y;
	if (f.playerFigure)
	{
		//look for peasants that are up
		x = f.x - 1;
		y = f.y + 1;
		if (x >= 0 && y < 8 && board[x][y] != Figure_Empty)
		{
			struct Figure attacker = figures[board[x][y]];
			if (!attacker.playerFigure && attacker.type == Figure_Peasant) return true;
		}
		x = f.x - 1;
		y = f.y - 1;
		if (x >= 0 && y >= 0 && board[x][y] != Figure_Empty)
		{
			struct Figure attacker = figures[board[x][y]];
			if (!attacker.playerFigure && attacker.type == Figure_Peasant) return true;
		}
	}
	else
	{
		//AI figure, look for peasants down
		x = f.x + 1;
		y = f.y + 1;
		if (x < 8 && y < 8 && board[x][y] != Figure_Empty)
		{
			struct Figure attacker = figures[board[x][y]];
			if (attacker.playerFigure && attacker.type == Figure_Peasant) return true;
		}
		x = f.x + 1;
		y = f.y - 1;
		if (x >= 0 && y >= 0 && board[x][y] != Figure_Empty)
		{
			struct Figure attacker = figures[board[x][y]];
			if (attacker.playerFigure && attacker.type == Figure_Peasant) return true;
		}
	}

	return false;

}

void getAvailableMoves(struct Figure f, char board[8][8], struct Move moves[], int* movesIndex, struct Figure figures[32])
{

	if (f.type == Figure_Peasant)
	{
		if (f.playerFigure)
		{
			// look up
			if (f.x - 1 >= 0 && board[f.x - 1][f.y] == Figure_Empty)
			{
				addToMoves(moves, movesIndex, f.x - 1, f.y, board, f);
			}
			//look up 2 fields - first peasant move
			if (f.firstMove && f.x - 2 >= 0 && board[f.x - 2][f.y] == Figure_Empty)
			{
				addToMoves(moves, movesIndex, f.x - 2, f.y, board, f);
			}
			if (f.x - 1 < 8 && f.y + 1 < 8 && board[f.x - 1][f.y + 1] != Figure_Empty)
			{
				// kill and add to moves
				if (!isFriendly(board, f, f.x - 1, f.y + 1, figures)) addToMoves(moves, movesIndex, f.x - 1, f.y + 1, board, f);
			}
			if (f.x - 1 < 8 && f.y - 1 >= 0 && board[f.x - 1][f.y - 1] != Figure_Empty)
			{
				// kill and add to moves
				if (!isFriendly(board, f, f.x - 1, f.y - 1, figures)) addToMoves(moves, movesIndex, f.x - 1, f.y - 1, board, f);
			}
		}
		else
		{
			// look down
			if (f.x + 1 < 8 && board[f.x + 1][f.y] == Figure_Empty)
			{
				addToMoves(moves, movesIndex, f.x + 1, f.y, board, f);
			}
			//look down 2 fields - first peasant move
			if (f.firstMove && f.x + 2 < 8 && board[f.x + 2][f.y] == Figure_Empty)
			{
				addToMoves(moves, movesIndex, f.x + 2, f.y, board, f);
			}
			if (f.x + 1<8 && f.y + 1 < 8 && board[f.x + 1][f.y + 1] != Figure_Empty)
			{
				// kill and add to moves
				if (!isFriendly(board, f, f.x + 1, f.y + 1, figures)) addToMoves(moves, movesIndex, f.x + 1, f.y + 1, board, f);
			}
			if (f.x + 1<8 && f.y - 1 >= 0 && board[f.x + 1][f.y - 1] != Figure_Empty)
			{
				// kill and add to moves
				if (!isFriendly(board, f, f.x + 1, f.y - 1, figures)) addToMoves(moves, movesIndex, f.x + 1, f.y - 1, board, f);
			}
		}
	}
	if (f.type == Figure_Rook)
	{
		//if (f.playerFigure)
		{
			int x;
			int y;
			// look up
			x = f.x - 1;
			y = f.y;
			while (x >= 0)
			{
				if (board[x][y] == Figure_Empty)
				{
					addToMoves(moves, movesIndex, x, y, board, f);
				}
				else
				{
					if (!isFriendly(board, f, x, y, figures)) addToMoves(moves, movesIndex, x, y, board, f);
					break;
				}
				x--;
			}
			//look down
			x = f.x + 1;
			y = f.y;
			while (x < 8)
			{
				if (board[x][y] == Figure_Empty)
				{
					addToMoves(moves, movesIndex, x, y, board, f);
				}
				else
				{
					if (!isFriendly(board, f, x, y, figures)) addToMoves(moves, movesIndex, x, y, board, f);
					break;
				}
				x++;
			}
			//look left
			x = f.x;
			y = f.y - 1;
			while (y >= 0)
			{
				if (board[x][y] == Figure_Empty)
				{
					addToMoves(moves, movesIndex, x, y, board, f);
				}
				else
				{
					if (!isFriendly(board, f, x, y, figures)) addToMoves(moves, movesIndex, x, y, board, f);
					break;
				}
				y--;
			}
			//look right
			x = f.x;
			y = f.y + 1;
			while (y < 8)
			{
				if (board[x][y] == Figure_Empty)
				{
					addToMoves(moves, movesIndex, x, y, board, f);
				}
				else
				{
					if (!isFriendly(board, f, x, y, figures))  addToMoves(moves, movesIndex, x, y, board, f);
					break;
				}
				y++;
			}
		}
	}
	if (f.type == Figure_Knight)
	{
		int x = f.x;
		int y = f.y;

		//up right
		if (x - 2 >= 0 && y + 1 < 8)
		{
			if (!isFriendly(board, f, x - 2, y + 1, figures))  addToMoves(moves, movesIndex, x - 2, y + 1, board, f);
		}
		if (x - 1 >= 0 && y + 2 < 8)
		{
			if (!isFriendly(board, f, x - 1, y + 2, figures)) addToMoves(moves, movesIndex, x - 1, y + 2, board, f);
		}
		//down right
		if (x + 1 < 8 && y + 2 < 8)
		{
			if (!isFriendly(board, f, x + 1, y + 2, figures)) addToMoves(moves, movesIndex, x + 1, y + 2, board, f);
		}
		if (x + 2 < 8 && y + 1 < 8)
		{
			if (!isFriendly(board, f, x + 2, y + 1, figures)) addToMoves(moves, movesIndex, x + 2, y + 1, board, f);
		}
		//down left
		if (x + 2 < 8 && y - 1 >= 0)
		{
			if (!isFriendly(board, f, x + 2, y - 1, figures)) addToMoves(moves, movesIndex, x + 2, y - 1, board, f);
		}
		if (x + 1 < 8 && y - 2 >= 0)
		{
			if (!isFriendly(board, f, x + 1, y - 2, figures))  addToMoves(moves, movesIndex, x + 1, y - 2, board, f);
		}
		//up left
		if (x - 1 >= 0 && y - 2 >= 0)
		{
			if (!isFriendly(board, f, x - 1, y - 2, figures)) addToMoves(moves, movesIndex, x - 1, y - 2, board, f);
		}
		if (x - 2 >= 0 && y - 1 >= 0)
		{
			if (!isFriendly(board, f, x - 2, y - 1, figures)) addToMoves(moves, movesIndex, x - 2, y - 1, board, f);
		}

	}
	if (f.type == Figure_Bishop)
	{
		int x;
		int y;
		// look up right
		x = f.x - 1;
		y = f.y + 1;
		while (x >= 0 && y < 8)
		{
			if (board[x][y] == Figure_Empty)
			{
				addToMoves(moves, movesIndex, x, y, board, f);
			}
			else
			{
				if (!isFriendly(board, f, x, y, figures)) addToMoves(moves, movesIndex, x, y, board, f);
				break;
			}
			x--;
			y++;
		}
		// look down right
		x = f.x + 1;
		y = f.y + 1;
		while (x < 8 && y < 8)
		{
			if (board[x][y] == Figure_Empty)
			{
				addToMoves(moves, movesIndex, x, y, board, f);
			}
			else
			{
				if (!isFriendly(board, f, x, y, figures)) addToMoves(moves, movesIndex, x, y, board, f);
				break;
			}
			x++;
			y++;
		}
		// look down left
		x = f.x + 1;
		y = f.y - 1;
		while (x < 8 && y >= 0)
		{
			if (board[x][y] == Figure_Empty)
			{
				addToMoves(moves, movesIndex, x, y, board, f);
			}
			else
			{
				if (!isFriendly(board, f, x, y, figures)) addToMoves(moves, movesIndex, x, y, board, f);
				break;
			}
			x++;
			y--;
		}
		// look up left
		x = f.x - 1;
		y = f.y - 1;
		while (x >= 0 && y >= 0)
		{
			if (board[x][y] == Figure_Empty)
			{
				addToMoves(moves, movesIndex, x, y, board, f);
			}
			else
			{
				if (!isFriendly(board, f, x, y, figures)) addToMoves(moves, movesIndex, x, y, board, f);
				break;
			}
			x--;
			y--;
		}
	}
	if (f.type == Figure_Queen)
	{
		int x;
		int y;
		// look up right
		x = f.x - 1;
		y = f.y + 1;
		while (x >= 0 && y < 8)
		{
			if (board[x][y] == Figure_Empty)
			{
				addToMoves(moves, movesIndex, x, y, board, f);
			}
			else
			{
				if (!isFriendly(board, f, x, y, figures)) addToMoves(moves, movesIndex, x, y, board, f);
				break;
			}
			x--;
			y++;
		}
		// look down right
		x = f.x + 1;
		y = f.y + 1;
		while (x < 8 && y < 8)
		{
			if (board[x][y] == Figure_Empty)
			{
				addToMoves(moves, movesIndex, x, y, board, f);
			}
			else
			{
				if (!isFriendly(board, f, x, y, figures)) addToMoves(moves, movesIndex, x, y, board, f);
				break;
			}
			x++;
			y++;
		}
		// look down left
		x = f.x + 1;
		y = f.y - 1;
		while (x < 8 && y >= 0)
		{
			if (board[x][y] == Figure_Empty)
			{
				addToMoves(moves, movesIndex, x, y, board, f);
			}
			else
			{
				if (!isFriendly(board, f, x, y, figures)) addToMoves(moves, movesIndex, x, y, board, f);
				break;
			}
			x++;
			y--;
		}
		// look up left
		x = f.x - 1;
		y = f.y - 1;
		while (x >= 0 && y >= 0)
		{
			if (board[x][y] == Figure_Empty)
			{
				addToMoves(moves, movesIndex, x, y, board, f);
			}
			else
			{
				if (!isFriendly(board, f, x, y, figures)) addToMoves(moves, movesIndex, x, y, board, f);
				break;
			}
			x--;
			y--;
		}
		// look up
		x = f.x - 1;
		y = f.y;
		while (x >= 0)
		{
			if (board[x][y] == Figure_Empty)
			{
				addToMoves(moves, movesIndex, x, y, board, f);
			}
			else
			{
				if (!isFriendly(board, f, x, y, figures)) addToMoves(moves, movesIndex, x, y, board, f);
				break;
			}
			x--;
		}
		//look down
		x = f.x + 1;
		y = f.y;
		while (x < 8)
		{
			if (board[x][y] == Figure_Empty)
			{
				addToMoves(moves, movesIndex, x, y, board, f);
			}
			else
			{
				if (!isFriendly(board, f, x, y, figures)) addToMoves(moves, movesIndex, x, y, board, f);
				break;
			}
			x++;
		}
		//look left
		x = f.x;
		y = f.y - 1;
		while (y >= 0)
		{
			if (board[x][y] == Figure_Empty)
			{
				addToMoves(moves, movesIndex, x, y, board, f);
			}
			else
			{
				if (!isFriendly(board, f, x, y, figures)) addToMoves(moves, movesIndex, x, y, board, f);
				break;
			}
			y--;
		}
		//look right
		x = f.x;
		y = f.y + 1;
		while (y < 8)
		{
			if (board[x][y] == Figure_Empty)
			{
				addToMoves(moves, movesIndex, x, y, board, f);
			}
			else
			{
				if (!isFriendly(board, f, x, y, figures)) addToMoves(moves, movesIndex, x, y, board, f);
				break;
			}
			y++;
		}
	}
	if (f.type == Figure_King)
	{
		int x = f.x;
		int y = f.y;
		//look up
		if (x - 1 >= 0 && !isFriendly(board, f, x - 1, y, figures)) addToMoves(moves, movesIndex, x - 1, y, board, f);
		//look up right
		if (x - 1 >= 0 && y + 1 < 8 && !isFriendly(board, f, x - 1, y + 1, figures)) addToMoves(moves, movesIndex, x - 1, y + 1, board, f);
		//look right
		if (y + 1 < 8 && !isFriendly(board, f, x, y + 1, figures)) addToMoves(moves, movesIndex, x, y + 1, board, f);
		//look down right
		if (x + 1 < 8 && y + 1 < 8 && !isFriendly(board, f, x + 1, y + 1, figures)) addToMoves(moves, movesIndex, x + 1, y + 1, board, f);
		//look down
		if (x + 1 < 8 && !isFriendly(board, f, x + 1, y, figures)) addToMoves(moves, movesIndex, x + 1, y, board, f);
		//look down left
		if (x + 1 < 8 && y - 1 >= 0 && !isFriendly(board, f, x + 1, y - 1, figures)) addToMoves(moves, movesIndex, x + 1, y - 1, board, f);
		//look left
		if (y - 1 >= 0 && !isFriendly(board, f, x, y - 1, figures)) addToMoves(moves, movesIndex, x, y - 1, board, f);
		//look up left
		if (x - 1 >= 0 && y - 1 >= 0 && !isFriendly(board, f, x - 1, y - 1, figures)) addToMoves(moves, movesIndex, x - 1, y - 1, board, f);


		// castling

		// bottom left
		if (
			f.firstMove &&
			f.playerFigure &&
			board[7][0] != Figure_Empty &&
			figures[board[7][0]].playerFigure &&
			figures[board[7][0]].type == Figure_Rook &&
			figures[board[7][0]].firstMove &&

			board[7][1] == Figure_Empty &&
			board[7][2] == Figure_Empty &&
			board[7][3] == Figure_Empty &&
			!isUnderAttack(f, figures, board)
			)
		{
			addToMoves(moves, movesIndex, 7, 2, board, f);
		}
		// bottom right
		if (
			f.firstMove &&
			f.playerFigure &&
			board[7][7] != Figure_Empty &&
			figures[board[7][7]].playerFigure &&
			figures[board[7][7]].type == Figure_Rook &&
			figures[board[7][7]].firstMove &&

			board[7][5] == Figure_Empty &&
			board[7][6] == Figure_Empty &&
			!isUnderAttack(f, figures, board)
			)
		{
			addToMoves(moves, movesIndex, 7, 6, board, f);
		}
		// top left
		if (
			f.firstMove &&
			!f.playerFigure &&
			board[7][7] != Figure_Empty &&
			!figures[board[0][0]].playerFigure &&
			figures[board[0][0]].type == Figure_Rook &&
			figures[board[0][0]].firstMove &&

			board[0][1] == Figure_Empty &&
			board[0][2] == Figure_Empty &&
			board[0][3] == Figure_Empty &&
			!isUnderAttack(f, figures, board)
			)
		{
			addToMoves(moves, movesIndex, 0, 2, board, f);
		}
		// top right
		if (
			f.firstMove &&
			!f.playerFigure &&
			board[0][7] != Figure_Empty &&
			!figures[board[0][7]].playerFigure &&
			figures[board[0][7]].type == Figure_Rook &&
			figures[board[0][7]].firstMove &&

			board[0][5] == Figure_Empty &&
			board[0][6] == Figure_Empty &&
			!isUnderAttack(f, figures, board)
			)
		{
			addToMoves(moves, movesIndex, 0, 6, board, f);
		}

	}

}

void getAllAvailableMoves(char board[8][8], struct Move moves[], int* movesIndex, struct Figure figures[32], bool player)
{
	int oldI = 0;
	int start = player ? 16 : 0; //If we are checking player moves then start at figure 16, otherwise figure 0 for AI
	int end = start + 16;
	//Hardcoded for AI (figure index 0 to 15)
	for (int j = start; j < end; j++) {
		if (!figures[j].alive) continue; //If the figure is dead, then we skip it
										 //printf("Getting available moves for figure type %c\n", figures[j].type);
		getAvailableMoves(figures[j], board, moves, movesIndex, figures);
		for (int i = oldI; i < *movesIndex; i++)
		{
			oldI++;
			//printf("Move %d: from %d %d to x:%d y:%d\n", i + 1, moves[i].figure.x, moves[i].figure.y, moves[i].newX, moves[i].newY);
		}
	}
}

struct Figure initFigure(char type, bool alive, bool playerFigure, int x, int y)
{
	struct Figure f;
	f.type = type;
	f.alive = alive;
	f.playerFigure = playerFigure;
	f.x = x;
	f.y = y;
	f.firstMove = true; //Doesn't affect non-peasant figures
	return f;
}

void refreshBoard(struct Figure figures[32], char board[8][8])
{
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			board[i][j] = Figure_Empty;
	for (int i = 0; i < 32; i++)
	{
		struct Figure f = figures[i];

		if (f.alive) board[f.x][f.y] = i;

	}
}

void initChessboard(struct Figure figures[32], char board[8][8])
{
	figures[0] = initFigure(Figure_Rook, 1, 0, 0, 0);
	figures[1] = initFigure(Figure_Knight, 1, 0, 0, 1);
	figures[2] = initFigure(Figure_Bishop, 1, 0, 0, 2);
	figures[3] = initFigure(Figure_Queen, 1, 0, 0, 3);
	figures[4] = initFigure(Figure_King, 1, 0, 0, 4);
	figures[5] = initFigure(Figure_Bishop, 1, 0, 0, 5);
	figures[6] = initFigure(Figure_Knight, 1, 0, 0, 6);
	figures[7] = initFigure(Figure_Rook, 1, 0, 0, 7);
	for (int i = 0; i < 8; i++) figures[8 + i] = initFigure(Figure_Peasant, 1, 0, 1, i);

	figures[16] = initFigure(Figure_Rook, 1, 1, 7, 0);
	figures[17] = initFigure(Figure_Knight, 1, 1, 7, 1);
	figures[18] = initFigure(Figure_Bishop, 1, 1, 7, 2);
	figures[19] = initFigure(Figure_Queen, 1, 1, 7, 3);
	figures[20] = initFigure(Figure_King, 1, 1, 7, 4);

	figures[21] = initFigure(Figure_Bishop, 1, 1, 7, 5);
	figures[22] = initFigure(Figure_Knight, 1, 1, 7, 6);
	figures[23] = initFigure(Figure_Rook, 1, 1, 7, 7);
	for (int i = 0; i < 8; i++) figures[24 + i] = initFigure(Figure_Peasant, 1, 1, 6, i);


	refreshBoard(figures, board);

}

struct Move evaluateMoves(char board[8][8], struct Figure figures[32], struct Move moves[], int movesIndex)
{
	struct Move tmp;
	int points;
	int bestMoveIndex = -1;
	int bestPoints = -1;
	for (int i = 0; i < movesIndex; i++)
	{
		points = 0;
		tmp = moves[i];
		//moves include only moves to empty field or field with an enemy figure
		if (board[tmp.newX][tmp.newY] == Figure_Empty)
		{
			points = 0; //if we move to empty field, then this move is neutral
		}
		else
		{
			//otherwise we check which figure we kill and assign appropriate points to this move
			switch (figures[board[tmp.newX][tmp.newY]].type)
			{
			case Figure_Peasant:
				points = 10;
				break;
			case Figure_Rook:
				points = 50;
				break;
			case Figure_Knight:
				points = 50;
				break;
			case Figure_Bishop:
				points = 50;
				break;
			case Figure_Queen:
				points = 200;
				break;
			case Figure_King:
				// TODO: generalize this! Now only works for AI vs player
				points = 1000;
				moves[i].fatalMove = true;
				break;
			}
		}
		if (figures[board[tmp.oldX][tmp.oldY]].type == Figure_Peasant)
		{
			if ((tmp.figure.playerFigure && tmp.newX == 0) || (!tmp.figure.playerFigure && tmp.newX == 7)) points += 175;
		}


		if (points > bestPoints)
		{
			bestPoints = points;
			bestMoveIndex = i;
		}
		moves[i].points = points;
	}
	//we return the best move so we don't have to check later
	return moves[bestMoveIndex];
}

void makeMove(char board[8][8], struct Move m, struct Figure figures[32])
{

	if (board[m.newX][m.newY] == Figure_Empty)
	{
		//OK, we can move freely
		//We don't have to do anything (as of now)
	}
	else
	{
		//We will kill an enemy figure by moving here
		figures[board[m.newX][m.newY]].alive = false;

	}
	//printf("Moving %c from x:%d y:%d to x:%d y:%d\n", m.figure.type, m.figure.x, m.figure.y, m.newX, m.newY);
	//printf("%d %d\n", figures[1].x, figures[1].y);


	// castling
	if (m.figure.type == Figure_King && (m.oldY - m.newY == 2 || m.oldY - m.newY == -2))
	{
		if (m.oldY - m.newY > 0) // move left
		{
			board[m.newX][3] = board[m.figure.x][0];
			figures[board[m.figure.x][0]].x = m.newX;
			figures[board[m.figure.x][0]].y = 3;
			figures[board[m.figure.x][0]].firstMove = false;
			board[m.figure.x][0] = Figure_Empty;
		}
		else // move right
		{
			board[m.newX][5] = board[m.figure.x][7];
			figures[board[m.figure.x][7]].x = m.newX;
			figures[board[m.figure.x][7]].y = 5;
			figures[board[m.figure.x][7]].firstMove = false;
			board[m.figure.x][7] = Figure_Empty;
		}
	}


	// changes peasant to queen if he reached end of the board
	figures[board[m.figure.x][m.figure.y]].type = m.figure.type;

	// update board with figure that moved
	board[m.newX][m.newY] = board[m.figure.x][m.figure.y];

	figures[board[m.figure.x][m.figure.y]].x = m.newX;
	figures[board[m.figure.x][m.figure.y]].y = m.newY;
	figures[board[m.figure.x][m.figure.y]].firstMove = false; //We have to set this to false in case we moved a peasant for the first time.
															  //printf("%d %d\n", figures[1].x, figures[1].y);

															  // update board by clearing figure that moved
	board[m.figure.x][m.figure.y] = Figure_Empty;

	//refreshBoard(figures, board);


}

void undoMove(char board[8][8], struct Move m, struct Figure figures[32])
{

	// un castling
	if (m.figure.type == Figure_King && (m.oldY - m.newY == 2 || m.oldY - m.newY == -2))
	{
		if (m.oldY - m.newY > 0) // move left
		{
			board[m.newX][0] = board[m.figure.x][3];
			figures[board[m.figure.x][3]].x = m.oldX;
			figures[board[m.figure.x][3]].y = 0;
			figures[board[m.figure.x][3]].firstMove = true;
			board[m.figure.x][3] = Figure_Empty;

			figures[board[m.figure.x][2]].firstMove = true;
		}
		else // move right
		{
			board[m.newX][7] = board[m.figure.x][5];
			figures[board[m.figure.x][5]].x = m.newX;
			figures[board[m.figure.x][5]].y = 7;
			figures[board[m.figure.x][5]].firstMove = true;
			board[m.figure.x][5] = Figure_Empty;

			figures[board[m.figure.x][6]].firstMove = true;
		}
	}



	// reverse peasant first move
	if (figures[board[m.newX][m.newY]].type == Figure_Peasant)
	{
		if ((m.figure.playerFigure && m.oldX == 6) || (!m.figure.playerFigure && m.oldX == 1)) figures[board[m.newX][m.newY]].firstMove = true;
	}


	figures[board[m.newX][m.newY]].type = m.oldFigureType;
	board[m.oldX][m.oldY] = board[m.newX][m.newY];
	if (m.newLocationFigure >= 0)
	{
		board[m.newX][m.newY] = m.newLocationFigure;
		figures[m.newLocationFigure].alive = true;
	}
	else
	{
		board[m.newX][m.newY] = Figure_Empty;
	}
	figures[board[m.oldX][m.oldY]].x = m.oldX;
	figures[board[m.oldX][m.oldY]].y = m.oldY;

}

void copyHistory(int history[], int newHistory[])
{
	for (int i = 0; i < (DEPTH_TO_CALC); i++)
	{
		newHistory[i] = history[i];
	}
}

struct MinimaxReturn minimax(char board[8][8], struct Figure figures[32], int depth, bool maximizingPlayer, int cost, int *lastInd, struct Element elArr[DEPTH_TO_CALC * 100], struct MinimaxReturn *retHelper)
{
	struct MinimaxReturn ret;
	ret.value = -99999;

	{
		struct Move moves[100];
		int movesIndex = 0;
		getAllAvailableMoves(board, moves, &movesIndex, figures, !maximizingPlayer);
		evaluateMoves(board, figures, moves, movesIndex);
		for (int i = 0; i < movesIndex; i++) {

			if (moves[i].fatalMove == true)
			{
				ret.value = 555;
				ret.bestMove = moves[i];
				int s = Count(lastInd, elArr);
				for (int j = 0; j < s; j++) PopFront(lastInd, elArr);
				return ret;
			}

			char newBoard[8][8];
			struct Figure newFigures[32];

			copyBoard(board, newBoard);
			copyFigures(figures, newFigures);
			makeMove(newBoard, moves[i], newFigures);

			int history[DEPTH_TO_CALC]; // = { -2,-2,-2,-2,-2, moves[i].points };
			for (int j = 0; j < DEPTH_TO_CALC; j++)
				history[j] = 0;
			history[DEPTH_TO_CALC - 1] = moves[i].points;


			PushFront(newBoard, newFigures, depth - 1, !maximizingPlayer, 0, moves[i], history, lastInd, elArr);

		}
	}


	int bestCosts[DEPTH_TO_CALC];
	struct Move bestFinalMoves[DEPTH_TO_CALC];
	for (int i = 0; i < DEPTH_TO_CALC; i++)
		bestCosts[i] = -999999;


	while (Count(lastInd, elArr) > 0)
	{
		struct Element el = PopFront(lastInd, elArr);
		if (el.depth == 0)
		{

			if (el.costHistory[el.depth] >= bestCosts[el.depth])
			{

				bestCosts[el.depth] = el.costHistory[el.depth];
				bestFinalMoves[el.depth] = el.firstMove;

			}

			if (*lastInd == 0 || elArr[0].depth > el.depth)
			{
				int tmpDepth = 1;
				int topDepth;
				if (*lastInd == 0) topDepth = DEPTH_TO_CALC;
				else topDepth = elArr[0].depth + 1;

				while (topDepth > tmpDepth)
				{
					if (-bestCosts[tmpDepth - 1] + el.costHistory[tmpDepth] > bestCosts[tmpDepth])
					{
						bestCosts[tmpDepth] = -bestCosts[tmpDepth - 1] + el.costHistory[tmpDepth];
						bestFinalMoves[tmpDepth] = bestFinalMoves[tmpDepth - 1];
					}
					bestCosts[tmpDepth - 1] = -99999;
					tmpDepth++;
				}
			}
			continue;
		}

		struct Move moves[100];
		int movesIndex = 0;
		getAllAvailableMoves(el.board, moves, &movesIndex, el.figures, !el.AI);
		evaluateMoves(el.board, el.figures, moves, movesIndex);
		for (int i = 0; i < movesIndex; i++) {

			char newBoard[8][8];
			struct Figure newFigures[32];

			copyBoard(el.board, newBoard);
			copyFigures(el.figures, newFigures);
			makeMove(newBoard, moves[i], newFigures);

			el.costHistory[el.depth - 1] = moves[i].points;
			PushFront(newBoard, newFigures, el.depth - 1, !(el.AI), cost, el.firstMove, el.costHistory, lastInd, elArr);
		}
	}

	ret.value = bestCosts[DEPTH_TO_CALC - 1];
	ret.bestMove = bestFinalMoves[DEPTH_TO_CALC - 1];

	ret.value = 10;

	*retHelper = ret;

	return ret;
}



void miniMaxAI(struct Figure figures[32], struct MinimaxReturn *ret)
{
	char board[8][8];
	refreshBoard(figures, board);



	struct Move moves[100];
	int movesIndex = 0;

	int lastInd = 0;
	struct Element elArr[DEPTH_TO_CALC * 100];

	struct MinimaxReturn retHelper;

	struct MinimaxReturn r = minimax(board, figures, DEPTH_TO_CALC, true, 0, &lastInd, elArr, &retHelper);

	/*int *x;
	*x = 123123;*/

	(*ret).value = 10;
	(*ret).bestMove = retHelper.bestMove;

	//getAllAvailableMoves(board, moves, &movesIndex, figures, false);
	//evaluateMoves(board, figures, moves, movesIndex);
	/*
	int bestValue = -999999;
	for (int i = 0; i < movesIndex; i++)
	{
	if (bestValue <= moves[i].points)
	{
	bestValue = moves[i].points;
	(*ret).value = moves[i].points;
	(*ret).bestMove = moves[i];
	}
	}*/

	/*(*ret).value = moves[0].points;
	(*ret).bestMove = moves[0];*/

	/*
	int lastInd = 0;
	struct Element elArr[DEPTH_TO_CALC * 100];
	return minimax(board, figures, DEPTH_TO_CALC, true, 0, &lastInd, elArr);
	//struct Move bestMove = mRet.bestMove;
	//makeMove(board, bestMove, figures);
	*/
}

__kernel void minimaxGPU(
	__global struct Figure figures[],
	__global struct MinimaxReturn miniMaxRet[],
	int size
)
{
	int i = get_global_id(0);

	//miniMaxRet[]

	while (i < size)
	{
		struct Figure figs[32];
		for (int j = 0; j < 32; j++)
			figs[j] = figures[j + i];

		struct MinimaxReturn retHelper;

		miniMaxAI(figs, &retHelper);
		miniMaxRet[i].value = retHelper.value;
		miniMaxRet[i].bestMove = retHelper.bestMove;
		//struct MinimaxReturn m;
		//m.value = i;
		//miniMaxRet[i] = m;
		i += get_global_size(0);
	}
}


/*
__kernel void vector_add(__global const int *A,
__global const int *B,
__global int *C,
int size)
{
// globalni indeks elementa
int i = get_global_id(0);
// izracun
while (i < size)
{
C[i] = A[i] + B[i];
i += get_global_size(0);
}
}
*/