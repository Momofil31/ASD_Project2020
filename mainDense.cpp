#define UPLOAD_TO_JUDGE


#ifdef UPLOAD_TO_JUDGE

#include "swrace.h"

#endif


#ifndef UPLOAD_TO_JUDGE

#define DEBUG //print debug
//#define MULTI_FILE_TEST // test on multiple files

#endif


#include <fstream>
#include <iostream>
#include <queue>
#include <vector>
#include <list>
#include <climits>
#include <algorithm>


using namespace std;

string INPUT_FILENAME = "input.txt";
string OUTPUT_FILENAME = "output.txt";

int MAX_DFS_DEPTH;
int TARGET_THRESHOLD = 30; //TODO
int N_ROWS;
int M_COLS;
int B_BLACK;
int W_WHITE;
int TOTAL_RINGS;
int totalScore;

int START_ROW;
int START_COL;

struct Coordinates {
    int row;
    int col;

    bool operator<(const Coordinates &a) const {
        return (row * row + col * col < a.row * a.row + a.col * a.col);
    }
};

// byte matrix
unsigned char **starMap;
vector<Coordinates> blackRings;
vector<Coordinates> whiteRings;
list<Coordinates> targets;

const unsigned char CELL_EMPTY = 0b00000000;
const unsigned char CELL_VISITED = 0b10000000;
const unsigned char CELL_BLACK = 0b01000000;
const unsigned char CELL_WHITE = 0b00100000;
const unsigned char CELL_WHITE_OR_BLACK = CELL_BLACK | CELL_WHITE;
const unsigned char CELL_PREPROCESSED = 0b00010000;
const unsigned char WALL_UP = 0b00001000;
const unsigned char WALL_RIGHT = 0b00000100;
const unsigned char WALL_DOWN = 0b00000010;
const unsigned char WALL_LEFT = 0b00000001;

const unsigned char WALL_ALL = 0b00001111;
const unsigned char CELL_NOT_VISITED = 0b01111111;

unsigned char **pathMap;
const unsigned char PATH_IN_UP = 0b10000000;
const unsigned char PATH_IN_RIGHT = 0b01000000;
const unsigned char PATH_IN_DOWN = 0b00100000;
const unsigned char PATH_IN_LEFT = 0b00010000;
const unsigned char PATH_OUT_UP = 0b00001000;
const unsigned char PATH_OUT_RIGHT = 0b00000100;
const unsigned char PATH_OUT_DOWN = 0b00000010;
const unsigned char PATH_OUT_LEFT = 0b00000001;

const unsigned char PATH_OUT_ALL = 0b00001111; //only out bits
const unsigned char PATH_NO_IN = 0b00001111; //only out bits
const unsigned char PATH_IN_ALL = 0b11110000; //only in bits
const unsigned char PATH_NO_OUT = 0b11110000; //only in bits

// FUNCTIONS
void init();

void preprocessMap();

bool checkBounds(int row, int col);

void updateAdjacentCells(int row, int col);

bool isIsolatedCell(int row, int col) {
    bool neighbour = false;
    //up
    if (checkBounds(row - 1, col)) {
        neighbour |= (starMap[row - 1][col] & CELL_WHITE_OR_BLACK) != 0;
    }
    //right
    if (checkBounds(row, col + 1)) {
        neighbour |= (starMap[row][col + 1] & CELL_WHITE_OR_BLACK) != 0;
    }
    //down
    if (checkBounds(row + 1, col)) {
        neighbour |= (starMap[row + 1][col] & CELL_WHITE_OR_BLACK) != 0;
    }
    //left
    if (checkBounds(row, col - 1)) {
        neighbour |= (starMap[row][col - 1] & CELL_WHITE_OR_BLACK) != 0;
    }

    return !neighbour;

}

void printPath(int ringsCount, int pathSize, char *buffer);

void mapToJson();

void computeSolutionDense();

void computeSolutionAnticipatedHome();

bool dfsDense(int row, int col, int index, int ringCount, int lastWhiteIndex, int lastBlackIndex, char *buffer);

bool dfsAnticipatedHome(int row, int col, int targetRow, int targetCol, int index, int ringCount, int lastWhiteIndex,
                        int lastBlackIndex,
                        char *buffer, int targetMaxDepth);

int main() {

#ifdef MULTI_FILE_TEST
    //keep this log, no submit with multi file enable
    cout << "MULTI FILE TESTING!!!" << endl;
    for (int i = 0; i < 20; ++i) {
        INPUT_FILENAME = "input/input" + std::to_string(i) + ".txt";
        OUTPUT_FILENAME = "output/output" + std::to_string(i) + ".txt";

        time_t start, end;
        start = clock();
        cout << "Testing file " << i << endl;
#endif

    init();

    preprocessMap();

    mapToJson();

    computeSolutionDense();

#ifdef MULTI_FILE_TEST
    end = clock();
    cout << "\nRunning:\t" << ((double) (end - start)) / CLOCKS_PER_SEC << "sec" << endl << endl;

}
#endif
    cout << "TERMINATO" << endl;
    return 0;
}

