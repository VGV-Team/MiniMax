#include <stdio.h>
#include <time.h>  
#include <stdlib.h>

//#define START_DEPTH 4

#define DEPTH_TO_CALC_CPU 4

// TODO: šah = edina poteza - later
// TODO: endless game by moving the same two figures
// TODO: endless game with two kings - later
// TODO: pat - no need yet

void copyHistory(int history[], int newHistory[]);
void copyBoard(char board[8][8], char newBoard[8][8]);
void copyFigures(struct Figure figures[32], struct Figure newFigures[32]);

bool gameOver = false;

enum FigureType
{
	Figure_Peasant = 'p',
	Figure_Rook = 'r',
	Figure_Knight = 'h',
	Figure_Bishop = 'b',
	Figure_Queen = 'q',
	Figure_King = 'k',
	Figure_Empty = '.'
};

struct Figure
{
	char type;
	int x;
	int y;

	bool playerFigure;
	bool alive;

	//Bool to check if the figure already moved
	bool firstMove;
};

struct Move
{
	char board[8][8];
	Figure figure;
	int newX, newY;
	int oldX, oldY;
	int newLocationFigure;
	char oldFigureType;
	//Is this good?
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
	Figure figures[32];
	int depth;
	bool AI;
	int cost;
	Move firstMove;
	Element* next;
	//int costHistory[(START_DEPTH+1)];
	int costHistory[DEPTH_TO_CALC_CPU];
	//int ind;
};


struct Element elArr[DEPTH_TO_CALC_CPU * 100];
int lastInd = 0;
void PushFront(char board[8][8], Figure figures[32], int depth, bool AI, int cost, Move firstMove, int costHistory[])
{
	for (int i = lastInd; i > 0; i--) elArr[i] = elArr[i - 1];
	copyBoard(board, elArr[0].board);
	copyFigures(figures, elArr[0].figures);
	elArr[0].depth = depth;
	elArr[0].AI = AI;
	//elArr[0].next = NULL;
	elArr[0].cost = cost;
	elArr[0].firstMove = firstMove;
	copyHistory(costHistory, elArr[0].costHistory);
	lastInd++;
}

Element PopFront()
{
	Element tmp = elArr[0];
	for (int i = 0; i< lastInd - 1; i++) elArr[i] = elArr[i + 1];
	lastInd--;
	return tmp;
}

int Count()
{
	return lastInd;
}

/*

struct Element *root = NULL;

void PushFront(char board[8][8], Figure figures[32], int depth, bool AI, int cost, Move firstMove, int costHistory[])
{
Element* q = (Element*)malloc(sizeof(Element));
copyBoard(board, q->board);
copyFigures(figures, q->figures);
q->depth = depth;
q->AI = AI;
q->next = NULL;
q->cost = cost;
q->firstMove = firstMove;

//for (int i = 0; i < 5; i++) {
//q->costHistory[i] = costHistory[i];
//}
copyHistory(costHistory, q->costHistory);

//if (root == NULL) root = q;
//else q->next = *root;
q->next = root;
root = q;
}

Element* PopFront()
{
Element* tmp = root;
if (root != NULL)
root = root->next;
return tmp;
}

int Count()
{
int cnt = 0;
Element* tmp = root;
while (tmp != NULL)
{
tmp = tmp->next;
cnt++;
}
return cnt;
}













struct FinalElement
{
//int costHistory[(START_DEPTH + 1)];
int costHistory[(DEPTH_TO_CALC_CPU)];
Move firstMove;
FinalElement* next;
};

struct FinalElement *finalElementRoot = NULL;
void PushFinal(int history[], Move firstMove)
{
FinalElement* q = (FinalElement*)malloc(sizeof(FinalElement));
q->next = NULL;
q->firstMove = firstMove;
copyHistory(history, q->costHistory);

//if (root == NULL) root = q;
//else q->next = *root;
q->next = finalElementRoot;
finalElementRoot = q;
}

*/





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

