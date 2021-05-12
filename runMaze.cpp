#include "wall.h"
#include "maze.h"


//The function will get a cell for the next step in the path. we pass a Maze object, start cell, and a reference of the next candidate cell as parameters. True will be returned if one of the cells for next step is availabe, otherwise False will be return.
bool availabeCell(Maze &theMaze, int startCell, int &nextCell)
{
    int delt[4] = {-1, 1, -theMaze.numCols(), theMaze.numCols()};

    for (int i = 0; i < 4; i++) {           //here only find one cell that is available for the next step
        nextCell = startCell + delt[i];
        if (theMaze.canGo(startCell, nextCell) && !theMaze.isMarked(nextCell)) {        // do not change the order, or memory leakage will happen
            return true;
        }
    }
    return false;
}

//The runMaze() function will find a path from cell number fromCell to cell number toCell. we pass a Maze object, a integer array stored the path found, start cell and end cell as parameters, it will return the length of path.
int runMaze(Maze &theMaze, int path[1], int startCell, int endCell)
{
    static int pathIndex{};
    int nextCell{};

    path[pathIndex] = startCell;    //push the startCell to the top of the stack, path is thought as a stack
    theMaze.mark(startCell);

    if (startCell == endCell) {
        int pathLength = pathIndex + 1;
        pathIndex = 0;
        return pathLength;
    }

    if (availabeCell(theMaze, startCell, nextCell)) {
        pathIndex++;
        return runMaze(theMaze, path, nextCell, endCell);
    }

    if (pathIndex != 0) {      //check if the stack is empty
        pathIndex--;           //back tracking
        return runMaze(theMaze, path, path[pathIndex], endCell);
    }

    return 0;
}
