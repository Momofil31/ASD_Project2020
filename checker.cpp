#include <bits/stdc++.h>

using namespace std;
int N, M, B, W;
char grid[257][257];

float calculate_points(string line) {
    stringstream sline;
    sline.str(line);
    int A, L, r, c;
    string movements;
    sline >> A >> L >> r >> c >> movements;
    if (movements.size() - 1 != L) {
        cout << "Expected " << L << " moves, found " << movements.size() - 1 << "; points: ";
        return 0;
    }
    char mov_grid[257][257];
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < M; ++j) {
            mov_grid[i][j] = '.';
        }
    }
    int x = c, y = r;
    int num = 0;
    bool closed = false;
    int direction = 0, old_direction = 0;
    bool direction_changed = false, last_W = false;
    int visited = 0;
    bool second_white = false;
    for (auto chr : movements) {
        if (chr == '#') {
            if (num != L) {
                cout << "Found # character not at the end; points: ";
                return 0;
            }
            break;
        }
        old_direction = direction;
        if (chr == 'U' || chr == 'D') {
            direction = 1;
        } else {
            direction = -1;
        }
        if (last_W) {
            if (old_direction != direction) {
                visited++;
                last_W = false;
                if (num == 2)
                    second_white = false;
            } else if (!second_white) {
                cout << "Direction not changed before or after W; points: ";
                return 0;
            }
        }
        if (old_direction != 0 && grid[y][x] != '.') {
            if (grid[y][x] == 'B') {
                if (old_direction != direction) {
                    visited++;
                } else {
                    cout << "Direction not changed on B; points: ";
                    return 0;
                }
            } else if (grid[y][x] == 'W') {
                if (num == 1) {
                    second_white = true;
                } else if (old_direction == direction) {
                    if (direction_changed) {
                        visited++;
                    } else {
                        last_W = true;
                    }
                } else {
                    cout << "Direction changed on W; points: ";
                    return 0;
                }
            }
        }
        if (old_direction != 0) {
            if (direction != old_direction) {
                direction_changed = true;
            } else {
                direction_changed = false;
            }
        }
        mov_grid[y][x] = chr;
        if (chr == 'U') {
            y--;
        }
        if (chr == 'D') {
            y++;
        }
        if (chr == 'L') {
            x--;
        }
        if (chr == 'R') {
            x++;
        }
        if (!(x < M && x >= 0) || !(y < N && y >= 0)) {
            cout << "Out of map:(" << y << ", " << x << "); points: ";
            return 0;
        }
        if (mov_grid[y][x] != '.') {
            if (num == L - 1 && y == r && x == c) {
                closed = true;
            } else {
                cout << "Path intersecting at move: " << num << " in (" << y << ", " << x << "); points: ";
                return 0;
            }
        }
        num++;
    }
    if (last_W) {
        cout << "Direction not changed before or after W; points: ";
        return 0;
    }
    if (!closed) {
        if (grid[r][c] != '.')
            visited++;
        if (grid[y][x] != '.')
            visited++;
        if (second_white) {
            cout << "Direction not changed before or after W; points: ";
            return 0;
        }
    } else {
        if (grid[r][c] == 'B') {
            if (direction == 1 && (mov_grid[r][c] == 'R' || mov_grid[r][c] == 'L')) {
                visited++;
            } else if (direction == -1 && (mov_grid[r][c] == 'U' || mov_grid[r][c] == 'D')) {
                visited++;
            } else {
                cout << "Direction not changed on B; points: ";
                return 0;
            }
        } else if (grid[r][c] == 'W') {
            if (direction_changed) {
                visited++;
            } else if ((movements[0] == 'U' || movements[0] == 'D') && (movements[1] == 'L' || movements[1] == 'R')) {
                visited++;
            } else if ((movements[1] == 'U' || movements[1] == 'D') && (movements[0] == 'L' || movements[0] == 'R')) {
                visited++;
            } else {
                cout << "Direction not changed before or after W; points: ";
                return 0;
            }
        }
        if (second_white) {
            if (direction == 1 && (mov_grid[r][c] == 'R' || mov_grid[r][c] == 'L')) {
                visited++;
            } else if (direction == -1 && (mov_grid[r][c] == 'U' || mov_grid[r][c] == 'D')) {
                visited++;
            } else {
                cout << "Direction not changed before or after W; points: ";
                return 0;
            }
        }
    }

    if (visited != A) {
        cout << "Visited only " << visited << " rings, not " << A << "; points: ";
        return 0;
    }

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            cout << mov_grid[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;

    float points = 100 * (float(A) / float(B + W));
    if (!closed) {
        points *= 1.0 / 2;
    }
    return points;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        cout << "Wrong arguments! <input> <output>" << endl;
        return 1;
    }
    string inputPath = "input/";
    string outputPath = "output/";
    ifstream inf(inputPath + argv[1]);
    ifstream outf(outputPath + argv[2]);

    inf >> N >> M >> B >> W;
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < M; ++j) {
            grid[i][j] = '.';
        }
    }
    for (int i = 0; i < B; ++i) {
        int r, c;
        inf >> r >> c;
        grid[r][c] = 'B';
    }
    for (int i = 0; i < W; ++i) {
        int r, c;
        inf >> r >> c;
        grid[r][c] = 'W';
    }
    /*for (int i = 0; i < N; ++i) {
        for (int j = 0; j < M; ++j) {
            cout << grid[i][j];
        }
        cout << endl;
    }*/
    string line;
    while (getline(outf, line)) {
        if (line[line.size() - 1] == '#')
            cout << calculate_points(line) << "%" << endl;
        else
            cout << "Not terminated in #: 0%" << endl;
    }
}