void addToMoves(struct Move moves[], int& movesIndex, int newX, int newY, char board[8][8], Figure fig)
{
	struct Move m;
	//char newBoard[8][8];
	//copyBoard(board, m.board);
	//TODO: change the board state
	m.figure = fig; //This is a COPY!!!
	m.newX = newX;
	m.newY = newY;
	m.oldX = fig.x;
	m.oldY = fig.y;
	m.oldFigureType = fig.type;
	m.fatalMove = false;

	// if peasant reached the end of the board he is promoted to a queen
	if (m.figure.type == Figure_Peasant)
	{
		if (m.figure.playerFigure && m.newX == 0 || !m.figure.playerFigure && m.newX == 7)
		{
			m.figure.type = Figure_Queen;
		}
	}

	if (board[newX][newY] == Figure_Empty) m.newLocationFigure = -1;
	else m.newLocationFigure = board[newX][newY];
	moves[movesIndex] = m;
	movesIndex++;
}

bool isFriendly(char board[8][8], Figure f, int x, int y, struct Figure figures[32])
{
	if (board[x][y] == Figure_Empty) return false;
	if (figures[board[x][y]].playerFigure == f.playerFigure) return true;
	else return false;
}

//Check if figure is under attack
bool isUnderAttack(Figure f, struct Figure figures[32], char board[8][8])
{
	int x, y;
	//check up
	x = f.x - 1;
	y = f.y;
	while (x >= 0 && board[x][y] == Figure_Empty) x--;
	if (x >= 0)
	{
		Figure attacker = figures[board[x][y]];
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
		Figure attacker = figures[board[x][y]];
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
		Figure attacker = figures[board[x][y]];
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
		Figure attacker = figures[board[x][y]];
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
		Figure attacker = figures[board[x][y]];
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
		Figure attacker = figures[board[x][y]];
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
		Figure attacker = figures[board[x][y]];
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
		Figure attacker = figures[board[x][y]];
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
		Figure attacker = figures[board[x - 2][y + 1]];
		if (attacker.playerFigure != f.playerFigure && attacker.type == Figure_Knight) return true;
	}
	if (x - 1 >= 0 && y + 2 < 8)
	{
		Figure attacker = figures[board[x - 1][y + 2]];
		if (attacker.playerFigure != f.playerFigure && attacker.type == Figure_Knight) return true;
	}
	//down right
	if (x + 1 < 8 && y + 2 < 8)
	{
		Figure attacker = figures[board[x + 1][y + 2]];
		if (attacker.playerFigure != f.playerFigure && attacker.type == Figure_Knight) return true;
	}
	if (x + 2 < 8 && y + 1 < 8)
	{
		Figure attacker = figures[board[x + 2][y + 1]];
		if (attacker.playerFigure != f.playerFigure && attacker.type == Figure_Knight) return true;
	}
	//down left
	if (x + 2 < 8 && y - 1 >= 0)
	{
		Figure attacker = figures[board[x + 2][y - 1]];
		if (attacker.playerFigure != f.playerFigure && attacker.type == Figure_Knight) return true;
	}
	if (x + 1 < 8 && y - 2 >= 0)
	{
		Figure attacker = figures[board[x + 1][y - 2]];
		if (attacker.playerFigure != f.playerFigure && attacker.type == Figure_Knight) return true;
	}
	//up left
	if (x - 1 >= 0 && y - 2 >= 0)
	{
		Figure attacker = figures[board[x - 1][y - 2]];
		if (attacker.playerFigure != f.playerFigure && attacker.type == Figure_Knight) return true;
	}
	if (x - 2 >= 0 && y - 1 >= 0)
	{
		Figure attacker = figures[board[x - 2][y - 1]];
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
			Figure attacker = figures[board[x][y]];
			if (!attacker.playerFigure && attacker.type == Figure_Peasant) return true;
		}
		x = f.x - 1;
		y = f.y - 1;
		if (x >= 0 && y >= 0 && board[x][y] != Figure_Empty)
		{
			Figure attacker = figures[board[x][y]];
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
			Figure attacker = figures[board[x][y]];
			if (attacker.playerFigure && attacker.type == Figure_Peasant) return true;
		}
		x = f.x + 1;
		y = f.y - 1;
		if (x >= 0 && y >= 0 && board[x][y] != Figure_Empty)
		{
			Figure attacker = figures[board[x][y]];
			if (attacker.playerFigure && attacker.type == Figure_Peasant) return true;
		}
	}

	return false;

}