/**************************** PREPROCESSING ****************************/
void preprocessBlackRings() {
    for (int i = 0; i < B_BLACK; ++i) {
        int row = blackRings[i].row;
        int col = blackRings[i].col;

        if (!(starMap[row][col] & CELL_PREPROCESSED)) {
            starMap[row][col] |= CELL_PREPROCESSED;

            // Heuristic 1: distance from border <=1
            // up border
            if (row <= 1) {
                //up wall
                starMap[row][col] |= WALL_UP;
                updateAdjacentCells(row, col);

                // down cell straight
                starMap[row + 1][col] |= WALL_LEFT;
                starMap[row + 1][col] |= WALL_RIGHT;
                starMap[row + 1][col] |= CELL_PREPROCESSED;
                updateAdjacentCells(row + 1, col);
            }

            // right border
            if (col >= M_COLS - 2) {
                //right wall
                starMap[row][col] |= WALL_RIGHT;
                updateAdjacentCells(row, col);

                // left cell straight
                starMap[row][col - 1] |= WALL_UP;
                starMap[row][col - 1] |= WALL_DOWN;
                starMap[row][col - 1] |= CELL_PREPROCESSED;
                updateAdjacentCells(row, col - 1);
            }

            // down border
            if (row >= N_ROWS - 2) {
                //down wall
                starMap[row][col] |= WALL_DOWN;
                updateAdjacentCells(row, col);

                // up cell straight
                starMap[row - 1][col] |= WALL_LEFT;
                starMap[row - 1][col] |= WALL_RIGHT;
                starMap[row - 1][col] |= CELL_PREPROCESSED;
                updateAdjacentCells(row - 1, col);
            }

            // left border
            if (col <= 1) {
                //left wall
                starMap[row][col] |= WALL_LEFT;
                updateAdjacentCells(row, col);

                // right cell straight
                starMap[row][col + 1] |= WALL_UP;
                starMap[row][col + 1] |= WALL_DOWN;
                starMap[row][col + 1] |= CELL_PREPROCESSED;
                updateAdjacentCells(row, col + 1);
            }

            // Heuristic 3: two black dots near have straight path on the opposite side
            // check only down and right
            // down
            if (checkBounds(row + 1, col) && starMap[row + 1][col] & CELL_BLACK) {
                //between 2 dots
                starMap[row][col] |= WALL_DOWN;
                updateAdjacentCells(row, col);
                // up straight
                starMap[row - 1][col] |= WALL_LEFT;
                starMap[row - 1][col] |= WALL_RIGHT;
                starMap[row - 1][col] |= CELL_PREPROCESSED;
                updateAdjacentCells(row - 1, col);
                // 2 cell down straight
                starMap[row + 2][col] |= WALL_LEFT;
                starMap[row + 2][col] |= WALL_RIGHT;
                starMap[row + 2][col] |= CELL_PREPROCESSED;
                updateAdjacentCells(row + 2, col);
            }
            // right
            if (checkBounds(row, col + 1) && starMap[row][col + 1] & CELL_BLACK) {
                //between 2 dots
                starMap[row][col] |= WALL_RIGHT;
                updateAdjacentCells(row, col);
                // left straight
                starMap[row][col - 1] |= WALL_UP;
                starMap[row][col - 1] |= WALL_DOWN;
                starMap[row][col - 1] |= CELL_PREPROCESSED;
                updateAdjacentCells(row, col - 1);
                // 2 cell right straight
                starMap[row][col + 2] |= WALL_UP;
                starMap[row][col + 2] |= WALL_DOWN;
                starMap[row][col + 2] |= CELL_PREPROCESSED;
                updateAdjacentCells(row, col + 2);
            }

            // Heuristic 5: two white next to black in an oblique angle
            // up
            if (checkBounds(row - 1, col - 1) && starMap[row - 1][col - 1] & CELL_WHITE &&
                checkBounds(row - 1, col + 1) &&
                starMap[row - 1][col + 1] & CELL_WHITE) {
                // down straight
                starMap[row + 1][col] |= WALL_LEFT;
                starMap[row + 1][col] |= WALL_RIGHT;
                starMap[row + 1][col] |= CELL_PREPROCESSED;
                updateAdjacentCells(row + 1, col);
            }
            // right
            if (checkBounds(row - 1, col + 1) && starMap[row - 1][col + 1] & CELL_WHITE &&
                checkBounds(row + 1, col + 1) &&
                starMap[row + 1][col + 1] & CELL_WHITE) {
                // left straight
                starMap[row][col - 1] |= WALL_UP;
                starMap[row][col - 1] |= WALL_DOWN;
                starMap[row][col - 1] |= CELL_PREPROCESSED;
                updateAdjacentCells(row, col - 1);
            }
            // down
            if (checkBounds(row + 1, col - 1) && starMap[row + 1][col - 1] & CELL_WHITE &&
                checkBounds(row + 1, col + 1) &&
                starMap[row + 1][col + 1] & CELL_WHITE) {
                // up straight
                starMap[row - 1][col] |= WALL_LEFT;
                starMap[row - 1][col] |= WALL_RIGHT;
                starMap[row - 1][col] |= CELL_PREPROCESSED;
                updateAdjacentCells(row - 1, col);
            }
            // left
            if (checkBounds(row - 1, col - 1) && starMap[row - 1][col - 1] & CELL_WHITE &&
                checkBounds(row + 1, col - 1) &&
                starMap[row + 1][col - 1] & CELL_WHITE) {
                // right straight
                starMap[row][col + 1] |= WALL_UP;
                starMap[row][col + 1] |= WALL_DOWN;
                starMap[row][col + 1] |= CELL_PREPROCESSED;
                updateAdjacentCells(row, col + 1);
            }
        }
    }
}

