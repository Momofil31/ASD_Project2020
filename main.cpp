//#include "swrace.h"
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

// FUNCTIONS
void init();

void preprocessMap();

bool checkBounds(int row, int col);

void updateAdjacentCells(int row, int col);

void computeSolution();

bool dfs(int row, int col, int index, int ringCount, const int *rowStart, const int *colStart, char *buffer,
         int bufferSize);

void printPath(int ringsCount, int pathSize, int startRow, int starCol, char *buffer);

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

            // Heuristic 1: distance from border <=1
            // up border
            if (row <= 1) {
                // down cell straight
                starMap[row + 1][col] |= WALL_LEFT;
                starMap[row + 1][col] |= WALL_RIGHT;
                starMap[row + 1][col] |= CELL_PREPROCESSED;
                updateAdjacentCells(row + 1, col);
            }

            // right border
            if (col >= M_COLS - 2) {
                // left cell straight
                starMap[row][col - 1] |= WALL_UP;
                starMap[row][col - 1] |= WALL_DOWN;
                starMap[row][col - 1] |= CELL_PREPROCESSED;
                updateAdjacentCells(row, col - 1);
            }

            // down border
            if (row >= N_ROWS - 2) {
                // up cell straight
                starMap[row - 1][col] |= WALL_LEFT;
                starMap[row - 1][col] |= WALL_RIGHT;
                starMap[row - 1][col] |= CELL_PREPROCESSED;
                updateAdjacentCells(row - 1, col);
            }

            // left border
            if (col <= 1) {
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
                // up
                starMap[row - 1][col] |= WALL_LEFT;
                starMap[row - 1][col] |= WALL_RIGHT;
                starMap[row - 1][col] |= CELL_PREPROCESSED;
                updateAdjacentCells(row - 1, col);
                // 2 cell down
                starMap[row + 2][col] |= WALL_LEFT;
                starMap[row + 2][col] |= WALL_RIGHT;
                starMap[row + 2][col] |= CELL_PREPROCESSED;
                updateAdjacentCells(row + 2, col);
            }
            // right
            if (checkBounds(row, col + 1) && starMap[row][col + 1] & CELL_BLACK) {
                // left
                starMap[row][col - 1] |= WALL_UP;
                starMap[row][col - 1] |= WALL_DOWN;
                starMap[row][col - 1] |= CELL_PREPROCESSED;
                updateAdjacentCells(row, col - 1);
                // 2 cell right
                starMap[row][col + 2] |= WALL_UP;
                starMap[row][col + 2] |= WALL_DOWN;
                starMap[row][col + 2] |= CELL_PREPROCESSED;
                updateAdjacentCells(row, col + 2);
            }

            // Heuristic 5: two white next to black in an oblique angle
            // up
            if (checkBounds(row - 1, col - 1) && starMap[row - 1][col - 1] & CELL_WHITE && checkBounds(row - 1, col + 1) &&
                starMap[row - 1][col + 1] & CELL_WHITE) {
                // down straight
                starMap[row + 1][col] |= WALL_LEFT;
                starMap[row + 1][col] |= WALL_RIGHT;
                starMap[row + 1][col] |= CELL_PREPROCESSED;
                updateAdjacentCells(row + 1, col);
            }
            // right
            if (checkBounds(row - 1, col + 1) && starMap[row - 1][col + 1] & CELL_WHITE && checkBounds(row + 1, col + 1) &&
                starMap[row + 1][col + 1] & CELL_WHITE) {
                // left straight
                starMap[row][col - 1] |= WALL_UP;
                starMap[row][col - 1] |= WALL_DOWN;
                starMap[row][col - 1] |= CELL_PREPROCESSED;
                updateAdjacentCells(row, col - 1);
            }
            // down
            if (checkBounds(row + 1, col - 1) && starMap[row + 1][col - 1] & CELL_WHITE && checkBounds(row + 1, col + 1) &&
                starMap[row + 1][col + 1] & CELL_WHITE) {
                // up straight
                starMap[row - 1][col] |= WALL_LEFT;
                starMap[row - 1][col] |= WALL_RIGHT;
                starMap[row - 1][col] |= CELL_PREPROCESSED;
                updateAdjacentCells(row - 1, col);
            }
            // left
            if (checkBounds(row - 1, col - 1) && starMap[row - 1][col - 1] & CELL_WHITE && checkBounds(row + 1, col - 1) &&
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
                if (checkBounds(row + 2, col) && starMap[row + 2][col] & CELL_WHITE && starMap[row + 1][col] & CELL_WHITE) {
                    starMap[row][col] |= WALL_UP;
                    starMap[row][col] |= WALL_DOWN;
                    starMap[row][col] |= CELL_PREPROCESSED;
                    needOneMoreCycle = true;
                }
                // right
                if (checkBounds(row, col + 2) && starMap[row][col + 2] & CELL_WHITE && starMap[row][col + 1] & CELL_WHITE) {
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

void printPath(int ringsCount, int pathSize, int startRow, int starCol, char *buffer) {
    cout << ringsCount << " " << pathSize << " " << startRow << " " << starCol << " ";
    for (int i = 0; i < pathSize; ++i) {
        cout << buffer[i];
    }
    cout << "#" << endl;
}

bool dfs(int row, int col, int index, int ringCount, const int *rowStart, const int *colStart, char *buffer,
         int bufferSize) {
    //Back home
    if (row == *rowStart && col == *colStart && index > 0) {
        if (ringCount >= W_WHITE + B_BLACK) {
            //THE END GAME
            printPath(ringCount, index, *rowStart, *colStart, buffer);
            totalScore = 99999;
        }
        return true;
    }

    //exceed buffer
    if (index > bufferSize - 2) {
        return false;
    }

    //check cell is a ring
    if (starMap[row][col] & CELL_BLACK || starMap[row][col] & CELL_WHITE) {
        ringCount++;
        //new ring, checkpoint
        if (ringCount > totalScore) {
            totalScore = ringCount;
            printPath(ringCount, index, *rowStart, *colStart, buffer);
        }
    }


    int numWays = 0;
    unsigned char availableDirection = CELL_EMPTY;
    //up
    if (!(starMap[row][col] & WALL_UP) && (!(starMap[row - 1][col] & CELL_VISITED) ||
                                           (ringCount == TOTAL_RINGS && *rowStart == row - 1 && *colStart == col))) {
        availableDirection |= WALL_UP;
        numWays++;
    }
    //right
    if (!(starMap[row][col] & WALL_RIGHT) && (!(starMap[row][col + 1] & CELL_VISITED) ||
                                              (ringCount == TOTAL_RINGS && *rowStart == row && *colStart == col + 1))) {
        availableDirection |= WALL_RIGHT;
        numWays++;
    }
    //down
    if (!(starMap[row][col] & WALL_DOWN) && (!(starMap[row + 1][col] & CELL_VISITED) ||
                                             (ringCount == TOTAL_RINGS && *rowStart == row + 1 && *colStart == col))) {
        availableDirection |= WALL_DOWN;
        numWays++;
    }
    //left
    if (!(starMap[row][col] & WALL_LEFT) && (!(starMap[row][col - 1] & CELL_VISITED) ||
                                             (ringCount == TOTAL_RINGS && *rowStart == row && *colStart == col - 1))) {
        availableDirection |= WALL_LEFT;
        numWays++;
    }

    // sono in un vicolo cieco
    if (numWays == 0) {
        return false;
        }

    //DFS
        starMap[row][col] |= CELL_VISITED;
    bool isDfsEnd= false;
    //up
    if (availableDirection & WALL_UP) {
        buffer[index] = 'U';
        isDfsEnd = dfs(row - 1, col, index + 1, ringCount, rowStart, colStart, buffer, bufferSize);
        starMap[row][col] &= CELL_NOT_VISITED;
        }
    //right
    if (!isDfsEnd && availableDirection & WALL_RIGHT) {
        buffer[index] = 'R';
        isDfsEnd = dfs(row, col + 1, index + 1, ringCount, rowStart, colStart, buffer, bufferSize);
        starMap[row][col] &= CELL_NOT_VISITED;
        }
    //down
    if (!isDfsEnd && availableDirection & WALL_DOWN) {
        buffer[index] = 'D';
        isDfsEnd = dfs(row + 1, col, index + 1, ringCount, rowStart, colStart, buffer, bufferSize);
        starMap[row][col] &= CELL_NOT_VISITED;
        }
    //left
    if (!isDfsEnd && availableDirection & WALL_LEFT) {
        buffer[index] = 'L';
        isDfsEnd = dfs(row, col - 1, index + 1, ringCount, rowStart, colStart, buffer, bufferSize);
        starMap[row][col] &= CELL_NOT_VISITED;
        }

    return isDfsEnd;

}

void computeSolution() {
    int row = 0, col = 0;
    char *buffer = new char[66000];
    // pick startup position
    for (int i = 0; i < W_WHITE; ++i) {
        int rowW = whiteRings[i].row;
        int colW = whiteRings[i].col;
        if (starMap[rowW][colW] & CELL_PREPROCESSED) {
            row = rowW;
            col = colW;
            break;
        }
    }
    printf("starting from cell[%d][%d]\n", row, col);

    dfs(row, col, 0, 0, &row, &col, buffer, 66000);
}

/**************************** MAP UTILS ****************************/
bool checkBounds(int row, int col) { return row >= 0 && row < N_ROWS && col >= 0 && col < M_COLS; }

void updateAdjacentCells(int row, int col) {
    // update virtual walls

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
}

/**************************** GENERAL UTILS ****************************/
void init() {
    ifstream in("input.txt");

    in >> N_ROWS >> M_COLS >> B_BLACK >> W_WHITE;
    TOTAL_RINGS = B_BLACK + W_WHITE;

    // build starMap
    starMap = new unsigned char *[N_ROWS];
    for (int i = 0; i < N_ROWS; ++i) {
        starMap[i] = new unsigned char[M_COLS];
        for (int j = 0; j < M_COLS; ++j) {
            starMap[i][j] = CELL_EMPTY;
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
    cout << "{\"cols\":" << M_COLS << ",\"rows\":" << N_ROWS << ",\"data\":[";
    for (int i = 0; i < N_ROWS; ++i) {
        cout << "[";
        for (int j = 0; j < M_COLS; ++j) {
            cout << (int)starMap[i][j];
            if (j < M_COLS - 1) {
                cout << ",";
            }
        }
        cout << "]";
        if (i < N_ROWS - 1) {
            cout << ",";
        }
    }
    cout << "]}" << endl;
}
