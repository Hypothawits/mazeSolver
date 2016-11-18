#pragma once
typedef struct COORD {
	//Struct type for holding X, Y coordinate data
	int X;
	int Y;
}COORD;

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

	//Start and End locations
	COORD	endMaze;
	COORD	startMaze;

	//MAze Size
	COORD	sizeMaze;

	//A Coordinate Array. So each step of the path will have an x and y value
	COORD	pathArray[1000000];	//assumes that maze path will take less than 1 Million 
								//Current position in the maze
	COORD	position;

	//number of steps taken
	int		stepNumber;

}MAZEDATA;

typedef enum
{
	False = 0,
	True = 1
}BOOL;

void	getMazeData(int** maze, FILE* mazeFile, COORD size);
int**	create2DArray(COORD size);
WALLS	toBinary(int decimal);
char	moveUpMaze(MAZEDATA* mData);
char	moveDownMaze(MAZEDATA* mData);
char	moveRightMaze(MAZEDATA* mData);
char	moveLeftMaze(MAZEDATA* mData);
char	pathFinder(MAZEDATA* mData);