void getAvailableMoves(Figure f, char board[8][8], struct Move moves[], int& movesIndex, struct Figure figures[32])
{
	bool friendly = f.playerFigure;

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
			int x, y;
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
		int x, y;
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
		int x, y;
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

/*
bool isTerminalNode(Board n)
{
// TODO: THIS
return true;
}
*/

void getAllAvailableMoves(char board[8][8], struct Move moves[], int& movesIndex, struct Figure figures[32], bool player)
{
	int oldI = 0;
	int start = player ? 16 : 0; //If we are checking player moves then start at figure 16, otherwise figure 0 for AI
	int end = start + 16;
	//Hardcoded for AI (figure index 0 to 15)
	for (int j = start; j < end; j++) {
		if (!figures[j].alive) continue; //If the figure is dead, then we skip it
										 //printf("Getting available moves for figure type %c\n", figures[j].type);
		getAvailableMoves(figures[j], board, moves, movesIndex, figures);
		for (int i = oldI; i < movesIndex; i++)
		{
			oldI++;
			//printf("Move %d: from %d %d to x:%d y:%d\n", i + 1, moves[i].figure.x, moves[i].figure.y, moves[i].newX, moves[i].newY);
		}
	}
}

// p - player
// e - enemy


struct Figure initFigure(FigureType type, bool alive, bool playerFigure, int x, int y)
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
		Figure f = figures[i];
		/*if(f.playerFigure) board[f.x][f.y] = f.type;
		else board[f.x][f.y] = f.type - 32;*/
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

struct Move evaluateMoves(char board[8][8], struct Figure figures[32], struct Move moves[], int movesIndex, bool playerMove = false)
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
			default:
				printf("Weird error in evaluateMoves() switch!");
			}
		}
		if (figures[board[tmp.oldX][tmp.oldY]].type == Figure_Peasant)
		{
			if (tmp.figure.playerFigure && tmp.newX == 0 || !tmp.figure.playerFigure && tmp.newX == 7) points += 175;
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

void printBoard(char board[8][8], struct Figure figures[32])
{
	printf("  0 1 2 3 4 5 6 7\n");
	for (int i = 0; i<8; i++)
	{
		printf("%d ", i);
		for (int j = 0; j < 8; j++)
		{
			char c;
			if (board[i][j] == Figure_Empty || !figures[board[i][j]].alive) c = Figure_Empty;
			else
			{
				c = figures[board[i][j]].type;
				if (figures[board[i][j]].playerFigure) c -= 32;
			}
			printf("%c ", c);
		}
		printf("\n");
	}
}

void makeMove(char board[8][8], struct Move m, struct Figure figures[32], bool realMove = false)
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
		if (realMove)
		{
			printf("Killed figure type: %c\n", figures[board[m.newX][m.newY]].type - 32);
			if (figures[board[m.newX][m.newY]].type == Figure_King)
			{
				gameOver = true;
				printf("King killed. Game finished.\n");
			}
		}

	}
	//printf("Moving %c from x:%d y:%d to x:%d y:%d\n", m.figure.type, m.figure.x, m.figure.y, m.newX, m.newY);
	//printf("%d %d\n", figures[1].x, figures[1].y);
	/*m.figure.x = m.newX;
	m.figure.y = m.newY;*/


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
		if (m.figure.playerFigure && m.oldX == 6 || !m.figure.playerFigure && m.oldX == 1) figures[board[m.newX][m.newY]].firstMove = true;
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


unsigned long long int numOfExecutions;

void copyHistory(int history[], int newHistory[])
{
	/*
	for (int i = 0; i < (START_DEPTH + 1); i++)
	{
	newHistory[i] = history[i];
	}
	*/
	for (int i = 0; i < (DEPTH_TO_CALC_CPU); i++)
	{
		newHistory[i] = history[i];
	}
}