void preprocessWhiteRings() {
    // Heuristic 2
    bool needOneMoreCycle = true;
    while (needOneMoreCycle) {
        needOneMoreCycle = false;
        for (int i = 0; i < W_WHITE; ++i) {
            int row = whiteRings[i].row;
            int col = whiteRings[i].col;
            // only if not preprocessed before
            if (!(starMap[row][col] & CELL_PREPROCESSED)) {
                // has one wall, set the opposite
                // wall up
                if (starMap[row][col] & WALL_UP) {
                    starMap[row][col] |= WALL_DOWN;
                    starMap[row][col] |= CELL_PREPROCESSED;
                    needOneMoreCycle = true;
                }
                // wall right
                if (starMap[row][col] & WALL_RIGHT) {
                    starMap[row][col] |= WALL_LEFT;
                    starMap[row][col] |= CELL_PREPROCESSED;
                    needOneMoreCycle = true;
                }
                // wall down
                if (starMap[row][col] & WALL_DOWN) {
                    starMap[row][col] |= WALL_UP;
                    starMap[row][col] |= CELL_PREPROCESSED;
                    needOneMoreCycle = true;
                }
                // wall left
                if (starMap[row][col] & WALL_LEFT) {
                    starMap[row][col] |= WALL_RIGHT;
                    starMap[row][col] |= CELL_PREPROCESSED;
                    needOneMoreCycle = true;
                }

                // Heuristic 4: if there n>=3 white dots next to each other there are n line straight through them
                // check only 2 right and 2 down,
                // update only current, to the other there is the while to update with walls
                // down
                if (checkBounds(row + 2, col) && starMap[row + 2][col] & CELL_WHITE &&
                    starMap[row + 1][col] & CELL_WHITE) {
                    starMap[row][col] |= WALL_UP;
                    starMap[row][col] |= WALL_DOWN;
                    starMap[row][col] |= CELL_PREPROCESSED;
                    needOneMoreCycle = true;
                }
                // right
                if (checkBounds(row, col + 2) && starMap[row][col + 2] & CELL_WHITE &&
                    starMap[row][col + 1] & CELL_WHITE) {
                    starMap[row][col] |= WALL_RIGHT;
                    starMap[row][col] |= WALL_LEFT;
                    starMap[row][col] |= CELL_PREPROCESSED;
                    needOneMoreCycle = true;
                }

                //HEURISTIC BONUS
                //wall on right -> wall on left, wall up -> wall down
                //if cell next to me is white copy the walls
                if (starMap[row][col] & WALL_RIGHT) {
                    //cell up
                    if (checkBounds(row - 1, col) && starMap[row - 1][col] & CELL_WHITE) {
                        starMap[row - 1][col] |= WALL_RIGHT;
                        starMap[row - 1][col] |= WALL_LEFT;
                        updateAdjacentCells(row - 1, col);

                        if (checkBounds(row - 2, col)) {
                            starMap[row - 2][col] |= WALL_UP;
                            updateAdjacentCells(row - 2, col);
                        }
                    }
                    //cell down
                    if (checkBounds(row + 1, col) && starMap[row + 1][col] & CELL_WHITE) {
                        starMap[row + 1][col] |= WALL_RIGHT;
                        starMap[row + 1][col] |= WALL_LEFT;
                        updateAdjacentCells(row + 1, col);

                        if (checkBounds(row + 2, col)) {
                            starMap[row + 2][col] |= WALL_DOWN;
                            updateAdjacentCells(row + 2, col);
                        }
                    }
                    needOneMoreCycle = true;
                }
                if (starMap[row][col] & WALL_UP) {
                    //cell left
                    if (checkBounds(row, col - 1) && starMap[row][col - 1] & CELL_WHITE) {
                        starMap[row][col - 1] |= WALL_UP;
                        starMap[row][col - 1] |= WALL_DOWN;
                        updateAdjacentCells(row, col - 1);

                        if (checkBounds(row, col - 2)) {
                            starMap[row][col - 2] |= WALL_LEFT;
                            updateAdjacentCells(row, col - 2);
                        }
                    }
                    //cell right
                    if (checkBounds(row, col + 1) && starMap[row][col + 1] & CELL_WHITE) {
                        starMap[row][col + 1] |= WALL_UP;
                        starMap[row][col + 1] |= WALL_DOWN;
                        updateAdjacentCells(row, col + 1);

                        if (checkBounds(row, col + 2)) {
                            starMap[row][col + 2] |= WALL_RIGHT;
                            updateAdjacentCells(row, col + 2);
                        }

                    }
                    needOneMoreCycle = true;
                }


                if (needOneMoreCycle) {
                    updateAdjacentCells(row, col);
                }
            }
        }
    }
}

void preprocessMap() {
    preprocessBlackRings();

    preprocessWhiteRings();
}

/**************************** SOLUTION ****************************/
void insertOutPathMap(int row, int col, unsigned char outMask) {
    pathMap[row][col] |= outMask;
    updateAdjacentCells(row, col);
}

void removeOutPathMap(int row, int col) {
    pathMap[row][col] &= PATH_NO_OUT;
    updateAdjacentCells(row, col);
}

bool isStartCell(int row, int col) {
    return row == START_ROW && col == START_COL;
}

unsigned char getPrevStarMapCell(int row, int col) {
    //arrived from up
    if (checkBounds(row - 1, col) && pathMap[row][col] & PATH_IN_UP) {
        return starMap[row - 1][col];
    }
    //arrived from right
    if (checkBounds(row, col + 1) && pathMap[row][col] & PATH_IN_RIGHT) {
        return starMap[row][col + 1];
    }
    //arrived from down
    if (checkBounds(row + 1, col) && pathMap[row][col] & PATH_IN_DOWN) {
        return starMap[row + 1][col];
    }
    //arrived from left
    if (checkBounds(row, col - 1) && pathMap[row][col] & PATH_IN_LEFT) {
        return starMap[row][col - 1];
    }

#ifdef DEBUG
    cerr << "PREV CELL NOT FOUND" << endl;
#endif
    return CELL_EMPTY;
}

unsigned char getPrevPathMapCell(int row, int col) {
    //arrived from up
    if (checkBounds(row - 1, col) && pathMap[row][col] & PATH_IN_UP) {
        return pathMap[row - 1][col];
    }
    //arrived from right
    if (checkBounds(row, col + 1) && pathMap[row][col] & PATH_IN_RIGHT) {
        return pathMap[row][col + 1];
    }
    //arrived from down
    if (checkBounds(row + 1, col) && pathMap[row][col] & PATH_IN_DOWN) {
        return pathMap[row + 1][col];
    }
    //arrived from left
    if (checkBounds(row, col - 1) && pathMap[row][col] & PATH_IN_LEFT) {
        return pathMap[row][col - 1];
    }

#ifdef DEBUG
    cerr << "PREV CELL NOT FOUND" << endl;
#endif
    return CELL_EMPTY;
}

unsigned char getNextPathMapCell(int row, int col) {
    //exit up
    if (checkBounds(row - 1, col) && pathMap[row][col] & PATH_OUT_UP) {
        return pathMap[row - 1][col];
    }
    //exit right
    if (checkBounds(row, col + 1) && pathMap[row][col] & PATH_OUT_RIGHT) {
        return pathMap[row][col + 1];
    }
    //exit down
    if (checkBounds(row + 1, col) && pathMap[row][col] & PATH_OUT_DOWN) {
        return pathMap[row + 1][col];
    }
    //exit left
    if (checkBounds(row, col - 1) && pathMap[row][col] & PATH_OUT_LEFT) {
        return pathMap[row][col - 1];
    }

#ifdef DEBUG
    cerr << "NEXT CELL NOT FOUND" << endl;
#endif
    return CELL_EMPTY;
}

