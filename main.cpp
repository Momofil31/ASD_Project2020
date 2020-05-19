#define DEBUG

#ifndef DEBUG

#include "swrace.h"

#endif

#include <fstream>
#include <iostream>
#include <vector>


using namespace std;

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
};

// byte matrix
unsigned char **starMap;
vector<Coordinates> blackRings;
vector<Coordinates> whiteRings;

const unsigned char CELL_EMPTY = 0b00000000;
const unsigned char CELL_VISITED = 0b10000000;
const unsigned char CELL_BLACK = 0b01000000;
const unsigned char CELL_WHITE = 0b00100000;
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

void computeSolution();

bool dfs(int row, int col, int index, int ringCount, int lastWhiteIndex, int lastBlackIndex, char *buffer);

void printPath(int ringsCount, int pathSize, char *buffer);

void mapToJson();

int main() {
    init();

    preprocessMap();

    mapToJson();

    computeSolution();

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
    ofstream out("output.txt", ofstream::app);
    out << ringsCount << " " << pathSize << " " << START_ROW << " " << START_COL << " ";
    for (int i = 0; i < pathSize; ++i) {
        out << buffer[i];
    }
    out << "#" << endl;
    out.close();
}

#endif //DEBUG


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

unsigned char getAvailableDirection(int row, int col, int bufferIndex, int lastWhiteBufferIndex, int ringCount) {
    bool onBlack = starMap[row][col] & CELL_BLACK;
    bool onWhite = starMap[row][col] & CELL_WHITE;
    unsigned char prevPathMapCell = getPrevPathMapCell(row, col);


    //CHOSE DIRECTION ACCORDING TO WALL AND VISITED
    unsigned char availableDirection = CELL_EMPTY;
    //up
    if (!(starMap[row][col] & WALL_UP) && (!(starMap[row - 1][col] & CELL_VISITED) ||
                                           (ringCount == TOTAL_RINGS && isStartCell(row - 1, col)))) {
        availableDirection |= PATH_OUT_UP;
    }
    //right
    if (!(starMap[row][col] & WALL_RIGHT) && (!(starMap[row][col + 1] & CELL_VISITED) ||
                                              (ringCount == TOTAL_RINGS && isStartCell(row, col + 1)))) {
        availableDirection |= PATH_OUT_RIGHT;
    }
    //down
    if (!(starMap[row][col] & WALL_DOWN) && (!(starMap[row + 1][col] & CELL_VISITED) ||
                                             (ringCount == TOTAL_RINGS && isStartCell(row + 1, col)))) {
        availableDirection |= PATH_OUT_DOWN;
    }
    //left
    if (!(starMap[row][col] & WALL_LEFT) && (!(starMap[row][col - 1] & CELL_VISITED) ||
                                             (ringCount == TOTAL_RINGS && isStartCell(row, col - 1)))) {
        availableDirection |= PATH_OUT_LEFT;
    }

    //CLEAN POSSIBILITIES ACCORDING TO GAME RULES
    //black rules
    if (onBlack) {
        //no out as the prev cell
        availableDirection &= ~(prevPathMapCell & PATH_OUT_ALL);
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

bool dfs(int row, int col, int index, int ringCount, int lastWhiteIndex, int lastBlackIndex, char *buffer) {
    //RETURNED HOME WITH MAX SCORE
    if (isStartCell(row, col) && index > 0) {
        if (ringCount >= TOTAL_RINGS) {
            //THE END GAME
            printPath(ringCount, index, buffer);
            mapToJson();
            totalScore = 999999;
        }
        return true;
    }

    bool onBlack = starMap[row][col] & CELL_BLACK;
    bool onWhite = starMap[row][col] & CELL_WHITE;
    unsigned char prevPathMapCell = getPrevPathMapCell(row, col);

    //RING COUNT CHECK
    if (onBlack || onWhite) {
        ringCount++;
        //new ring, checkpoint
        if (ringCount > totalScore) {
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

    // blind spot, return
    if (availableDirection == 0) {
        return false;
    }

    //update white with this if I am on white
    if (onWhite) {
        if (index == 0 || (prevPathMapCell & pathMap[row][col] & PATH_IN_ALL)) {
            lastWhiteIndex = index;
        }
    }

    //DFS EXPLORATION
    starMap[row][col] |= CELL_VISITED;
    bool isDfsEnd = false;
    //up
    if (availableDirection & PATH_OUT_UP) {
        insertOutPathMap(row, col, PATH_OUT_UP);

        buffer[index] = 'U';
        isDfsEnd = dfs(row - 1, col, index + 1, ringCount, lastWhiteIndex, lastBlackIndex, buffer);

        removeOutPathMap(row, col);
    }
    //right
    if (!isDfsEnd && availableDirection & PATH_OUT_RIGHT) {
        insertOutPathMap(row, col, PATH_OUT_RIGHT);

        buffer[index] = 'R';
        isDfsEnd = dfs(row, col + 1, index + 1, ringCount, lastWhiteIndex, lastBlackIndex, buffer);

        removeOutPathMap(row, col);
    }
    //down
    if (!isDfsEnd && availableDirection & PATH_OUT_DOWN) {
        insertOutPathMap(row, col, PATH_OUT_DOWN);

        buffer[index] = 'D';
        isDfsEnd = dfs(row + 1, col, index + 1, ringCount, lastWhiteIndex, lastBlackIndex, buffer);

        removeOutPathMap(row, col);
    }
    //left
    if (!isDfsEnd && availableDirection & PATH_OUT_LEFT) {
        insertOutPathMap(row, col, PATH_OUT_LEFT);

        buffer[index] = 'L';
        isDfsEnd = dfs(row, col - 1, index + 1, ringCount, lastWhiteIndex, lastBlackIndex, buffer);

        removeOutPathMap(row, col);
    }

    if (!isDfsEnd) {
        //wrong path, going back reset visited
        starMap[row][col] &= CELL_NOT_VISITED;
    }

    return isDfsEnd;

}

void computeSolution() {
    START_ROW = 0;
    START_COL = 0;
    char *buffer = new char[66000];
    // pick startup position
    for (int i = 0; i < W_WHITE; ++i) {
        int rowW = whiteRings[i].row;
        int colW = whiteRings[i].col;
        if (starMap[rowW][colW] & CELL_PREPROCESSED) {
            START_ROW = rowW;
            START_COL = colW;
            break;
        }
    }
#ifdef DEBUG
    printf("starting from cell[%d][%d]\n", START_ROW, START_COL);
#endif
    dfs(START_ROW, START_COL, 0, 0, -10, -10, buffer);
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
    ifstream in("input.txt");
    ofstream out("output.txt");
    out.close();

    in >> N_ROWS >> M_COLS >> B_BLACK >> W_WHITE;
    TOTAL_RINGS = B_BLACK + W_WHITE;

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
    blackRings.resize(B_BLACK);
    for (int k = 0; k < B_BLACK; ++k) {
        int row, col;
        in >> row >> col;
        blackRings[k].col = col;
        blackRings[k].row = row;
        starMap[row][col] |= CELL_BLACK;
    }

    // white rings
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
