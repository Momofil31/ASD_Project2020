#include "swrace.h"
#include <iostream>
#include <fstream>
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

//byte matrix
unsigned char **map;
vector<Coordinates> blackRings;
vector<Coordinates> whiteRings;

const unsigned char CELL_EMPTY = 0b00000000;
const unsigned char CELL_VISITED = 0b10000000;
const unsigned char CELL_BLACK = 0b01000000;
const unsigned char CELL_WHITE = 0b00100000;
const unsigned char CELL_UNUSED = 0b00010000;
const unsigned char WALL_UP = 0b00001000;
const unsigned char WALL_RIGHT = 0b00000100;
const unsigned char WALL_DOWN = 0b00000010;
const unsigned char WALL_LEFT = 0b00000001;

void init();

void preprocessMap();

void printMap();

int main() {
    init();

    preprocessMap();

    printMap();

    return 0;
}



void preprocessMap(){

}


void init() {
    ifstream in("input.txt");

    in >> N_ROWS >> M_COLS >> B_BLACK >> W_WHITE;

    //build map
    map = new unsigned char *[N_ROWS];
    for (int i = 0; i < N_ROWS; ++i) {
        map[i] = new unsigned char[M_COLS];
        for (int j = 0; j < M_COLS; ++j) {
            map[i][j] = CELL_EMPTY;
        }
    }

    // black rings
    blackRings.resize(B_BLACK);
    for (int k = 0; k < B_BLACK; ++k) {
        int row, col;
        in >> row >> col;
        blackRings[k].col = col;
        blackRings[k].row = row;
        map[row][col] |= CELL_BLACK;
    }

    // white rings
    whiteRings.resize(W_WHITE);
    for (int k = 0; k < W_WHITE; ++k) {
        int row, col;
        in >> row >> col;
        whiteRings[k].col = col;
        whiteRings[k].row = row;
        map[row][col] |= CELL_WHITE;
    }

    in.close();

}

void printMap() {
    for (int i = 0; i < N_ROWS; ++i) {
        cout << endl;
        for (int j = 0; j < M_COLS; ++j) {
            if (map[i][j] & CELL_BLACK) {
                cout << "B ";
            } else if (map[i][j] & CELL_WHITE) {
                cout << "W ";
            } else {
                cout << "_ ";
            }
        }
    }
}
