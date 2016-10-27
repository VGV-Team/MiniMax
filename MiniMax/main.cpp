#include "stdio.h"

// TODO: sestopanje
// TODO: ne refresh tabel - treba je narediti s sestopanjem
// TODO: rušada, menjava peasanta za neki bolsga

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


void getAvailableMoves(Figure f, char board[8][8], struct Move moves[])
{
	if(f.type == Figure_Peasant)
	{
		if(f.playerFigure)
		{
			// look up
			if (f.x + 1<8 && board[f.x + 1][f.y] == Figure_Empty)
			{
				// add to moves
			}
			if (board[f.x + 1][f.y+1] != Figure_Empty)
			{
				// add to moves
			}
			if (board[f.x + 1][f.y - 1] != Figure_Empty)
			{
				// add to moves
			}
			
		}
		else 
		{
			if (board[f.x - 1][f.y] == Figure_Empty)
				// add to moves
		}
	}
	if (f.type == Figure_Rook)
	{

	}
	if (f.type == Figure_Knight)
	{

	}
	if (f.type == Figure_Bishop)
	{

	}
	if (f.type == Figure_Queen)
	{

	}
	if (f.type == Figure_King)
	{

	}

}

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
			board[i][j] = Figure_Empty;
	for (int i = 0; i < 32; i++)
	{
		Figure f = figures[i];
		if(f.playerFigure) board[f.x][f.y] = f.type;
		else board[f.x][f.y] = f.type - 32;
		
	}
}

void initChessboard(struct Figure figures[32], char board[8][8])
{
	figures[0] = initFigure(Figure_Rook, 1, 0, 0, 0);
	figures[1] = initFigure(Figure_Knight, 1, 0, 0, 1);
	figures[2] = initFigure(Figure_Bishop, 1, 0, 0, 2);
	figures[3] = initFigure(Figure_King, 1, 0, 0, 3);
	figures[4] = initFigure(Figure_Queen, 1, 0, 0, 4);
	figures[5] = initFigure(Figure_Bishop, 1, 0, 0, 5);
	figures[6] = initFigure(Figure_Knight, 1, 0, 0, 6);
	figures[7] = initFigure(Figure_Rook, 1, 0, 0, 7);
	for (int i = 0; i < 8; i++) figures[8+i] = initFigure(Figure_Peasant, 1, 0, 1, i);

	figures[16] = initFigure(Figure_Rook, 1, 1, 7, 0);
	figures[17] = initFigure(Figure_Knight, 1, 1, 7, 1);
	figures[18] = initFigure(Figure_Bishop, 1, 1, 7, 2);
	figures[19] = initFigure(Figure_King, 1, 1, 7, 3);
	figures[20] = initFigure(Figure_Queen, 1, 1, 7, 4);
	figures[21] = initFigure(Figure_Bishop, 1, 1, 7, 5);
	figures[22] = initFigure(Figure_Knight, 1, 1, 7, 6);
	figures[23] = initFigure(Figure_Rook, 1, 1, 7, 7);
	for (int i = 0; i < 8; i++) figures[24 + i] = initFigure(Figure_Peasant, 1, 0, 6, i);

	
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

