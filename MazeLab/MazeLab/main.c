#include <stdio.h>
#include <string.h>
#include "main.h"

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

//add Bool typedef to make code easier to read?
typedef enum
{
	false =0,
	true = 1

}Bool;

void getMazeData(int** maze, FILE* mazeFile, COORD size)
{	//Gets the Maze values out of the Text File
	//Incrementors
	int i, j;
	//Get pointer to the current position (after previous read)
	char* pch = &mazeFile[0];
	int mazeValue;
	//Load in 
	for (j = 0; j < size.Y; j++)
	{
		for (i = 0; i < size.X; i++)
		{
			fscanf_s(pch, "%d", &mazeValue);	//Pointer increments on each function call
			maze[i][j] = mazeValue;				//Stores 
		}
	}

	return(1);
}

int** create2DArray(COORD size) 
{	//Creates a double pointer and Creates a 2D array of given size.
	//create the Number of Columns, with data size int
	int** newArray;
	newArray = malloc(size.X * sizeof(int));		
	
	//Loop through the Number of Columns Creating the Number of Rows
	int i;
	for (i = 0; i<size.Y; i++)					
	{
		newArray[i] = malloc(size.Y * sizeof(int));
	}
	return(newArray);
}

WALLS toBinary(int decimal) {
	// Where 1 indecates a wall
	WALLS walls;
	switch (decimal)
	{
	case 0:
		walls.U = 0;
		walls.L = 0;
		walls.D = 0;
		walls.R = 0;
		return(walls);
	case 1:
		walls.U = 0;
		walls.L = 0;
		walls.D = 0;
		walls.R = 1;
		return(walls);
	case 2:
		walls.U = 0;
		walls.L = 0;
		walls.D = 1;
		walls.R = 0;
		return(walls);
	case 3:
		walls.U = 0;
		walls.L = 0;
		walls.D = 1;
		walls.R = 1;
		return(walls);
	case 4:
		walls.U = 0;
		walls.L = 1;
		walls.D = 0;
		walls.R = 0;
		return(walls);
	case 5:
		walls.U = 0;
		walls.L = 1;
		walls.D = 0;
		walls.R = 1;
		return(walls);
	case 6:
		walls.U = 0;
		walls.L = 1;
		walls.D = 1;
		walls.R = 0;
		return(walls);
	case 7:
		walls.U = 0;
		walls.L = 1;
		walls.D = 1;
		walls.R = 1;
		return(walls);
	case 8:
		walls.U = 1;
		walls.L = 0;
		walls.D = 0;
		walls.R = 0;
		return(walls);
	case 9:
		walls.U = 1;
		walls.L = 0;
		walls.D = 0;
		walls.R = 1;
		return(walls);
	case 10:
		walls.U = 1;
		walls.L = 0;
		walls.D = 1;
		walls.R = 0;
		return(walls);
	case 11:
		walls.U = 1;
		walls.L = 0;
		walls.D = 1;
		walls.R = 1;
		return(walls);
	case 12:
		walls.U = 1;
		walls.L = 1;
		walls.D = 0;
		walls.R = 0;
		return(walls);
	case 13:
		walls.U = 1;
		walls.L = 1;
		walls.D = 0;
		walls.R = 1;
		return(walls);
	case 14:
		walls.U = 1;
		walls.L = 1;
		walls.D = 1;
		walls.R = 0;
		return(walls);
	case 15:
		walls.U = 1;
		walls.L = 1;
		walls.D = 1;
		walls.R = 1;
		return(walls);
	}
}

