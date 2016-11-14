#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "main.h"

int main(int argc, char *argv[])
{
	if (argc == 0)
	{
		perror("Error: No Arguments Given");
		return(-1);
	}

	//Error variables
	errno_t errorW = 0;	//write error
	errno_t errorM = 0;	//maze open error
	errno_t errorS = 0; //solution open error

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

	//Create an instance of the MAZEDATA struct to store all data related to the maze
	MAZEDATA mazeData;

	//read in maze size, start and end location, store XY values in Coordinate Struct
	//COORD sizeMaze, startMaze, endMaze;
	int foundValues;	//Store the number of successful values read
	foundValues = fscanf_s(mazeFile, "%d %d %d %d %d %d ",
		&mazeData.sizeMaze.X,	&mazeData.sizeMaze.Y,
		&mazeData.startMaze.X,	&mazeData.startMaze.Y,
		&mazeData.endMaze.X,	&mazeData.endMaze.Y);
	//Check if all values successfully read in, should have 6 values
	if (foundValues != 6)
	{
		printf("Error reading in Size, starting location and End location");
		return (-1);
	}
	
	//Create Maze Array ( Using Malloc for Dynamic Array setting)
	mazeData.maze = create2DArray(mazeData.sizeMaze);//create 2D array	

	//Call GetMazeData to read in the Maze values.
	getMazeData(mazeData.maze, mazeFile, mazeData.sizeMaze);

	//Sets the initial Step number to Zero
	mazeData.stepNumber = 0;
	
	//Set initial position to start location
	mazeData.position = mazeData.startMaze;

	//Find a Path through the maze (returns true is path found)
	int pathFound = pathFinder(&mazeData);
	//int pathFound = pathFinder(maze, pathArray, &stepNumber, &position, endMaze);

	//If a path was found, convert the steps into directions (U,D,L,R)
	//and write the direction (append) to the solution file
	if (pathFound)
	{	//Loop through all steps up to step number
		//Compare current location with next location
		
		//write number of moves required to solve maze to solutionFile
		errorW = fprintf_s(solutionFile,"%d",mazeData.stepNumber);	
		errorW = fprintf_s(solutionFile, "\n");			//add new line

		int i;
		for (i = 0; i < mazeData.stepNumber; i++)
		{
			char xMove = mazeData.pathArray[i + 1].X - mazeData.pathArray[i].X;
			char yMove = mazeData.pathArray[i + 1].Y - mazeData.pathArray[i].Y;

			if (xMove)
			{	//if xMove is non-zero; Move was in x direction
				switch (xMove)	//swtich case for finding direction on selected axis
				{
				case 1:		//Case: moved Right (R)
					errorW = fprintf_s(solutionFile, "R ");
					break;

				case -1:	//Case: moved Left (L)
					errorW = fprintf_s(solutionFile, "L ");
					break;

				default:	//Case: Error
					break;
				}
			}
			else
			{	//if yMove is non-zero; Move was in y direction
				switch (yMove)	//swtich case for finding direction on selected axis
				{
				case 1:		//Case: moved Down (D)
					errorW = fprintf_s(solutionFile, "D ");
					break;

				case -1:	//Case: moved Up (U)
					errorW = fprintf_s(solutionFile, "U ");
					break;

				default:	//Case: Error
					break;
				}
			}
		}
	}
	else
	{
		printf("No Path found, Maze not solved!");
	}




//Debuging Print 
	printf("\n Size:	%d-%d\n Start:	%d - %d\n End:	%d-%d",
		mazeData.sizeMaze.X,	mazeData.sizeMaze.Y,
		mazeData.startMaze.X,	mazeData.startMaze.Y,
		mazeData.endMaze.X,		mazeData.endMaze.Y);

	//int i,j;
	//printf("\n");
	//for (j = 0; j < sizeMaze.Y; j++)
	//{
	//	for (i = 0; i < sizeMaze.X; i++)
	//	{
	//		printf(" %d", maze[i][j]);
	//	}
	//	printf("\n");
	//}

	printf("\nMaze Route Found: %d", pathFound);
	printf("\nNumber of Steps: %d", mazeData.stepNumber);

//End Debuging Print

	while (True);
	//Close the file
	free(mazeData.maze);
	fclose(mazeFile);

	return(0);
}

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
	return;
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
	default:	//all walls (value 15) should never enter into this position
		walls.U = 1;
		walls.L = 1;
		walls.D = 1;
		walls.R = 1;
		return(walls);
	}
}

