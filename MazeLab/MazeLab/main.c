#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <windows.h>
#include "main.h"


int main(int argc, char *argv[])
{
	char* guiArg;			//should maze be drawn
	char* mazeFileName;		//Hold maze file name
	char* solutionFileName;	//hold solution file name
	char  error = False;	//General Error Variable

							//Check the number of arguments passed and allocation to correct variable.
	switch (argc)
	{
	case 3:	//Only file names passed
		mazeFileName	 = argv[1];
		solutionFileName = argv[2];
		break;

	case 4:	//files and gui indicater passed
		guiArg			 = argv[1];
		mazeFileName	 = argv[2];
		solutionFileName = argv[3];
		break;

	default:	//Error in the number of arguments passed
		printf("Error: Invalid Number of arguments Passed\n");
		printf("Press enter to exit Program.");

		getchar();	//Wait for Keypress before Exit
		return(-1);
		break;
	}

	//create File pointers and open Files
	FILE *mazeFile = 0;
	FILE *solutionFile = 0;

	fopen_s(&mazeFile, mazeFileName, "r");
	fopen_s(&solutionFile, solutionFileName, "w");

	//Check if the file opened successfully
	if (checkFile(mazeFile) || checkFile(solutionFile))
	{	//Print errors and exit programe
		printf("Error: One of the files failed to load!\n");	//display Error message
		printf("Maze:	  %p \n", mazeFile);						//print file pointers to indecate which is Null
		printf("Solution: %p \n", solutionFile);

		getchar();	//Wait for Keypress before Exit
		return(-1);
	}
	else
	{
		printf("Files Loaded\n");
	}

	//Create an instance of the MAZEDATA struct to store all data related to the maze
	MAZEDATA mazeData;

	//read in maze size, start and end location, store XY values in Coordinate Struct
	//COORD sizeMaze, startMaze, endMaze;
	int foundValues;	//Store the number of successful values read
	foundValues = fscanf_s(mazeFile, "%d %d %d %d %d %d ",
		&mazeData.sizeMaze.X, &mazeData.sizeMaze.Y,
		&mazeData.startMaze.X, &mazeData.startMaze.Y,
		&mazeData.endMaze.X, &mazeData.endMaze.Y);
	//Check if all values successfully read in, should have 6 values
	if (foundValues != 6)
	{
		printf("Error reading in Size, starting location and End location");
		getchar();	//Wait for Keypress before Exit
		return (-1);
	}

	//Create Maze Array, Using Malloc for Dynamic Array sizing
	mazeData.maze = create2DArray(mazeData.sizeMaze);//create 2D array	

													 //Call GetMazeData to read in the Maze values.
	error = getMazeData(mazeFile, &mazeData);
	if (error)
	{
		printf("Error loading maze data from file.");
		getchar();	//Wait for Keypress before Exit
		return (-1);
	}

	//Sets the initial Step number to Zero
	mazeData.stepNumber = 0;

	//Set initial position to start location
	mazeData.position = mazeData.startMaze;

	//Find a Path through the maze (returns true is path found)
	char pathFound = pathFinder(&mazeData);

	//If a path was found, convert the steps into directions (U,D,L,R)
	//and write the direction (append) to the solution file
	if (pathFound)
	{	//convert and write to file
		error = writeSolutionFile(&mazeData, solutionFile);
		if (error)
		{
			printf("Failed to write to Solution File.");
			getchar();	//Wait for Keypress before Exit
			return(-1);
		}
	}
	else
	{
		printf("No Path found, Maze not solved!");
	}

	//Debuging Options
	char debug = True;
	char printMaze = False;

	if (debug)
	{	//Debugging Print 

		//Print maze params
		printf("\nSize:	%d-%d\nStart:	%d-%d\nEnd:	%d-%d\n",
			mazeData.sizeMaze.X, mazeData.sizeMaze.Y,
			mazeData.startMaze.X, mazeData.startMaze.Y,
			mazeData.endMaze.X, mazeData.endMaze.Y);

		if (printMaze)
		{	//print maze data
			int i, j;
			printf("\n");
			for (j = 0; j < mazeData.sizeMaze.Y; j++)
			{
				for (i = 0; i < mazeData.sizeMaze.X; i++)
				{
					printf(" %d", mazeData.maze[i][j]);
				}
				printf("\n");
			}
		}

		//print solution values
		printf("\nMaze Route Found: %d", pathFound);
		printf("\nNumber of Steps: %d", mazeData.stepNumber);
	}

	while (True);
	//Close the file
	free(mazeData.maze);
	fclose(mazeFile);

	return(0);
}

char checkFile(FILE* filePointer)
{	//Check if the file opened successfully
	if ((filePointer == NULL))
	{
		return(True);
	}
	else
	{
		return(False);
	}
}

char getMazeData(FILE* mazeFile, MAZEDATA* mData)
{	//Gets the Maze values out of the Text File

	//Incrementors
	int i, j;
	char readSuccess;	//number of values read in by fscanf
	int valueCount = 0;	//Total number of values read in
	int mazeValue;		//Value of maze at position (i,j)

						//Load in 
	for (j = 0; j < mData->sizeMaze.Y; j++)
	{
		for (i = 0; i < mData->sizeMaze.X; i++)
		{
			readSuccess = fscanf_s(mazeFile, "%d", &mazeValue);	//Pointer increments on each function call
			valueCount += readSuccess;							//Add number read in to the total

																//maze value cannot be larger than 15,therefore check and return error if larger.
			if (mazeValue <= 15)
			{	//Maze value within range, add to array
				mData->maze[i][j] = mazeValue;						//Stores 
			}
			else
			{	//Maze value out of range, throw Error
				return(True);
			}
		}
	}
	if (valueCount != mData->sizeMaze.X*mData->sizeMaze.Y)
	{	//if Value Count doesn't match the number in the maze, give error 
		return(True);
	}

	return(False);
}

