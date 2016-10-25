#include "stdio.h"



// p - player
// e - enemy
enum Figure
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

void initChessboard(char board[8][8])
{
	board[0][0] = Figure_eRook;
	board[0][1] = Figure_eKnight;
	board[0][2] = Figure_eBishop;
	board[0][3] = Figure_eKing;
	board[0][4] = Figure_eQueen;
	board[0][5] = Figure_eBishop;
	board[0][6] = Figure_eKnight;
	board[0][7] = Figure_eRook;
	for (int i = 0; i < 8; i++) board[1][i] = Figure_ePeasant;

	board[7][0] = Figure_pRook;
	board[7][1] = Figure_pKnight;
	board[7][2] = Figure_pBishop;
	board[7][3] = Figure_pKing;
	board[7][4] = Figure_pQueen;
	board[7][5] = Figure_pBishop;
	board[7][6] = Figure_pKnight;
	board[7][7] = Figure_pRook;
	for (int i = 0; i < 8; i++) board[6][i] = Figure_pPeasant;
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
	
	for(int i=0; i<8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			board[i][j] = ' ';
		}
	}
	
	initChessboard(board);
	printBoard(board);

	scanf_s("%d", NULL);
	return 0;
}