unsigned char getAvailableDirection(int row, int col, int bufferIndex, int lastWhiteBufferIndex, int ringCount) {
    bool onBlack = starMap[row][col] & CELL_BLACK;
    bool onWhite = starMap[row][col] & CELL_WHITE;
    unsigned char prevPathMapCell = getPrevPathMapCell(row, col);
    unsigned char prevStarMapCell = getPrevStarMapCell(row, col);


    //CHOSE DIRECTION ACCORDING TO WALL AND VISITED
    unsigned char availableDirection = CELL_EMPTY;
    //up
    if (!(starMap[row][col] & WALL_UP) && (!(starMap[row - 1][col] & CELL_VISITED) ||
                                           ((ringCount == TOTAL_RINGS || ringCount >= TARGET_THRESHOLD) &&
                                            isStartCell(row - 1, col)))) {
        availableDirection |= PATH_OUT_UP;
    }
    //right
    if (!(starMap[row][col] & WALL_RIGHT) && (!(starMap[row][col + 1] & CELL_VISITED) ||
                                              ((ringCount == TOTAL_RINGS || ringCount >= TARGET_THRESHOLD) &&
                                               isStartCell(row, col + 1)))) {
        availableDirection |= PATH_OUT_RIGHT;
    }
    //down
    if (!(starMap[row][col] & WALL_DOWN) && (!(starMap[row + 1][col] & CELL_VISITED) ||
                                             ((ringCount == TOTAL_RINGS || ringCount >= TARGET_THRESHOLD) &&
                                              isStartCell(row + 1, col)))) {
        availableDirection |= PATH_OUT_DOWN;
    }
    //left
    if (!(starMap[row][col] & WALL_LEFT) && (!(starMap[row][col - 1] & CELL_VISITED) ||
                                             ((ringCount == TOTAL_RINGS || ringCount >= TARGET_THRESHOLD) &&
                                              isStartCell(row, col - 1)))) {
        availableDirection |= PATH_OUT_LEFT;
    }

    //FIRST CELL IGNORE RULES
    if (bufferIndex == 0) {
        return availableDirection;
    }


    //CLEAN POSSIBILITIES ACCORDING TO GAME RULES
    //black rules
    if (onBlack) {
        //no out as the prev cell
        availableDirection &= ~(prevPathMapCell & PATH_OUT_ALL);
    }
    //Prev cell is black go straight
    if (prevStarMapCell & CELL_BLACK) {
        //only available direction is the same of prev cell
        availableDirection &= (prevPathMapCell & PATH_OUT_ALL);
    }


    //white rules
    if (onWhite && bufferIndex > 0) {
        //on white, no change of direction. Keep the same of prev cell
        availableDirection &= (prevPathMapCell & PATH_OUT_ALL);
    }

    //ALL CELL RULES
    // if the cell behind me is white (with index check) go straight
    if (bufferIndex - lastWhiteBufferIndex == 1) {
        //distance from white is 1 only possibility thisOutPath != prevOutPath
        availableDirection &= ~(prevPathMapCell & PATH_OUT_ALL);
    }

    return availableDirection;
}

int computeDistance(Coordinates a, Coordinates b) { return abs(a.row - b.row) + abs(a.col - b.col); }

void updateTargetMaxDepth(Coordinates thisCell, Coordinates target, int &targetMaxDepth) {
    if (isStartCell(target.row, target.col)) {
        int distanceToTarget = computeDistance(thisCell, target);
        if (distanceToTarget * 2 + 2 < targetMaxDepth)
            targetMaxDepth = distanceToTarget * 2 + 2;
    }
}

bool isCoordCellVisited(Coordinates const &cell) {
    return (starMap[cell.row][cell.col] & CELL_VISITED) != 0; // need this !=0
}

Coordinates getNearestTarget(Coordinates start) {
    Coordinates nearestTarget = {-1, -1};
    int minDistance = INT_MAX;

    //remove visited ring cell
    targets.remove_if(isCoordCellVisited);

    for (Coordinates &target : targets) {
        if (!isCoordCellVisited(target)) {
            int distance = computeDistance(start, target);
            if (distance < minDistance) {
                minDistance = distance;
                nearestTarget = target;
            }
        } else {
            cerr << "Error in remove if" << endl;
        }
    }

    return nearestTarget;
}

void computeSolutionDense() {
    // Parte dal bianco isolato più a sinistra.
    // Se non lo trova prende il bianco più a sinistra.
    START_ROW = 0;
    START_COL = 0;
    char *buffer = new char[66000];

    // pick startup position
    int colMin = INT_MAX;
    sort(whiteRings.begin(), whiteRings.end());

    for (int i = 0; i < W_WHITE; ++i) {
        int rowW = whiteRings[i].row;
        int colW = whiteRings[i].col;
        if (colW < colMin && isIsolatedCell(rowW, colW)) {
            colMin = colW;
            START_ROW = rowW;
            START_COL = colW;
        }
    }
    if (START_ROW == 0 && START_COL == 0 && !(starMap[START_ROW][START_COL] & CELL_WHITE)) {
        for (int i = 0; i < W_WHITE; ++i) {
            int rowW = whiteRings[i].row;
            int colW = whiteRings[i].col;
            if (colW < colMin) {
                colMin = colW;
                START_ROW = rowW;
                START_COL = colW;
            }
        }
    }

    if (START_ROW == -1) {
#ifdef DEBUG
        cerr << "startup cell not found" << endl;
#endif
        START_ROW = 0;
        START_COL = 0;
    }

#ifdef DEBUG
    cout << "START: " << START_ROW << "," << START_COL << endl;
#endif

    dfsDense(START_ROW, START_COL, 0, 0, -10, -10, buffer);
}

