
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

int N_ROWS;
int M_COLS;
int B_BLACK;
int W_WHITE;

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

// FUNCTIONS
void init();

void preprocessMap();

bool checkBounds(int row, int col);

void updateAdjacentCells(int row, int col);

void printMap();

int main() {
    init();

    preprocessMap();

    printMap();

    return 0;
}

/**************************** PREPROCESSING ****************************/
void preprocessBorders() {
    for (int i = 0; i < N_ROWS; ++i) {
        // first col, wall left
        if (starMap[i][0] & CELL_WHITE) {
            // paper 4, this cell is crossed straight
            starMap[i][0] |= WALL_RIGHT;
            starMap[i][0] |= CELL_PREPROCESSED;
            updateAdjacentCells(i, 0);
        }
        if (starMap[i][0] & CELL_BLACK) {
            // paper 3, cell on right is straight
            starMap[i][1] |= WALL_UP;
            starMap[i][1] |= WALL_DOWN;
            starMap[i][1] |= CELL_PREPROCESSED;
            updateAdjacentCells(i, 1);
        }

        // last col, wall right
        if (starMap[i][M_COLS - 1] & CELL_WHITE) {
            // paper 4, this cell is crossed straight
            starMap[i][M_COLS - 1] |= WALL_LEFT;
            starMap[i][M_COLS - 1] |= CELL_PREPROCESSED;
            updateAdjacentCells(i, M_COLS - 1);
        }
        if (starMap[i][M_COLS - 1] & CELL_BLACK) {
            // paper 3, cell on left is straight
            starMap[i][M_COLS - 2] |= WALL_UP;
            starMap[i][M_COLS - 2] |= WALL_DOWN;
            starMap[i][M_COLS - 2] |= CELL_PREPROCESSED;
            updateAdjacentCells(i, M_COLS - 2);
        }
    }

    for (int j = 0; j < M_COLS; ++j) {
        // first row, wall up
        if (starMap[0][j] & CELL_WHITE) {
            // paper 4, this cell is crossed straight
            starMap[0][j] |= WALL_DOWN;
            starMap[0][j] |= CELL_PREPROCESSED;
            updateAdjacentCells(0, j);
        }

        if (starMap[0][j] & CELL_BLACK) {
            // paper 3, cell down is straight
            starMap[1][j] |= WALL_LEFT;
            starMap[1][j] |= WALL_RIGHT;
            starMap[1][j] |= CELL_PREPROCESSED;
            updateAdjacentCells(1, j);
        }

        // last row, wall down
        if (starMap[N_ROWS - 1][j] & CELL_WHITE) {
            // paper 4, this cell is crossed straight
            starMap[N_ROWS - 1][j] |= WALL_UP;
            starMap[N_ROWS - 1][j] |= CELL_PREPROCESSED;
            updateAdjacentCells(N_ROWS - 1, j);
        }

        if (starMap[N_ROWS - 1][j] & CELL_BLACK) {
            // paper 3, cell up is straight
            starMap[N_ROWS - 2][j] |= WALL_LEFT;
            starMap[N_ROWS - 2][j] |= WALL_RIGHT;
            starMap[N_ROWS - 2][j] |= CELL_PREPROCESSED;
            updateAdjacentCells(N_ROWS - 2, j);
        }
    }
}

void preprocessBlackRings() {
    // TODO paper 5
    // TODO paper 7
}

void preprocessWhiteRings() {
    // paper 6
    for (int i = 0; i < W_WHITE; ++i) {
        int row = whiteRings[i].row;
        int col = whiteRings[i].col;
        // only if not preprocessed before
        if (!(starMap[row][col] & CELL_PREPROCESSED)) {
            // left and right are white
            if (checkBounds(row, col - 1) && starMap[row][col - 1] & CELL_WHITE && checkBounds(row, col + 1) && starMap[row][col + 1] & CELL_WHITE) {
                // all 3 cell straight up - down
                // left
                starMap[row][col - 1] |= WALL_RIGHT;
                starMap[row][col - 1] |= WALL_LEFT;
                starMap[row][col - 1] |= CELL_PREPROCESSED;
                updateAdjacentCells(row, col - 1);
                // right
                starMap[row][col + 1] |= WALL_RIGHT;
                starMap[row][col + 1] |= WALL_LEFT;
                starMap[row][col + 1] |= CELL_PREPROCESSED;
                updateAdjacentCells(row, col + 1);
                // current
                starMap[row][col] |= WALL_RIGHT;
                starMap[row][col] |= WALL_LEFT;
                starMap[row][col] |= CELL_PREPROCESSED;
                updateAdjacentCells(row, col);

            } else
                // up and down are white
                if (checkBounds(row - 1, col) && starMap[row - 1][col] & CELL_WHITE && checkBounds(row + 1, col) &&
                    starMap[row + 1][col] & CELL_WHITE) {
                // all 3 cell straight left - right

                // up
                starMap[row - 1][col] |= WALL_UP;
                starMap[row - 1][col] |= WALL_DOWN;
                starMap[row - 1][col] |= CELL_PREPROCESSED;
                updateAdjacentCells(row - 1, col);
                // down
                starMap[row + 1][col] |= WALL_UP;
                starMap[row + 1][col] |= WALL_DOWN;
                starMap[row + 1][col] |= CELL_PREPROCESSED;
                updateAdjacentCells(row + 1, col);
                // current
                starMap[row][col] |= WALL_UP;
                starMap[row][col] |= WALL_DOWN;
                starMap[row][col] |= CELL_PREPROCESSED;
                updateAdjacentCells(row, col);
            }
        }
    }
}

void preprocessMap() {
    preprocessBorders();

    preprocessBlackRings();

    preprocessWhiteRings();
}

/**************************** MAP UTILS ****************************/
bool checkBounds(int row, int col) { return row >= 0 && row < N_ROWS && col >= 0 && col < M_COLS; }

void updateAdjacentCells(int row, int col) {
    // update virtual walls

    // up
    if (starMap[row][col] & WALL_UP && checkBounds(row - 1, col)) {
        starMap[row - 1][col] |= WALL_UP;
    }
    // right
    if (starMap[row][col] & WALL_RIGHT && checkBounds(row, col + 1)) {
        starMap[row][col + 1] |= WALL_RIGHT;
    }
    // down
    if (starMap[row][col] & WALL_DOWN && checkBounds(row + 1, col)) {
        starMap[row + 1][col] |= WALL_DOWN;
    }
    // left
    if (starMap[row][col] & WALL_LEFT && checkBounds(row, col - 1)) {
        starMap[row][col - 1] |= WALL_LEFT;
    }
}

/**************************** GENERAL UTILS ****************************/
void init() {
    ifstream in("input/input0.txt");

    in >> N_ROWS >> M_COLS >> B_BLACK >> W_WHITE;

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

void printMap() {
    for (int i = 0; i < N_ROWS; ++i) {
        cout << endl;
        for (int j = 0; j < M_COLS; ++j) {
            if (starMap[i][j] & CELL_BLACK) {
                cout << "B\t";
            } else if (starMap[i][j] & CELL_WHITE) {
                cout << "W\t";
            } else {
                // cout << "_ ";
                cout << (int)starMap[i][j] << "\t";
            }
        }
    }
}