int pathFinder(int** maze, COORD* pathArray, char *stepNumber, COORD* pos, COORD endMaze)
{
	//char 'Boolean" for when path is found
	char pathFound = 0;

	//Check if current position is the End location (then End)
	if (pos[0].X == endMaze.X && pos[0].Y == endMaze.Y)
	{	//End has been reached

		//Add current pos to pathArray
		pathArray[*stepNumber].X = pos[0].X;
		pathArray[*stepNumber].Y = pos[0].Y;
		//increment stepNumber
		*stepNumber += 1;
		// Rutern pathFound true
		return(1);	
	}
	else
	{
		//Check if current position is on current path, then check movement directions and enter next level of recursion
		int i;
		char overlap = 0;
		//Step through till currrent number of steps taken reached (to prevent checking unused steps in array)
		//not including current step (not added to array yet) hence 'stepNumber-1'
		for (i = 0; i < *stepNumber-1; i++)
		{
			//loop Through all visited locations in pathArray and check if overlapping
			if (pos[0].X == pathArray[i].X && pos[0].Y == pathArray[i].Y)
			{
				overlap = 1; // Set overlap to true
			}
		}

		if (overlap)
		{	// path overlaps path already taken, reverse and try another route.
		
			// Dont add new position to pathArray
			// decrement stepNumber
			*stepNumber -= 1;
			// set pos to previous position from pathArray
			pos[0].X = pathArray[*stepNumber].X;
			pos[0].Y = pathArray[*stepNumber].Y;
			
			// Return pathFound False
			return(0); 
		}
		else
		{	//Current Location is not on path already taken, look for new direction to move in.
			//Check which directions are free to move in, and where the path was found in a previous direction and isreturning from recursion
			//Convert to "binary" values, where 1 indecates a wall in that direction.
			WALLS walls = toBinary(maze[pos[0].X][pos[0].Y]);
			if (walls.U == 0 && !pathFound)
			{	//Can move in Up direction

				//Prepare to move (recursive step):
				//Add current pos to pathArray
				pathArray[*stepNumber].X = pos[0].X;
				pathArray[*stepNumber].Y = pos[0].Y;
				//increment stepNumber
				*stepNumber += 1;
				//move pos up
				pos[0].Y--;

				//Enter Next level of recursion
				pathFound = pathFinder(maze, pathArray, stepNumber, pos, endMaze);
			}

			if (walls.D == 0 && !pathFound)
			{	//Can move in Down direction
			
				//Prepare to move (recursive step):
				//Add current pos to pathArray
				pathArray[*stepNumber].X = pos[0].X;
				pathArray[*stepNumber].Y = pos[0].Y;
				//increment stepNumber
				*stepNumber += 1;
				//move pos down
				pos[0].Y++;

				//Enter Next level of recursion
				pathFound = pathFinder(maze, pathArray, stepNumber, pos, endMaze);
			}

			if (walls.L == 0 && !pathFound)
			{	//Can move in Left direction
				
				//Prepare to move (recursive step):				
				//Add current pos to pathArray
				pathArray[*stepNumber].X = pos[0].X;
				pathArray[*stepNumber].Y = pos[0].Y;
				//increment stepNumber
				*stepNumber += 1;
				//move pos left
				pos[0].X--;

				//Enter Next level of recursion
				pathFound = pathFinder(maze, pathArray, stepNumber, pos, endMaze);
			}

			if (walls.R == 0 && !pathFound)
			{	//Can move in Right direction

				//Prepare to move (recursive step):
				//Add current pos to pathArray
				pathArray[*stepNumber].X = pos[0].X;
				pathArray[*stepNumber].Y = pos[0].Y;
				//increment stepNumber
				*stepNumber += 1;
				//move pos right
				pos[0].X++;

				//Enter Next level of recursion
				pathFound = pathFinder(maze, pathArray, stepNumber, pos, endMaze);
			}
			
			if (pathFound == 0)
			{	//if Path still not found, then posible deadend encountered and so
				//return 0 to upper level and reset all approperate values
				
				// Dont add new position to pathArray
				// decrement stepNumber
				*stepNumber -= 1;
				// set pos to previous position from pathArray
				pos[0].X = pathArray[*stepNumber].X;
				pos[0].Y = pathArray[*stepNumber].Y;

				// Return pathFound False
				return(0);
			}
			else
			{	//Path has been found, return 1!
				return(1);
			}
		}
	}
	return(1);
}

int main(int argc, char *argv[])
{
	if (argc == 0)
	{
		perror("Error: No Arguments Given");
		return(-1);
	}

	//Error variables
	errno_t error = 0;	//general error ####remove if unused
	errno_t errorM = 0;	//maze error
	errno_t errorS = 0; //solution error

	//get file name from userer
	char* mazeFileName = argv[1];
	char* solutionFileName = argv[2];

	//open files, store any errors
	FILE *mazeFile;
	FILE *solutionFile;

	errorM = fopen_s(&mazeFile, mazeFileName, "r");
	errorS = fopen_s(&solutionFile, solutionFileName, "w");

	//Check if the file opened successfully
	if ((mazeFile == NULL) || (errorM != 0))
	{
		perror("Error: Maze File Failed to Load");
		return(-1);
	}
	else if ((solutionFile == NULL) || (errorS != 0))
	{
		perror("Error: Solution File Failed to Load");
		return(-1);
	}
	else
	{
		printf("Files Loaded");
	}

	//read in maze size, start and end location, store XY values in Coordinate Struct
	COORD sizeMaze, startMaze, endMaze;
	int foundValues;	//Store the number of successful values read
	foundValues = fscanf_s(mazeFile, "%d %d %d %d %d %d ",
		&sizeMaze.X, &sizeMaze.Y,
		&startMaze.X, &startMaze.Y,
		&endMaze.X, &endMaze.Y);
	//Check if all values successfully read in, should have 6 values
	if (foundValues != 6)
	{
		printf("Error reading in Size, starting location and End location");
		return (-1);
	}

	//Create Maze Array ( Using Malloc for Dynamic Array setting)
	int **maze = create2DArray(sizeMaze);		//create 2D array

	//Call GetMazeData to read in the Maze values.
	getMazeData(maze, mazeFile, sizeMaze);


	//Creates a Coordinate Array. So each step of the path will have an x and y value
	COORD pathArray[1000];
	char stepNumber = 0;
	
	//Set Position to start location
	COORD position;
	position.X = startMaze.X;
	position.Y = startMaze.Y;


	int pathFound = pathFinder(maze, pathArray, &stepNumber, &position, endMaze);



//Debuging Print 
	printf("\n Size:%d-%d\n Start:%d-%d\n End:%d-%d",
		sizeMaze.X, sizeMaze.Y,
		startMaze.X, startMaze.Y,
		endMaze.X, endMaze.Y);

	int i,j;
	printf("\n");
	for (j = 0; j < sizeMaze.Y; j++)
	{
		for (i = 0; i < sizeMaze.X; i++)
		{
			printf(" %d", maze[i][j]);
		}
		printf("\n");
	}

	printf("\nMaze Route Found? %d", pathFound);

//End Debuging Print

	while (1);
	//Close the file
	free(maze);
	fclose(mazeFile);
	return(0);
}
