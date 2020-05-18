#include <bits/stdc++.h>

using namespace std;

enum type { SQUARE, WHITE, BLACK };

int N, M, B, W;

vector<vector<type>> mappa;

bool check(vector<vector<type>> &mappa, int r, int c, int pathLength, string &path) {
    int x, y = r, c;
    int lastx, lasty = r, c;
    char ch, lastCh, nextCh;

    for (int i = 0; i < pathLength; i++) {
        ch = path[i];
        lastCh = path[i - 1];
        nextCh = path[i + 1];

        switch (c) {
        case 'R':
            lastx, lasty = x, y;
            x, y = x, y + 1;
            break;
        case 'L':
            lastx, lasty = x, y;
            x, y = x, y - 1;
            break;
        case 'U':
            lastx, lasty = x, y;
            x, y = x + 1, y;
            break;
        case 'D':
            lastx, lasty = x, y;
            x, y = x - 1, y;
            break;
        case '#': // sono arrivato alla fine
            break;
        default:
            return false;
        }
        // controllo se la cella nuova è quadrato, bianco o nero
        if (mappa[x][y] == SQUARE) {
            // non faccio nulla
        } else if (mappa[x][y] == BLACK) {
            if (lastCh != ch && nextCh == ch) // OK angolo di novanta gradi
        } else if (mappa[x][y] == WHITE) {
            if (lastCh == ch && nextCh != ch)
        }
    }

    return true;
}

int main(int argc, char *argv[]) {
    int A, L, r, c;
    string path;

    if (argc != 3) {
        cout << "wrong arguments! ./checker <outputToCheck> <inputFile> \n";
    }

    string outputToCheckPath = "output/";
    string inputFilePath = "input/";
    ifstream inO(outputToCheckPath + argv[1]);
    ifstream inI(inputFilePath + argv[2]);

    // READING LAST VALID LINE FROM OUTPUT FILE TO CHECK
    string lineToCheck;
    string lineSoFar;
    while (getline(inO, lineSoFar)) {
        if (lineSoFar[lineSoFar.length() - 1] == '#')
            lineToCheck = lineSoFar;
    }
    cout << lineToCheck << endl;

    // Splitting line by spaces
    istringstream ss(lineToCheck);
    // Read a word
    string word;
    ss >> A >> L >> r >> c >> path;

    // cout << A << L << r << c << path << endl; // to check if splitting worked

    // READING MAP FROM INPUT FILE
    inI >> N >> M >> B >> W;

    mappa.resize(N, vector<type>(M, SQUARE));

    for (int i = 0; i < B; i++) {
        int x, y;
        inI >> x >> y;
        mappa[x][y] = BLACK;
    }

    for (int i = 0; i < W; i++) {
        int x, y;
        inI >> x >> y;
        mappa[x][y] = WHITE;
    }

    for (auto &row : mappa) {
        for (auto &cell : row) {
            cout << cell << " ";
        }
        cout << endl;
    }
    cout << endl;

    check(mappa, r, c, L, path);

    inO.close();
    inI.close();
}