struct MinimaxReturn minimax(char board[8][8], struct Figure figures[32], int depth, bool maximizingPlayer, int cost)
{


	numOfExecutions++;
	struct MinimaxReturn ret;
	ret.value = -99999;

	// first init of moves
	{
		struct Move moves[100];
		int movesIndex = 0;
		getAllAvailableMoves(board, moves, movesIndex, figures, !maximizingPlayer);
		evaluateMoves(board, figures, moves, movesIndex);
		for (int i = 0; i < movesIndex; i++) {

			//char newBoard[8][8];
			//struct Figure newFigures[32];

			if (moves[i].fatalMove == true)
			{
				ret.value = 555;
				ret.bestMove = moves[i];
				int s = Count();
				//for (int j = 0; j < s; j++) free(PopFront());
				for (int j = 0; j < s; j++) PopFront();
				return ret;
			}

			char newBoard[8][8];
			struct Figure newFigures[32];

			copyBoard(board, newBoard);
			copyFigures(figures, newFigures);
			makeMove(newBoard, moves[i], newFigures);

			/*
			int history[(START_DEPTH + 1)]; // = { -2,-2,-2,-2,-2, moves[i].points };
			for (int j = 0; j < (START_DEPTH + 1); j++)
			history[j] = 0;
			history[(START_DEPTH + 1) - 1] = moves[i].points;
			*/
			int history[DEPTH_TO_CALC_CPU]; // = { -2,-2,-2,-2,-2, moves[i].points };
			for (int j = 0; j < DEPTH_TO_CALC_CPU; j++)
				history[j] = 0;
			history[DEPTH_TO_CALC_CPU - 1] = moves[i].points;

			//printf("%d - %d\n", moves[i].points, history[4]);

			//if (moves[i].fatalMove != true)
			{
				//if (!maximizingPlayer) history[depth] += moves[i].points;
				//else history[depth] -= moves[i].points;
				//PushFront(newBoard, newFigures, depth - 1, !maximizingPlayer, -(cost + moves[i].points), moves[i]);

				PushFront(newBoard, newFigures, depth - 1, !maximizingPlayer, 0, moves[i], history);
			}/*
			 else
			 {
			 moves[i].points *= depth;
			 if (!maximizingPlayer) history[depth] += moves[i].points;
			 else history[depth] -= moves[i].points;
			 //PushFront(newBoard, newFigures, 0, !maximizingPlayer, -(cost + moves[i].points), moves[i]);
			 PushFront(newBoard, newFigures, 0, !maximizingPlayer, 0, moves[i], history);
			 }*/
			 //root->ind = i;
			 //if (i > 0)
			 //{
			 //	printf("%d %d", root->ind, root->next->ind);
			 //	printBoard(root->next->board, root->next->figures);
			 //}
			 //printBoard(newBoard, newFigures);

		}
	}



	//int bestLocalCost = -99999;
	//Move bestLocalMove;

	//int moveCosts[60];
	//Move bestMoves[60];
	//int moveCostIndex = 0;
	/*
	int bestCosts[(START_DEPTH + 1)];// = { -999999,-9999999,-9999999,-999999,-99999999 };
	Move bestFinalMoves[(START_DEPTH + 1)];
	for (int i = 0; i < (START_DEPTH + 1); i++)
	bestCosts[i] = -999999;
	*/

	int bestCosts[DEPTH_TO_CALC_CPU];// = { -999999,-9999999,-9999999,-999999,-99999999 };
	Move bestFinalMoves[DEPTH_TO_CALC_CPU];
	for (int i = 0; i < DEPTH_TO_CALC_CPU; i++)
		bestCosts[i] = -999999;