bool dfsDense(int row, int col, int index, int ringCount, int lastWhiteIndex, int lastBlackIndex, char *buffer) {
    bool onBlack = starMap[row][col] & CELL_BLACK;
    bool onWhite = starMap[row][col] & CELL_WHITE;
    unsigned char prevPathMapCell = getPrevPathMapCell(row, col);
    unsigned char prevStarMapCell = getPrevStarMapCell(row, col);

    //RETURNED HOME WITH MAX SCORE
    if (isStartCell(row, col) && index > 0 && ringCount >= TOTAL_RINGS) {
        //check home white end rule
        unsigned char nextPathMapCell = getNextPathMapCell(row, col);

        //if on black
        bool validExit = true;
        if (onBlack) {
            // prevIn == currentIN && nextOut == currentOut
            validExit = ((prevPathMapCell & pathMap[row][col] & PATH_IN_ALL) != 0) &&
                        ((nextPathMapCell & pathMap[row][col] & PATH_OUT_ALL) != 0);
        }

        //if on a white
        if (onWhite) {
            //prevIn != currentIn OR nextOut != currentOut
            validExit = ((prevPathMapCell & pathMap[row][col] & PATH_IN_ALL) == 0) ||
                        ((nextPathMapCell & pathMap[row][col] & PATH_OUT_ALL) == 0);
        }

        if (validExit) {
            // USCITA VALIDA TERMINO DFS
            printPath(ringCount, index, buffer);
            mapToJson();
            totalScore = 999999;
            return true;
        }
    }

    //RING COUNT CHECK
    if (onBlack || onWhite) {
        ringCount++;
    }

    //new ring, checkpoint
    if (ringCount > totalScore) {
        bool validExit = false;

        if (onBlack) {
            //open path ends in black
            // prevIn == currentIN && nextOut == currentOut
            validExit = ((prevPathMapCell & pathMap[row][col] & PATH_IN_ALL) != 0);
        } else if (onWhite) {
            //open path ends in white,
            // enterPrev != enterCurrent OR exitNext != exitCurrent
            //        validExit |= onWhite &&
            //                     ((prevPathMapCell & pathMap[row][col] & PATH_IN_ALL) == 0);
            validExit = (prevStarMapCell & CELL_WHITE) == 0;
        } else {
            //not on white or black every move is ok
            validExit = (prevStarMapCell & CELL_WHITE) == 0;;
        }

        if (validExit || !onBlack) {
            totalScore = ringCount;
            printPath(ringCount, index, buffer);
            mapToJson();
        }
    }

    //PRELIMINARY CHECKS
    // 1) I am on black, prev tile has straight path?
    // 1) correct is prevInPath == thisInPath
    if (onBlack) {
        if ((prevPathMapCell & pathMap[row][col] & PATH_IN_ALL) == 0) {
            //prev and current in path not matching
            return false;
        }
    }

    //PATH DECISION
    unsigned char availableDirection = getAvailableDirection(row, col, index, lastWhiteIndex, ringCount);

    // blind spot, return false
    if (availableDirection == 0) {
        return false;
    }

    //update white with this if I am on white and I need to change direction in next cell
    if (onWhite) {
        //TODO SKIP CHECK ON FIRST CELL(?)
        if (index != 0 && (prevPathMapCell & pathMap[row][col] & PATH_IN_ALL)) {
            lastWhiteIndex = index;
        }
    }

    //DFS EXPLORATION
    starMap[row][col] |= CELL_VISITED;

    // CHOSE DIRECTION THAT LEADS TO SHORTEST DISTANCE FROM TARGET
    priority_queue<pair<int, unsigned char>, vector<pair<int, unsigned char>>, greater<pair<int, unsigned char>>> pq;

    //up
    if (availableDirection & PATH_OUT_UP) {
        int dist = computeDistance({row - 1, col}, {N_ROWS - 1, col}); // stessa colonna ma del muro in giù
        if (starMap[row - 1][col] & CELL_WHITE_OR_BLACK)
            dist -= 1; //se c'è un bianco o un nero nella cella adiacente do priorità
        pq.push(make_pair(dist, PATH_OUT_UP));
    }

    //right
    if (availableDirection & PATH_OUT_RIGHT) {
        int dist = computeDistance({row, col + 1}, {row, 0});
        if (starMap[row][col + 1] & CELL_WHITE_OR_BLACK) dist -= 1;
        pq.push(make_pair(dist, PATH_OUT_RIGHT));
    }

    //down
    if (availableDirection & PATH_OUT_DOWN) {
        int dist = computeDistance({row + 1, col}, {0, col});
        if (starMap[row + 1][col] & CELL_WHITE_OR_BLACK) dist -= 1;
        pq.push(make_pair(dist, PATH_OUT_DOWN));
    }

    //left
    if (availableDirection & PATH_OUT_LEFT) {
        int dist = computeDistance({row, col - 1}, {row, M_COLS - 1});
        if (starMap[row][col - 1] & CELL_WHITE_OR_BLACK) dist -= 1;
        pq.push(make_pair(dist, PATH_OUT_LEFT));
    }

    //DFS EXPLORATION
    bool isDfsEnd = false;

    // if i'm on white and on a legal move there's a black ring, go into it
    if (onWhite) {
        if ((availableDirection & PATH_OUT_UP) && (starMap[row - 1][col] & CELL_BLACK)) {
            insertOutPathMap(row, col, PATH_OUT_UP);
            buffer[index] = 'U';
            isDfsEnd = dfsDense(row - 1, col, index + 1, ringCount, lastWhiteIndex, lastBlackIndex,
                                buffer);
        } else if ((availableDirection & PATH_OUT_RIGHT) && (starMap[row][col + 1] & CELL_BLACK)) {
            insertOutPathMap(row, col, PATH_OUT_RIGHT);
            buffer[index] = 'R';
            isDfsEnd = dfsDense(row, col + 1, index + 1, ringCount, lastWhiteIndex, lastBlackIndex,
                                buffer);
        } else if (((availableDirection & PATH_OUT_DOWN) && (starMap[row + 1][col] & CELL_BLACK))) {
            insertOutPathMap(row, col, PATH_OUT_DOWN);
            buffer[index] = 'D';
            isDfsEnd = dfsDense(row + 1, col, index + 1, ringCount, lastWhiteIndex, lastBlackIndex,
                                buffer);

        } else if (((availableDirection & PATH_OUT_LEFT) && (starMap[row][col - 1] & CELL_BLACK))) {
            insertOutPathMap(row, col, PATH_OUT_LEFT);
            buffer[index] = 'L';
            isDfsEnd = dfsDense(row, col - 1, index + 1, ringCount, lastWhiteIndex, lastBlackIndex,
                                buffer);
        }
        removeOutPathMap(row, col);
    }

    // Chiamate ricorsive in ordine UP, RIGHT, LEFT, DOWN
    // Se ho superato la soglia di ritorno a casa, chiamo dfsAnticipatedHome()
    //up
    if (!isDfsEnd && availableDirection & PATH_OUT_UP) {
        insertOutPathMap(row, col, PATH_OUT_UP);
        buffer[index] = 'U';
        if (ringCount >= TARGET_THRESHOLD) {
            isDfsEnd = dfsAnticipatedHome(row - 1, col, START_ROW, START_COL, index + 1, ringCount, lastBlackIndex,
                                          lastBlackIndex,
                                          buffer,
                                          MAX_DFS_DEPTH);
        } else {
            isDfsEnd = dfsDense(row - 1, col, index + 1, ringCount, lastWhiteIndex, lastBlackIndex, buffer);
        }

        removeOutPathMap(row, col);
    }
    //right
    if (!isDfsEnd && availableDirection & PATH_OUT_RIGHT) {
        insertOutPathMap(row, col, PATH_OUT_RIGHT);
        buffer[index] = 'R';
        if (ringCount >= TARGET_THRESHOLD) {
            isDfsEnd = dfsAnticipatedHome(row, col + 1, START_ROW, START_COL, index + 1, ringCount, lastBlackIndex,
                                          lastBlackIndex,
                                          buffer,
                                          MAX_DFS_DEPTH);
        } else {
            isDfsEnd = dfsDense(row, col + 1, index + 1, ringCount, lastWhiteIndex, lastBlackIndex, buffer);
        }
        removeOutPathMap(row, col);
    }
    //down
    if (!isDfsEnd && availableDirection & PATH_OUT_DOWN) {
        insertOutPathMap(row, col, PATH_OUT_DOWN);
        buffer[index] = 'D';
        if (ringCount >= TARGET_THRESHOLD) {
            isDfsEnd = dfsAnticipatedHome(row + 1, col, START_ROW, START_COL, index + 1, ringCount, lastBlackIndex,
                                          lastBlackIndex,
                                          buffer,
                                          MAX_DFS_DEPTH);
        } else {
            isDfsEnd = dfsDense(row + 1, col, index + 1, ringCount, lastWhiteIndex, lastBlackIndex, buffer);
        }
        removeOutPathMap(row, col);
    }
    //left
    if (!isDfsEnd && availableDirection & PATH_OUT_LEFT) {
        insertOutPathMap(row, col, PATH_OUT_LEFT);
        buffer[index] = 'L';
        if (ringCount >= TARGET_THRESHOLD) {
            isDfsEnd = dfsAnticipatedHome(row, col - 1, START_ROW, START_COL, index + 1, ringCount, lastBlackIndex,
                                          lastBlackIndex,
                                          buffer,
                                          MAX_DFS_DEPTH);
        } else {
            isDfsEnd = dfsDense(row, col - 1, index + 1, ringCount, lastWhiteIndex, lastBlackIndex, buffer);
        }
        removeOutPathMap(row, col);
    }

    //Se la dfs non è finita setto la cella attuale come non visitata per poter fare backtrack
    if (!isDfsEnd) {
        //wrong path, going back reset visited
        starMap[row][col] &= CELL_NOT_VISITED;
    }

    return isDfsEnd;
}

