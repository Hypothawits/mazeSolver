#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "main.h"

#include <Windows.h>
#include <GL/GL.h>
#include "GL/freeglut.h"

//Global Variables for Rendering the Maze in OpenGL
float tileSize;			//Float for scaling maze render
int** renderMazeData;	//Pointer to maze data 
myCOORD renderMazeSize;	//COORD for maze Size

void renderMaze(float pos_x, float pos_y, WALLS wall, float tileSize, float width)
{//Bottom left is 0,0
	//Draw walls
	if (wall.D)
	{	//Draw Down Wall
		glRectd(pos_x, pos_y, pos_x + tileSize, pos_y + width);
	}
	if (wall.L)
	{	//Draw Left Wall
		glRectd(pos_x, pos_y, pos_x + width, pos_y + tileSize);
	}
	if (wall.R)
	{	//Draw Right Wall
		glRectd(pos_x + tileSize, pos_y, pos_x + tileSize - width, pos_y + tileSize);
	}
	if (wall.U)
	{	//Draw Top Wall
		glRectd(pos_x, pos_y + tileSize, pos_x + tileSize, pos_y + tileSize - width);
	}
	//Draw corner Bits
	if (wall.U && wall.R)
	{	//Draw bottom left bit
		glRectd(pos_x, pos_y, pos_x + width, pos_y + width);
	}
	if (wall.D && wall.R)
	{	//Draw top left bit
		glRectd(pos_x, pos_y + tileSize, pos_x + width, pos_y + tileSize - width);
	}
	if (wall.U && wall.L)
	{	//Draw bottom right bit
		glRectd(pos_x + tileSize, pos_y, pos_x + tileSize - width, pos_y + width);
	}
	if (wall.D && wall.L)
	{	//Draw top right bit
		glRectd(pos_x + tileSize, pos_y + tileSize, pos_x + tileSize - width, pos_y + tileSize - width);
	}
	//Walls
	//glRectd(pos_x, pos_y, pos_x + tileSize, pos_y + width);							//Bottom 
	//glRectd(pos_x, pos_y + tileSize, pos_x + tileSize, pos_y + tileSize - width);		//Top 
	//glRectd(pos_x + tileSize, pos_y, pos_x + tileSize - width, pos_y + tileSize);		//Right
	//glRectd(pos_x, pos_y, pos_x + width, pos_y + tileSize);							//Left
	//Coners
	//glRectd(pos_x + tileSize, pos_y, pos_x + tileSize - width, pos_y + width);						//Bottom Right
	//glRectd(pos_x, pos_y, pos_x + width, pos_y + width);											//Bottom Left
	//glRectd(pos_x + tileSize, pos_y + tileSize, pos_x + tileSize - width, pos_y + tileSize - width);		//Top Right
	//glRectd(pos_x, pos_y + tileSize, pos_x + width, pos_y + tileSize - width);						//Top Left
}

//Main Render Function
void renderScene() 
{	//Example Triangle
	glClear(GL_COLOR_BUFFER_BIT);

	//Variables
	float width = tileSize/5.0f;		//Scaling
	float origin_x = -1.0;			//Origin (bottom left)
	float origin_y = -1.0;			//Origin (bottom left)
	float pos_x;
	float pos_y;
	WALLS wall;

	//Set maze wall colour
	glColor3f(1, 1, 1);

	int i, j;
	for (j = 0; j < renderMazeSize.Y; j++)
	{
		for (i = 0; i < renderMazeSize.X; i++)
		{
			wall = toBinary(renderMazeData[i][j]);

			pos_x = origin_x + tileSize * (i);
			pos_y = origin_y + tileSize * (renderMazeSize.Y-j) - tileSize;		//Invert j (y coordinate) becuase of render system, -tile size is becuase it builds tile upwards, to offset by 1 tile

			renderMaze(pos_x, pos_y, wall, tileSize, width);
		}
	}
	glutSwapBuffers();		
	return;
}