char moveUpMaze(MAZEDATA* mData)
{	//Can move in Up direction

	//Prepare to move (recursive step):
	//Add current pos to pathArray
	mData->pathArray[mData->stepNumber].X = mData->position.X;
	mData->pathArray[mData->stepNumber].Y = mData->position.Y;
	//increment stepNumber
	mData->stepNumber += 1;
	//move pos up
	mData->position.Y--;

	//Enter Next level of recursion
	char pathFound = pathFinder(mData);
	return(pathFound);
}

char pathFinder(MAZEDATA* mData)
{
	//char 'Boolean" for when path is found
	char pathFound = False;
	
	//Check if current position is the End location (then End)
	if (mData->position.X == mData->endMaze.X && mData->position.Y == mData->endMaze.Y)
	{	//End has been reached

		//Add current pos to pathArray
		mData->pathArray[mData->stepNumber].X = mData->position.X;
		mData->pathArray[mData->stepNumber].Y = mData->position.Y;
		//increment stepNumber
		mData->stepNumber += 1;
		// Rutern pathFound true
		return(True);
	}
	else
	{
		//Check if current position is on current path, then check movement directions and enter next level of recursion
		int i;
		char overlap = 0;
		//Step through till currrent number of steps taken reached (to prevent checking unused steps in array)
		//not including current step (not added to array yet) hence 'stepNumber-1'
		for (i = 0; i < mData->stepNumber-1; i++)
		{
			//loop Through all visited locations in pathArray and check if overlapping
			if (mData->position.X == mData->pathArray[i].X && mData->position.Y == mData->pathArray[i].Y)
			{
				overlap = 1; // Set overlap to True
			}
		}

		if (overlap)
		{	// path overlaps path already taken, reverse and try another route.
		
			// Dont add new position to pathArray
			// decrement stepNumber
			mData->stepNumber -= 1;
			// set pos to previous position from pathArray
			mData->position.X = mData->pathArray[mData->stepNumber].X;
			mData->position.Y = mData->pathArray[mData->stepNumber].Y;
			
			// Return pathFound False
			return(False);
		}
		else
		{	//Current Location is not on path already taken, look for new direction to move in.
			//Check which directions are free to move in, and where the path was found in a previous direction and isreturning from recursion
			//Convert to "binary" values, where 1 indecates a wall in that direction.
			WALLS walls = toBinary(mData->maze[mData->position.X][mData->position.Y]);
			if (walls.U == 0 && !pathFound)
			{	//Can move in Up direction

				pathFound = moveUpMaze(mData);
				////Prepare to move (recursive step):
				////Add current pos to pathArray
				//mData->pathArray[mData->stepNumber].X = mData->position.X;
				//mData->pathArray[mData->stepNumber].Y = mData->position.Y;
				////increment stepNumber
				//mData->stepNumber += 1;
				////move pos up
				//mData->position.Y--;

				////Enter Next level of recursion
				//pathFound = pathFinder(mData);
			}

			if (walls.D == 0 && !pathFound)
			{	//Can move in Down direction
			
				//Prepare to move (recursive step):
				//Add current pos to pathArray
				mData->pathArray[mData->stepNumber].X = mData->position.X;
				mData->pathArray[mData->stepNumber].Y = mData->position.Y;
				//increment stepNumber
				mData->stepNumber += 1;
				//move pos down
				mData->position.Y++;

				//Enter Next level of recursion
				pathFound = pathFinder(mData);
			}

			if (walls.L == 0 && !pathFound)
			{	//Can move in Left direction
				
				//Prepare to move (recursive step):				
				//Add current pos to pathArray
				mData->pathArray[mData->stepNumber].X = mData->position.X;
				mData->pathArray[mData->stepNumber].Y = mData->position.Y;
				//increment stepNumber
				mData->stepNumber += 1;
				//move pos left
				mData->position.X--;

				//Enter Next level of recursion
				pathFound = pathFinder(mData);
			}

			if (walls.R == 0 && !pathFound)
			{	//Can move in Right direction

				//Prepare to move (recursive step):
				//Add current pos to pathArray
				mData->pathArray[mData->stepNumber].X = mData->position.X;
				mData->pathArray[mData->stepNumber].Y = mData->position.Y;
				//increment stepNumber
				mData->stepNumber += 1;
				//move pos right
				mData->position.X++;

				//Enter Next level of recursion
				pathFound = pathFinder(mData);
			}
			
			if (pathFound == 0)
			{	//if Path still not found, then posible deadend encountered and so
				//return 0 to upper level and reset all approperate values
				
				// Dont add new position to pathArray
				// decrement stepNumber
				mData->stepNumber -= 1;
				// set pos to previous position from pathArray
				mData->position.X = mData->pathArray[mData->stepNumber].X;
				mData->position.Y = mData->pathArray[mData->stepNumber].Y;

				// Return pathFound False
				return(False);
			}
			else
			{	//Path has been found, return 1!
				return(True);
			}
		}
	}
}