void computeSolutionAnticipatedHome() {
    // Parte dal bianco isolato più a sinistra.
    // Se non lo trova prende il bianco più a sinistra.
    START_ROW = 0;
    START_COL = 0;
    char *buffer = new char[66000];

    // pick startup position
    int colMin = INT_MAX;
    sort(whiteRings.begin(), whiteRings.end());

    for (int i = 0; i < W_WHITE; ++i) {
        int rowW = whiteRings[i].row;
        int colW = whiteRings[i].col;
        if (colW < colMin && isIsolatedCell(rowW, colW)) {
            colMin = colW;
            START_ROW = rowW;
            START_COL = colW;
        }
    }
    if (START_ROW == 0 && START_COL == 0 && !(starMap[START_ROW][START_COL] & CELL_WHITE)) {
        for (int i = 0; i < W_WHITE; ++i) {
            int rowW = whiteRings[i].row;
            int colW = whiteRings[i].col;
            if (colW < colMin) {
                colMin = colW;
                START_ROW = rowW;
                START_COL = colW;
            }
        }
    }

    if (START_ROW == -1) {
#ifdef DEBUG
        cerr << "startup cell not found" << endl;
#endif
        START_ROW = 0;
        START_COL = 0;
    }

#ifdef DEBUG
    cout << "START: " << START_ROW << "," << START_COL << endl;
#endif

    // fill targets with black rings
    targets.clear();
//    for (Coordinates &black : blackRings) {
//        targets.push_back(black);
//    }
    for (Coordinates &white : whiteRings) {
        targets.push_back(white);
    }

    dfsAnticipatedHome(START_ROW, START_COL, START_ROW, START_COL, 0, 0, -10, -10, buffer, MAX_DFS_DEPTH);
}