int** create2DArray(COORD size)
{	//Creates a double pointer and Creates a 2D array of given size.
	//create the Number of Columns, with data size int
	int** newArray;
	newArray = malloc(size.X * sizeof(int));

	//Loop through the Number of Columns Creating the Number of Rows
	int i;
	for (i = 0; i < size.X; i++)
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
		walls.U = False;
		walls.L = False;
		walls.D = False;
		walls.R = False;
		return(walls);
	case 1:
		walls.U = False;
		walls.L = False;
		walls.D = False;
		walls.R = True;
		return(walls);
	case 2:
		walls.U = False;
		walls.L = False;
		walls.D = True;
		walls.R = False;
		return(walls);
	case 3:
		walls.U = False;
		walls.L = False;
		walls.D = True;
		walls.R = True;
		return(walls);
	case 4:
		walls.U = False;
		walls.L = True;
		walls.D = False;
		walls.R = False;
		return(walls);
	case 5:
		walls.U = False;
		walls.L = True;
		walls.D = False;
		walls.R = True;
		return(walls);
	case 6:
		walls.U = False;
		walls.L = True;
		walls.D = True;
		walls.R = False;
		return(walls);
	case 7:
		walls.U = False;
		walls.L = True;
		walls.D = True;
		walls.R = True;
		return(walls);
	case 8:
		walls.U = True;
		walls.L = False;
		walls.D = False;
		walls.R = False;
		return(walls);
	case 9:
		walls.U = True;
		walls.L = False;
		walls.D = False;
		walls.R = True;
		return(walls);
	case 10:
		walls.U = True;
		walls.L = False;
		walls.D = True;
		walls.R = False;
		return(walls);
	case 11:
		walls.U = True;
		walls.L = False;
		walls.D = True;
		walls.R = True;
		return(walls);
	case 12:
		walls.U = True;
		walls.L = True;
		walls.D = False;
		walls.R = False;
		return(walls);
	case 13:
		walls.U = True;
		walls.L = True;
		walls.D = False;
		walls.R = True;
		return(walls);
	case 14:
		walls.U = True;
		walls.L = True;
		walls.D = True;
		walls.R = False;
		return(walls);
	default:	//all walls (value 15) should never enter into this position
		walls.U = True;
		walls.L = True;
		walls.D = True;
		walls.R = True;
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

char moveDownMaze(MAZEDATA* mData)
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
	char pathFound = pathFinder(mData);
	return(pathFound);
}

char moveRightMaze(MAZEDATA* mData)
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
	char pathFound = pathFinder(mData);
	return(pathFound);
}

char moveLeftMaze(MAZEDATA* mData)
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
		for (i = 0; i < mData->stepNumber - 1; i++)
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

			char deltaX;
			char deltaY;
			//Calculate difference to get previous move
			if (mData->stepNumber == 0)
			{
				deltaX = 0;
				deltaY = 0;
			}
			else
			{
				deltaX = mData->position.X - mData->pathArray[mData->stepNumber - 1].X;	//X direction last taken, R=1, L=-1
				deltaY = mData->position.Y - mData->pathArray[mData->stepNumber - 1].Y;	//Y direction last taken, D=1, U=-1
			}

			//debug - turn on/off direction check
			//deltaX = 0; deltaY = 0;

			//Check if direction free & path not found & hasnt come from that direction
			if (walls.U == 0 && !pathFound && deltaY !=1)
			{	//Can move in Up direction
				pathFound = moveUpMaze(mData);
			}

			if (walls.D == 0 && !pathFound && deltaY != -1)
			{	//Can move in Down direction
				pathFound = moveDownMaze(mData);
			}

			if (walls.L == 0 && !pathFound && deltaX != 1)
			{	//Can move in Left direction
				pathFound = moveLeftMaze(mData);
			}

			if (walls.R == 0 && !pathFound && deltaX != -1)
			{	//Can move in Right direction
				pathFound = moveRightMaze(mData);
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

char writeSolutionFile(MAZEDATA* mazeData, FILE* solutionFile)
{	//Loop through all steps up to step number
	//Compare current location with next location

	char Error = False;
	//write number of moves required to solve maze to solutionFile
	fprintf_s(solutionFile, "%d", mazeData->stepNumber);
	fprintf_s(solutionFile, "\n");			//add new line

	int i;
	for (i = 0; i < mazeData->stepNumber; i++)
	{
		char xMove = mazeData->pathArray[i + 1].X - mazeData->pathArray[i].X;
		char yMove = mazeData->pathArray[i + 1].Y - mazeData->pathArray[i].Y;

		if (xMove)
		{	//if xMove is non-zero; Move was in x direction
			switch (xMove)	//swtich case for finding direction on selected axis
			{
			case 1:		//Case: moved Right (R)
				fprintf_s(solutionFile, "R ");
				break;

			case -1:	//Case: moved Left (L)
				fprintf_s(solutionFile, "L ");
				break;

			default:	//Case: Error invalid move
				Error = True;
				break;
			}
		}
		else
		{	//if yMove is non-zero; Move was in y direction
			switch (yMove)	//swtich case for finding direction on selected axis
			{
			case 1:		//Case: moved Down (D)
				fprintf_s(solutionFile, "D ");
				break;

			case -1:	//Case: moved Up (U)
				fprintf_s(solutionFile, "U ");
				break;

			default:	//Case: Error invalid move
				Error = True;
				break;
			}
		}
	}

	return(Error);
}