int main(int argc, char *argv[])
{
	printf("Program Started\n");
	//Variables
	char* guiArg = "ab";				//char passed from command line
	char* mazeFileName = " ";		//Hold maze file name
	char* solutionFileName = " ";	//hold solution file name
	char displayMaze = 1;				//bool for displaying the maze (note: true = o, false = 1)

	//create File pointers and open Files
	FILE *mazeFile = 0;
	FILE *solutionFile = 0;

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
		displayMaze = strcmp(guiArg, "-d");	//Test if gui arg = "-d"
		break;

	default:	//Error in the number of arguments passed
		printf("Error: Invalid Number of arguments Passed\n");
		printf("Press enter to exit Program.");

		quitError();
		break;
	}
	
	fopen_s(&mazeFile, mazeFileName, "r");
	fopen_s(&solutionFile, solutionFileName, "w");

	//Check if the file opened successfully
	checkFile(mazeFile, mazeFileName);
	checkFile(solutionFile, solutionFileName);

	//Create an instance of the MAZEDATA struct to store all data related to the maze
	MAZEDATA mazeData;

	//Read in maze Parameters
	getMazeParam(mazeFile, &mazeData);
	printf("Got Maze Parameters...\n");
	
	//Create Maze Array, Using Malloc for Dynamic Array sizing
	create2DArray(&mazeData);	//create 2D array	
	printf("Created maze Array...\n");

	//Call to read in the Maze values.
	getMazeData(mazeFile, &mazeData);
	printf("Got maze Data...\n");
	//Sets the initial Step number to Zero
	mazeData.stepNumber = 0;

	//Set initial position to start location
	mazeData.position = mazeData.startMaze;

	//Find a Path through the maze (returns true is path found)
	printf("Started maze Solver...\n");
	char pathFound = pathFinder(&mazeData);
	printf("Finished maze Solver...\n");

	//If a path was found, convert the steps into directions (U,D,L,R)
	//and write the direction (append) to the solution file
	if (pathFound)
	{	//convert and write to file
		writeSolutionFile(&mazeData, solutionFile);
	}
	else
	{
		printf("No Path found, Maze not solved!");
		quitError();
	}

	//Debuging Options
	char debug			= True;
	char printMaze		= False;
	char openGLDisplay	= False;

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

	
	if (displayMaze == 0) //False //Open GL 
	{
		//Set globals to values, to 'pass' them into render function
		tileSize = 2.0f / mazeData.sizeMaze.X;	//Divide grind into X number of tiles with size a fraction of 1 <- is this english?
		renderMazeData = mazeData.maze;	
		renderMazeSize = mazeData.sizeMaze;

		//Set up OpenGL
		glutInit(&argc, argv);
		//Window position
		glutInitWindowPosition(100, 100);
		//Window
		glutInitWindowSize(500*(mazeData.sizeMaze.X/mazeData.sizeMaze.Y), 500);	//Set grid to same aspect ration as maze
		//Display properties
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
		//Name window
		glutCreateWindow("Maze Name");
		//Point to render Function
		glutDisplayFunc(renderScene);
		//Start Render Loop
		glutMainLoop();
	}

	//Close the files
	fclose(mazeFile);
	fclose(solutionFile);

	//free memory
	free(mazeData.maze);

	while (True);
	return(0);
}

void checkFile(FILE* filePointer, char* fileName1)
{	//Check if the file opened successfully
	if ((filePointer == NULL))
	{	//Error has occured
		printf("Error, Failed to open: %s\n", fileName1);	//display Error message
		quitError();
		return;
	}
	else
	{	//No Error
		printf("Opened File: %s\n", fileName1);
		return;
	}
}

void getMazeParam(FILE* mazeFile, MAZEDATA* mazeData)
{	//read in maze size, start and end location, store XY values in Coordinate Struct
	int foundValues;	//Store the number of successful values read
	foundValues = fscanf_s(mazeFile, "%d %d %d %d %d %d ",
		&mazeData->sizeMaze.Y, &mazeData->sizeMaze.X,
		&mazeData->startMaze.Y, &mazeData->startMaze.X,
		&mazeData->endMaze.Y, &mazeData->endMaze.X);

	//Check if all values successfully read in, should have 6 values
	if (foundValues != 6)
	{
		printf("Error reading in Size, starting location and End location");
		quitError();
	}
}

void getMazeData(FILE* mazeFile, MAZEDATA* mData)
{	//Gets the Maze values out of the Text File

	//Incrementors
	int i, j;
	char readSuccess;	//number of values read in by fscanf
	int valueCount = 0;	//Total number of values read in
	int mazeValue;		//Value of maze at position (i,j)

	//Increment through each value of maze data
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
				mData->mazeVisit[i][j] = False;			//Set to not visitied
			}
			else
			{	//Maze value out of range, throw Error
				printf("Error Loading Maze Data\n");
				printf("Error: Invalid Maze Data in File\n");
				quitError();
			}
		}
	}
	if (valueCount != mData->sizeMaze.X*mData->sizeMaze.Y)
	{	//if Value Count doesn't match the number in the maze, give error 
		printf("Error Loading Maze Data\n");
		printf("Error: Insufficient Maze Data in File\n");
		quitError();
	}

	return;		//return with no error
}

void create2DArray(MAZEDATA* mData)
{	//Creates 2 double pointer arrays, one for the maze data (type int) 
	//one for storing where the maze solver has been in the maze (type char)
	//Creates a double pointer and Creates a 2D array of given size.
	//create the Number of Columns, with data size int
	int** newMazeArray;
	int** newVisitArray;
	newMazeArray	= malloc(mData->sizeMaze.X * sizeof(int));
	newVisitArray	= malloc(mData->sizeMaze.X * sizeof(BOOL));

	//Loop through the Number of Columns Creating the Number of Rows
	int i;
	for (i = 0; i < mData->sizeMaze.X; i++)
	{
		newMazeArray[i]  = malloc(mData->sizeMaze.Y * sizeof(int));
		newVisitArray[i] = malloc(mData->sizeMaze.Y * sizeof(BOOL));
	}

	mData->maze		 = newMazeArray;
	mData->mazeVisit = newVisitArray;

	return;
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
		if (mData->mazeVisit[mData->position.X][mData->position.Y])	//if True then has visited
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

			//Set current position to visited
			mData->mazeVisit[mData->position.X][mData->position.Y] = True;

			char deltaX;
			char deltaY;
			//Calculate difference to get previous move, to avoid going back on self
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

void writeSolutionFile(MAZEDATA* mazeData, FILE* solutionFile)
{	//Loop through all steps up to step number
	//Compare current location with next location

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
				printf("Error Writing Solution\n");
				printf("Invalid Move Taken\n");
				quitError();
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
				printf("Error Writing Solution\n");
				printf("Invalid Move Taken\n");
				quitError();
				break;
			}
		}
	}

	return;	//No Error
}

void quitError()
{
	char x = getchar();
	exit(-1);
}