bool dfsAnticipatedHome(int row, int col, int targetRow, int targetCol, int index, int ringCount, int lastWhiteIndex,
                        int lastBlackIndex,
                        char *buffer, int targetMaxDepth) {
    if (targetMaxDepth < 0) {
#ifdef DEBUG
//        cerr << "BREAK FOR TARGET MAX DEPTH" << endl;
//        cout << "BREAK FOR TARGET MAX DEPTH" << endl;
//        mapToJson();
//        //exit(1);
#endif
        return false;
    }

    bool onBlack = starMap[row][col] & CELL_BLACK;
    bool onWhite = starMap[row][col] & CELL_WHITE;
    unsigned char prevPathMapCell = getPrevPathMapCell(row, col);
    unsigned char prevStarMapCell = getPrevStarMapCell(row, col);

    starMap[row][col] |= CELL_VISITED;

    // se il target non è la cella di partenza allora cerco un nuovo target
    if (index == 0 || !isStartCell(targetRow, targetCol)) {
        Coordinates target = getNearestTarget({row, col});
        targetRow = target.row;
        targetCol = target.col;
        targetMaxDepth = MAX_DFS_DEPTH; //resetto la massima profondità di ricorsione, altrimenti se il target è più lontano finisco le chiamate ricorsive prima di arrivarci
    }

    // se non c'è nessun target settato o ho finito i target da visitare o ho raggiunto la soglia, imposto come target casa
    if ((targetRow == -1 && targetCol == -1) || targets.size() == 0 || ringCount >= TARGET_THRESHOLD) {
        // Go back home
        targetRow = START_ROW;
        targetCol = START_COL;
    }

    //RETURNED HOME WITH MAX SCORE or AFTER TRIGGERING THRESHOLD
    if (isStartCell(row, col) && index > 0 && (ringCount >= TOTAL_RINGS || ringCount >= TARGET_THRESHOLD)) {
        //check home white end rule
        unsigned char nextPathMapCell = getNextPathMapCell(row, col);

        //if on black
        bool validExit = true;
        if (onBlack) {
            // prevIn == currentIN && nextOut == currentOut
            validExit = ((prevPathMapCell & pathMap[row][col] & PATH_IN_ALL) != 0) &&
                        ((nextPathMapCell & pathMap[row][col] & PATH_OUT_ALL) != 0);
        }

        //if on a white
        if (onWhite) {
            // prevIn != currentIn OR nextOut != currentOut
            validExit = ((prevPathMapCell & pathMap[row][col] & PATH_IN_ALL) == 0) ||
                        ((nextPathMapCell & pathMap[row][col] & PATH_OUT_ALL) == 0);
        }

        if (validExit) {
            // USCITA VALIDA TERMINO DFS
            printPath(ringCount, index, buffer);
            mapToJson();
            totalScore = 999999;
            //TODO set exit(0) if needed()
            return true;
        }
    }

    //RING COUNT CHECK
    if (onBlack || onWhite) {
        ringCount++;
    }

    //new ring, checkpoint
    if (ringCount > totalScore) {
        bool validExit = false;

        if (onBlack) {
            //open path ends in black
            // prevIn == currentIN
            validExit = ((prevPathMapCell & pathMap[row][col] & PATH_IN_ALL) != 0);
        } else if (onWhite) {
            //open path ends in white,
            // enterPrev != enterCurrent OR exitNext != exitCurrent
            //        validExit |= onWhite &&
            //                     ((prevPathMapCell & pathMap[row][col] & PATH_IN_ALL) == 0);
            validExit = (prevStarMapCell & CELL_WHITE) == 0;
        } else {
            //not on white or black every move is ok

            validExit = (prevStarMapCell & CELL_WHITE) == 0;;
        }

        // TODO onblack removed print (?)
        if (validExit && !onBlack) {
            totalScore = ringCount;
            printPath(ringCount, index, buffer);
            mapToJson();
        }
    }

    //PRELIMINARY CHECKS
    // 1) I am on black, prev tile has straight path?
    // 1) correct is prevInPath == thisInPath
    //TODO SKIP FIRST CELL CHECK(?)
    if (onBlack && index > 1) {
        if ((prevPathMapCell & pathMap[row][col] & PATH_IN_ALL) == 0) {
            //prev and current in path not matching
            starMap[row][col] &= CELL_NOT_VISITED;

            //push the cell in targets again to do backtracking
            targets.push_back({row, col});
            return false;
        }
    }

    // TODO: CHECK CORRECTNESS OF WHITE CELL (?)

    //PATH DECISION
    unsigned char availableDirection = getAvailableDirection(row, col, index, lastWhiteIndex, ringCount);

    // blind spot, return
    if (availableDirection == 0) {
        starMap[row][col] &= CELL_NOT_VISITED;
        if (onWhite || onBlack) {
            //push the cell in targets again
            targets.push_back({row, col});
        }
        return false;
    }

    //update white with this if I am on white and I need to change direction in next cell
    if (onWhite) {
        //TODO SKIP CHECK ON FIRST CELL(?)
        if (index != 0 && (prevPathMapCell & pathMap[row][col] & PATH_IN_ALL)) {
            lastWhiteIndex = index;
        }
    }

    // CHOSE DIRECTION THAT LEADS TO SHORTEST DISTANCE FROM TARGET
    priority_queue<pair<int, unsigned char>, vector<pair<int, unsigned char>>, greater<pair<int, unsigned char>>> pq;

    // Inserting directions into priority queue
    //up
    if (availableDirection & PATH_OUT_UP) {
        int dist = computeDistance({row - 1, col}, {targetRow, targetCol});
        pq.push(make_pair(dist, PATH_OUT_UP));
    }

    //right
    if (availableDirection & PATH_OUT_RIGHT) {
        int dist = computeDistance({row, col + 1}, {targetRow, targetCol});
        pq.push(make_pair(dist, PATH_OUT_RIGHT));
    }

    //down
    if (availableDirection & PATH_OUT_DOWN) {
        int dist = computeDistance({row + 1, col}, {targetRow, targetCol});
        pq.push(make_pair(dist, PATH_OUT_DOWN));
    }

    //left
    if (availableDirection & PATH_OUT_LEFT) {
        int dist = computeDistance({row, col - 1}, {targetRow, targetCol});
        pq.push(make_pair(dist, PATH_OUT_LEFT));
    }


    //DFS EXPLORATION
    bool isDfsEnd = false;

    // TODO CHECK THIS FORMULA
    updateTargetMaxDepth({row, col}, {targetRow, targetCol}, targetMaxDepth);

    // if i'm on white and on a legal move there's a black ring, go into it
    if (onWhite) {
        if ((availableDirection & PATH_OUT_UP) && (starMap[row - 1][col] & CELL_BLACK)) {
            insertOutPathMap(row, col, PATH_OUT_UP);
            buffer[index] = 'U';
            isDfsEnd = dfsAnticipatedHome(row - 1, col, targetRow, targetCol, index + 1, ringCount, lastWhiteIndex,
                                          lastBlackIndex,
                                          buffer, targetMaxDepth - 1);
        } else if ((availableDirection & PATH_OUT_RIGHT) && (starMap[row][col + 1] & CELL_BLACK)) {
            insertOutPathMap(row, col, PATH_OUT_RIGHT);
            buffer[index] = 'R';
            isDfsEnd = dfsAnticipatedHome(row, col + 1, targetRow, targetCol, index + 1, ringCount, lastWhiteIndex,
                                          lastBlackIndex,
                                          buffer, targetMaxDepth - 1);
        } else if (((availableDirection & PATH_OUT_DOWN) && (starMap[row + 1][col] & CELL_BLACK))) {
            insertOutPathMap(row, col, PATH_OUT_DOWN);
            buffer[index] = 'D';
            isDfsEnd = dfsAnticipatedHome(row + 1, col, targetRow, targetCol, index + 1, ringCount, lastWhiteIndex,
                                          lastBlackIndex,
                                          buffer, targetMaxDepth - 1);
        } else if (((availableDirection & PATH_OUT_LEFT) && (starMap[row][col - 1] & CELL_BLACK))) {
            insertOutPathMap(row, col, PATH_OUT_LEFT);
            buffer[index] = 'L';
            isDfsEnd = dfsAnticipatedHome(row, col - 1, targetRow, targetCol, index + 1, ringCount, lastWhiteIndex,
                                          lastBlackIndex,
                                          buffer, targetMaxDepth - 1);
        }
        removeOutPathMap(row, col);
    }

    while (!isDfsEnd && !pq.empty()) {
        //best decision
        unsigned char bestDirection = pq.top().second;
        pq.pop();

        insertOutPathMap(row, col, bestDirection);
        switch (bestDirection) {
            case PATH_OUT_UP:
                buffer[index] = 'U';
                isDfsEnd = dfsAnticipatedHome(row - 1, col, targetRow, targetCol, index + 1, ringCount, lastWhiteIndex,
                                              lastBlackIndex,
                                              buffer, targetMaxDepth - 1);
                break;

            case PATH_OUT_RIGHT:
                buffer[index] = 'R';
                isDfsEnd = dfsAnticipatedHome(row, col + 1, targetRow, targetCol, index + 1, ringCount, lastWhiteIndex,
                                              lastBlackIndex,
                                              buffer, targetMaxDepth - 1);
                break;

            case PATH_OUT_DOWN:
                buffer[index] = 'D';
                isDfsEnd = dfsAnticipatedHome(row + 1, col, targetRow, targetCol, index + 1, ringCount, lastWhiteIndex,
                                              lastBlackIndex,
                                              buffer, targetMaxDepth - 1);
                break;

            case PATH_OUT_LEFT:
                buffer[index] = 'L';
                isDfsEnd = dfsAnticipatedHome(row, col - 1, targetRow, targetCol, index + 1, ringCount, lastWhiteIndex,
                                              lastBlackIndex,
                                              buffer, targetMaxDepth - 1);
                break;

        }
        removeOutPathMap(row, col);
    }

    if (!isDfsEnd) {
        //wrong path, going back reset visited
        starMap[row][col] &= CELL_NOT_VISITED;
        if (onWhite || onBlack) {
            //push the cell in targets again
            targets.push_back({row, col});
        }
    }

    return isDfsEnd;
}


