#include "stdio.h"



struct Figure
{

	char type;
	int x;
	int y;

	bool playerFigure;
	bool alive;
};

struct Move
{
	char board[8][8];
	Figure figure;
	int newX, newY;

};

/*
bool isTerminalNode(Board n)
{
	// TODO: THIS
	return true;
}
int minimax(Board node, int depth, bool maximizingPlayer)
{
	if (depth == 0 || isTerminalNode(node))
		return cost(node);

	if (maximizingPlayer)
	{
		int bestValue = -999999;
		for each child of node
		{
			int v = minimax(child, depth - 1, FALSE);
			bestValue = max(bestValue, v);
		}
		return bestValue;
	}
	else // (*minimizing player *)
	{
		int bestValue = 999999;
		for each child of node
		{
			int v = minimax(child, depth - 1, TRUE);
			bestValue = min(bestValue, v);
		}
		return bestValue;
	}
}
*/


// p - player
// e - enemy
enum FigureType
{
	Figure_ePeasant = 'P',
	Figure_pPeasant = 'p',
	Figure_eRook = 'R',
	Figure_pRook = 'r',
	Figure_eKnight = 'H',
	Figure_pKnight = 'h',
	Figure_eBishop = 'B',
	Figure_pBishop = 'b',
	Figure_eQueen = 'Q',
	Figure_pQueen = 'q',
	Figure_eKing = 'K',
	Figure_pKing = 'k'
};

struct Figure initFigure(FigureType type, bool alive, bool playerFigure, int x, int y)
{
	struct Figure f;
	f.type = type;
	f.alive = alive;
	f.playerFigure = playerFigure;
	f.x = x;
	f.y = y;
	return f;
}

void refreshBoard(struct Figure figures[32], char board[8][8])
{
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			board[i][j] = '.';
	for (int i = 0; i < 32; i++)
	{
		Figure f = figures[i];
		board[f.x][f.y] = f.type;
	}
}

void initChessboard(struct Figure figures[32], char board[8][8])
{
	figures[0] = initFigure(Figure_eRook, 1, 0, 0, 0);
	figures[1] = initFigure(Figure_eKnight, 1, 0, 0, 1);
	figures[2] = initFigure(Figure_eBishop, 1, 0, 0, 2);
	figures[3] = initFigure(Figure_eKing, 1, 0, 0, 3);
	figures[4] = initFigure(Figure_eQueen, 1, 0, 0, 4);
	figures[5] = initFigure(Figure_eBishop, 1, 0, 0, 5);
	figures[6] = initFigure(Figure_eKnight, 1, 0, 0, 6);
	figures[7] = initFigure(Figure_eRook, 1, 0, 0, 7);
	for (int i = 0; i < 8; i++) figures[8+i] = initFigure(Figure_ePeasant, 1, 0, 1, i);

	figures[16] = initFigure(Figure_pRook, 1, 1, 7, 0);
	figures[17] = initFigure(Figure_pKnight, 1, 1, 7, 1);
	figures[18] = initFigure(Figure_pBishop, 1, 1, 7, 2);
	figures[19] = initFigure(Figure_pKing, 1, 1, 7, 3);
	figures[20] = initFigure(Figure_pQueen, 1, 1, 7, 4);
	figures[21] = initFigure(Figure_pBishop, 1, 1, 7, 5);
	figures[22] = initFigure(Figure_pKnight, 1, 1, 7, 6);
	figures[23] = initFigure(Figure_pRook, 1, 1, 7, 7);
	for (int i = 0; i < 8; i++) figures[24 + i] = initFigure(Figure_pPeasant, 1, 0, 6, i);

	
	refreshBoard(figures, board);
	
}



void printBoard(char board[8][8])
{
	for (int i = 0; i<8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			printf("%c ", board[i][j]);
		}
		printf("\n");
	}
}

int main()
{
	char board[8][8];
	/*
	for(int i=0; i<8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			board[i][j] = ' ';
		}
	}
	*/
	struct Figure figures[32];
	initChessboard(figures, board);
	printBoard(board);

	scanf_s("%d", NULL);
	return 0;
}