	while (Count() > 0)
	{

		int x = Count();
		Element el = PopFront();
		if (el.depth == 0)
		{

			if (el.costHistory[el.depth] >= bestCosts[el.depth])
			{

				bestCosts[el.depth] = el.costHistory[el.depth];
				bestFinalMoves[el.depth] = el.firstMove;

			}

			if (lastInd == 0 || elArr[0].depth > el.depth)
			{
				int tmpDepth = 1;
				int topDepth;
				if (lastInd == 0) topDepth = DEPTH_TO_CALC_CPU;
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
		getAllAvailableMoves(el.board, moves, movesIndex, el.figures, !el.AI);
		evaluateMoves(el.board, el.figures, moves, movesIndex);
		for (int i = 0; i < movesIndex; i++) {

			char newBoard[8][8];
			struct Figure newFigures[32];

			copyBoard(el.board, newBoard);
			copyFigures(el.figures, newFigures);
			makeMove(newBoard, moves[i], newFigures);

			el.costHistory[el.depth - 1] = moves[i].points;
			PushFront(newBoard, newFigures, el.depth - 1, !(el.AI), cost, el.firstMove, el.costHistory);
		}
	}





#pragma region Commented

	//while (Count() > 0)
	//{
	//	int x = Count();
	//	Element* el = PopFront();
	//	if (el->depth == 0)
	//	{
	//		//int bestLeafCost = -9999999;

	//		if (el->costHistory[el->depth] >= bestCosts[el->depth])
	//		{

	//			bestCosts[el->depth] = el->costHistory[el->depth];
	//			bestFinalMoves[el->depth] = el->firstMove;

	//		}

	//		if (el->next == NULL || el->next->depth > el->depth)
	//		{
	//			int tmpDepth = 1;
	//			int topDepth;
	//			if (el->next == NULL) topDepth = DEPTH_TO_CALC_CPU;
	//			else topDepth = el->next->depth + 1;

	//			while (topDepth > tmpDepth)
	//			{
	//				if (-bestCosts[tmpDepth - 1] + el->costHistory[tmpDepth] > bestCosts[tmpDepth])
	//				{
	//					bestCosts[tmpDepth] = -bestCosts[tmpDepth - 1] + el->costHistory[tmpDepth];
	//					bestFinalMoves[tmpDepth] = bestFinalMoves[tmpDepth - 1];
	//				}
	//				bestCosts[tmpDepth - 1] = -99999;
	//				tmpDepth++;
	//			}
	//			/*
	//			// curr lvl + prev orig > best prev
	//			if (bestCosts[el->depth + 1] <= bestEl.costHistory[bestEl.depth] + bestEl.costHistory[bestEl.depth + 1])
	//			{
	//			bestCosts[el->depth + 1] = bestEl.costHistory[bestEl.depth] + bestEl.costHistory[bestEl.depth + 1];
	//			}
	//			*/

	//		}



	//		free(el);
	//		continue;
	//	}

	//	/*
	//	if (el->costHistory[el->depth + 1] >= 1000)
	//	{
	//	el->costHistory[el->depth - 1] = 1000;
	//	if(el->depth-1 == 0) el->costHistory[el->depth - 1] = -4000;
	//	PushFront(el->board, el->figures, el->depth - 1, !(el->AI), cost, el->firstMove, el->costHistory);
	//	free(el);
	//	continue;
	//	}
	//	*/

	//	struct Move moves[100];
	//	int movesIndex = 0;
	//	getAllAvailableMoves(el->board, moves, movesIndex, el->figures, !el->AI);
	//	evaluateMoves(el->board, el->figures, moves, movesIndex);
	//	for (int i = 0; i < movesIndex; i++) {

	//		char newBoard[8][8];
	//		struct Figure newFigures[32];

	//		//char newBoard[8][8]; // = new char[8][8];
	//		//struct Figure newFigures[32]; // = new Figure[32];
	//		copyBoard(el->board, newBoard);
	//		copyFigures(el->figures, newFigures);
	//		makeMove(newBoard, moves[i], newFigures);
	//		//printBoard(newBoard, newFigures);


	//		// THIS HAS PROBLEMS WITH KING !!!
	//		/*
	//		if (moves[i].fatalMove == true)
	//		{
	//		moves[i].points *= el->depth;

	//		if((depth - el->depth) % 2 == 0) bestCosts[el->depth] = moves[i].points;
	//		else bestCosts[el->depth] = -moves[i].points;
	//		bestFinalMoves[el->depth] = el->firstMove;
	//		continue;
	//		}
	//		*/

	//		//if (el->AI) el->costHistory[el->depth] += moves[i].points;
	//		//else el->costHistory[el->depth] -= moves[i].points;

	//		el->costHistory[el->depth - 1] = moves[i].points;

	//		//PushFront(newBoard, newFigures, el->depth-1, !(el->AI), -(el->cost+moves[i].points), el->firstMove);
	//		//undoMove(board, moves[i], figures);
	//		//PushFront(newBoard, newFigures, el->depth - 1, !(el->AI), tmp, el->firstMove);

	//		/*
	//		if(moves[i].points == 200)
	//		{
	//		printf("qwe %d ;; ", moves[i].points);
	//		printBoard(el->board, el->figures);
	//		printBoard(newBoard, newFigures);
	//		}
	//		*/

	//		PushFront(newBoard, newFigures, el->depth - 1, !(el->AI), cost, el->firstMove, el->costHistory);
	//		//}
	//		/*
	//		else
	//		{
	//		moves[i].points *= el->depth;
	//		//if (el->AI) el->costHistory[el->depth] += moves[i].points;
	//		//else el->costHistory[el->depth] -= moves[i].points;

	//		el->costHistory[el->depth] = moves[i].points;

	//		//PushFront(newBoard, newFigures, 0, !el->AI, -(el->cost + moves[i].points), el->firstMove);
	//		//PushFront(newBoard, newFigures, 0, !el->AI, tmp, el->firstMove);
	//		PushFront(newBoard, newFigures, el->depth - 1, !(el->AI), cost, el->firstMove, el->costHistory);
	//		}*/
	//	}

	//	free(el);
	//}

#pragma endregion




	/*int currentDepth = 1;
	while (currentDepth <= depth)
	{

	FinalElement* curr = finalElementRoot;

	int bestCost = -99999;
	Move bestMove;

	while (curr != NULL)
	{
	if (bestCost <= curr->costHistory[currentDepth])
	{
	//Better cost at this depth
	}
	}

	currentDepth++;
	}*/


	// while queue not empty
	// compare
	// moves to queue
	//printf("%d\n", Count());

	//ret.value = costs[depth-1];
	//ret.bestMove = bestMoves[depth - 1];

	ret.value = bestCosts[DEPTH_TO_CALC_CPU - 1];
	ret.bestMove = bestFinalMoves[DEPTH_TO_CALC_CPU - 1];
	//for (int i = 0; i < DEPTH_TO_CALC_CPU; i++)
	//{
	//	printf("%d\n", bestCosts[i]);
	//}
	printf("%d\n", bestCosts[DEPTH_TO_CALC_CPU - 1]);
	return ret;
}

void randomAI(char board[8][8], struct Figure figures[32], bool isPlayer = false)
{
	//get all available moves this turn
	struct Move moves[100];
	int movesIndex = 0;
	//true - player moves, false - AI moves
	getAllAvailableMoves(board, moves, movesIndex, figures, isPlayer);

	//Take a random move and execute it
	int r = rand() % movesIndex;
	makeMove(board, moves[r], figures, true);

}

void bestMoveAI(char board[8][8], struct Figure figures[32])
{
	//get all available moves this turn
	struct Move moves[100];
	int movesIndex = 0;
	//true - player moves, false - AI moves
	getAllAvailableMoves(board, moves, movesIndex, figures, false);

	//Evaluate available moves and execute the most potential one
	struct Move m = evaluateMoves(board, figures, moves, movesIndex);
	makeMove(board, m, figures, true);

}
double totalTime = 0;
int i;
int numofMoves = 0;
void miniMaxAI(char board[8][8], struct Figure figures[32], int depth, bool AI = true)
{
	lastInd = 0;

	numOfExecutions = 0;
	clock_t begin = clock();

	struct MinimaxReturn mRet = minimax(board, figures, depth, AI, 0);

	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	printf("Used %lf seconds.\n", elapsed_secs);
	//printf("%d::%lf::%llu::%d::release\n", i, elapsed_secs, numOfExecutions, depth);
	totalTime += elapsed_secs;
	struct Move bestMove = mRet.bestMove;
	numofMoves++;
	printf("%d %d -> %d %d : %d\n", bestMove.oldX, bestMove.oldY, bestMove.newX, bestMove.newY, mRet.value);

	makeMove(board, bestMove, figures, true);


}


void playerMove(char board[8][8], struct Figure figures[32], bool player = true)
{
	int fx, fy, newX, newY;

	bool validMove = false;
	struct Move moves[100];
	int movesIndex = 0;

	struct Move m;


	//we get user input and check if it is valid; we do that until we get a valid move
	do {
		//Input format: figureX, figureY, newX, newY
		scanf_s("%d %d %d %d", &fx, &fy, &newX, &newY);

		if (fx == -1)
		{
			undoMove(board, m, figures);
			continue;
		}

		movesIndex = 0;
		getAllAvailableMoves(board, moves, movesIndex, figures, player);



		for (int i = 0; i < movesIndex; i++)
		{
			m = moves[i];
			//printf("%d-%d %d-%d %d-%d %d-%d\n", m.figure.x, fx, m.figure.y, fy, m.newX, newX, m.newY, newY);
			if (m.figure.x == fx && m.figure.y == fy && m.newX == newX && m.newY == newY)
			{
				validMove = true;
				break;
			}
		}

		if (!validMove) printf("Invalid move. Try again.\n");

	} while (!validMove);


	//execute the move
	makeMove(board, m, figures, true);

}

void gameLoop(char board[8][8], struct Figure figures[32])
{
	srand(time(NULL));
	printBoard(board, figures);

	while (true)
	{
		//printf("**** PLAYER MOVE ****\n");
		//playerMove(board, figures);
		randomAI(board, figures, true);
		//miniMaxAI(board, figures, DEPTH_TO_CALC_CPU, false);
		//Print the board to see the result
		//bestMoveAI(board, figures);
		//printBoard(board, figures);

		if (gameOver)
		{
			printf("**** Player won. ****\n");
			break;
		}

		//printf("****** AI MOVE ******\n");
		//randomAI(board, figures);
		//bestMoveAI(board, figures);
		miniMaxAI(board, figures, DEPTH_TO_CALC_CPU, true);
		//playerMove(board, figures, false);
		//Print the board to see the result
		//printBoard(board, figures);

		//if (isUnderAttack(figures[16], figures, board))
		//{
		//	printf("UNDER ATTACK\n");
		//}


		if (gameOver)
		{
			printf("**** AI won. ****\n");
			break;
		}

	}




	//TESTING
	/*for (int i = 0; i < 100; i++)
	randomAI(board, figures);*/

}

int main()
{
	//for (i = 0; i<5; i++)
	{
		//printf("NEW\n");

		char board[8][8];
		struct Figure figures[32];
		for (int i = 0; i < 100; i++) {
			//INIT
			initChessboard(figures, board);
			//printBoard(board, figures);
			gameOver = false;

			//MAIN LOOP
			gameLoop(board, figures);
		}

		printf("TOTAL %lf MOVES %d", totalTime, numofMoves);

	}

#pragma region Old testing
	//TESTING getAvailableMoves function
	/*
	struct Move moves[100];
	int movesIndex = 0;


	int oldI = 0;
	for (int j = 0; j < 16; j++) {
	printf("Getting available moves for figure type %c\n", figures[j].type);
	getAvailableMoves(figures[j], board, moves, movesIndex, figures);
	for (int i = oldI; i < movesIndex; i++)
	{
	oldI++;
	printf("Move %d: x:%d y:%d\n", i + 1, moves[i].newX, moves[i].newY);
	}
	}*/
	//END TESTING
#pragma endregion


	scanf_s("%d", NULL);
	return 0;
}

