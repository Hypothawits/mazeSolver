#pragma once
typedef struct myCOORD {
	//Struct type for holding X, Y coordinate data
	int X;
	int Y;
}myCOORD;

typedef struct WALLS {
	//Struct type for holding 4 bit binary number
	//where b0 b1 b2 b3
	//	=	U  L  D  R      
	char U; char L; char D;  char R;
}WALLS;

typedef struct MAZEDATA
{
	//Maze data
	int**	maze;
	int**	mazeVisit;

	//Start and End locations
	myCOORD	endMaze;
	myCOORD	startMaze;

	//MAze Size
	myCOORD	sizeMaze;

	//A Coordinate Array. So each step of the path will have an x and y value
	myCOORD	pathArray[500000];	//assumes that maze path will take less than 1 Million 
								//Current position in the maze
	myCOORD	position;

	//number of steps taken
	int		stepNumber;

}MAZEDATA;

typedef enum
{
	False = 0,
	True = 1
}BOOL;


void	quitError();
void	getMazeParam(FILE* mazeFile, MAZEDATA* mazeData);
void	getMazeData(FILE* mazeFile, MAZEDATA* mData);

void	create2DArray(MAZEDATA* mData);
WALLS	toBinary(int decimal);

char	moveUpMaze(MAZEDATA* mData);
char	moveDownMaze(MAZEDATA* mData);
char	moveRightMaze(MAZEDATA* mData);
char	moveLeftMaze(MAZEDATA* mData);
char	pathFinder(	MAZEDATA* mData);

void	checkFile(FILE* filePointer, char* fileName1);
void	writeSolutionFile(MAZEDATA* mazeData, FILE* solutionFile);