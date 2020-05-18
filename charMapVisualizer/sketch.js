const CELL_VISITED = 0b10000000;
const CELL_BLACK = 0b01000000;
const CELL_WHITE = 0b00100000;
const CELL_PREPROCESSED = 0b00010000;
const WALL_UP = 0b00001000;
const WALL_RIGHT = 0b00000100;
const WALL_DOWN = 0b00000010;
const WALL_LEFT = 0b00000001;

// let jsonMap = '{"cols":8,"rows":8,"data":[[9,58,8,8,8,8,26,76],[1,8,0,32,0,32,12,53],[1,0,0,0,0,0,0,4],[1,32,0,0,0,32,0,4],[1,0,0,0,0,0,36,53],[53,1,32,0,0,0,32,4],[53,1,0,0,0,2,6,53],[3,2,2,2,2,58,58,70]]}';
//load json
let jsonMap = prompt("Mettimi la mappa in json", "{\"cols\":0,\"rows\":0}");

jsonAsoc = JSON.parse(jsonMap);

let cols = parseInt(jsonAsoc['cols']);
let rows = parseInt(jsonAsoc['rows']);
if (cols > 0 && rows > 0) {


    let mapData = jsonAsoc['data'];

    let mapTable = document.getElementById('charMap');
    for (let i = 0; i < rows; i++) {
        let row = mapTable.insertRow(i);

        for (let j = 0; j < cols; j++) {
            let cell = row.insertCell(j);

            if (mapData[i][j] & WALL_UP) {
                cell.classList.add("WALL_UP");
            }
            if (mapData[i][j] & WALL_RIGHT) {
                cell.classList.add("WALL_RIGHT");
            }
            if (mapData[i][j] & WALL_DOWN) {
                cell.classList.add("WALL_DOWN");
            }
            if (mapData[i][j] & WALL_LEFT) {
                cell.classList.add("WALL_LEFT");
            }
            if (mapData[i][j] & CELL_BLACK) {
                cell.classList.add("CELL_BLACK");
            }
            if (mapData[i][j] & CELL_WHITE) {
                cell.classList.add("CELL_WHITE");
            }

            if (mapData[i][j] & CELL_PREPROCESSED) {
                cell.classList.add("CELL_PREPROCESSED");
            }
            if (mapData[i][j] & CELL_VISITED) {
                cell.classList.add("CELL_VISITED");
            }
        }

    }
}