/**************************** MAP UTILS ****************************/
bool checkBounds(int row, int col) { return row >= 0 && row < N_ROWS && col >= 0 && col < M_COLS; }

void updateAdjacentCells(int row, int col) {
    // update virtual walls

    //STAR MAP
    // up
    if (starMap[row][col] & WALL_UP && checkBounds(row - 1, col)) {
        starMap[row - 1][col] |= WALL_DOWN;
    }
    // right
    if (starMap[row][col] & WALL_RIGHT && checkBounds(row, col + 1)) {
        starMap[row][col + 1] |= WALL_LEFT;
    }
    // down
    if (starMap[row][col] & WALL_DOWN && checkBounds(row + 1, col)) {
        starMap[row + 1][col] |= WALL_UP;
    }
    // left
    if (starMap[row][col] & WALL_LEFT && checkBounds(row, col - 1)) {
        starMap[row][col - 1] |= WALL_RIGHT;
    }

    //PATH MAP
    // up
    if (checkBounds(row - 1, col)) {
        if (pathMap[row][col] & PATH_OUT_UP) {
            //add
            pathMap[row - 1][col] |= PATH_IN_DOWN;
        } else {
            //remove
            pathMap[row - 1][col] &= (unsigned char) ~PATH_IN_DOWN;
        }
    }

    // right
    if (checkBounds(row, col + 1)) {
        if (pathMap[row][col] & PATH_OUT_RIGHT) {
            //add
            pathMap[row][col + 1] |= PATH_IN_LEFT;
        } else {
            //remove
            pathMap[row][col + 1] &= (unsigned char) ~PATH_IN_LEFT;
        }
    }

    // down
    if (checkBounds(row + 1, col)) {
        if (pathMap[row][col] & PATH_OUT_DOWN) {
            //add
            pathMap[row + 1][col] |= PATH_IN_UP;
        } else {
            //remove
            pathMap[row + 1][col] &= (unsigned char) ~PATH_IN_UP;
        }
    }

    // left
    if (checkBounds(row, col - 1)) {
        if (pathMap[row][col] & PATH_OUT_LEFT) {
            //add
            pathMap[row][col - 1] |= PATH_IN_RIGHT;
        } else {
            //remove
            pathMap[row][col - 1] &= (unsigned char) ~PATH_IN_RIGHT;
        }
    }

}

/**************************** GENERAL UTILS ****************************/
void init() {
    ifstream in(INPUT_FILENAME);
    ofstream out(OUTPUT_FILENAME);
    out.close();

    in >> N_ROWS >> M_COLS >> B_BLACK >> W_WHITE;
    TOTAL_RINGS = B_BLACK + W_WHITE;
    MAX_DFS_DEPTH = (int) N_ROWS + M_COLS;

    // build starMap
    starMap = new unsigned char *[N_ROWS];
    pathMap = new unsigned char *[N_ROWS];
    for (int i = 0; i < N_ROWS; ++i) {
        starMap[i] = new unsigned char[M_COLS];
        pathMap[i] = new unsigned char[M_COLS];
        for (int j = 0; j < M_COLS; ++j) {
            starMap[i][j] = CELL_EMPTY;
            pathMap[i][j] = CELL_EMPTY;
            // first row
            if (i == 0)
                starMap[i][j] |= WALL_UP;
            // last row
            if (i == N_ROWS - 1)
                starMap[i][j] |= WALL_DOWN;

            // first col
            if (j == 0)
                starMap[i][j] |= WALL_LEFT;
            // last col
            if (j == M_COLS - 1)
                starMap[i][j] |= WALL_RIGHT;
        }
    }

    // black rings
    blackRings.clear();
    blackRings.resize(B_BLACK);
    for (int k = 0; k < B_BLACK; ++k) {
        int row, col;
        in >> row >> col;
        blackRings[k].col = col;
        blackRings[k].row = row;
        starMap[row][col] |= CELL_BLACK;
    }

    // white rings
    whiteRings.clear();
    whiteRings.resize(W_WHITE);
    for (int k = 0; k < W_WHITE; ++k) {
        int row, col;
        in >> row >> col;
        whiteRings[k].col = col;
        whiteRings[k].row = row;
        starMap[row][col] |= CELL_WHITE;
    }

    in.close();
}

/**************************** OUTPUT ****************************/
#ifdef DEBUG

void printPath(int ringsCount, int pathSize, char *buffer) {
    cout << ringsCount << " " << pathSize << " " << START_ROW << " " << START_COL << " ";
    for (int i = 0; i < pathSize; ++i) {
        cout << buffer[i];
    }
    cout << "#" << endl;
}

#endif //DEBUG

#ifndef DEBUG

void printPath(int ringsCount, int pathSize, char *buffer) {
    ofstream out(OUTPUT_FILENAME, ofstream::app);
    out << ringsCount << " " << pathSize << " " << START_ROW << " " << START_COL << " ";
    for (int i = 0; i < pathSize; ++i) {
        out << buffer[i];
    }
    out << "#" << endl;
    out.close();
}

#endif //DEBUG


void mapToJson() {
#ifdef DEBUG
    cout << "{\"cols\":" << M_COLS << ",\"rows\":" << N_ROWS;
    cout << ",\"starMap\":[";
    for (int i = 0; i < N_ROWS; ++i) {
        cout << "[";
        for (int j = 0; j < M_COLS; ++j) {
            cout << (int) starMap[i][j];
            if (j < M_COLS - 1) {
                cout << ",";
            }
        }
        cout << "]";
        if (i < N_ROWS - 1) {
            cout << ",";
        }
    }
    cout << "]";

    cout << ",\"pathMap\":[";
    for (int i = 0; i < N_ROWS; ++i) {
        cout << "[";
        for (int j = 0; j < M_COLS; ++j) {
            cout << (int) pathMap[i][j];
            if (j < M_COLS - 1) {
                cout << ",";
            }
        }
        cout << "]";
        if (i < N_ROWS - 1) {
            cout << ",";
        }
    }
    cout << "]";
    cout << "}" << endl;
#endif //DEBUG
}