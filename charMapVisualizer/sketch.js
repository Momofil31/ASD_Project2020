const CELL_VISITED = 0b10000000;
const CELL_BLACK = 0b01000000;
const CELL_WHITE = 0b00100000;
const CELL_PREPROCESSED = 0b00010000;
const WALL_UP = 0b00001000;
const WALL_RIGHT = 0b00000100;
const WALL_DOWN = 0b00000010;
const WALL_LEFT = 0b00000001;

const PATH_IN_UP = 0b10000000;
const PATH_IN_RIGHT = 0b01000000;
const PATH_IN_DOWN = 0b00100000;
const PATH_IN_LEFT = 0b00010000;
const PATH_OUT_UP = 0b00001000;
const PATH_OUT_RIGHT = 0b00000100;
const PATH_OUT_DOWN = 0b00000010;
const PATH_OUT_LEFT = 0b00000001;

let jsonMap = '{"cols":8,"rows":8,"starMap":[[137,186,136,136,136,136,154,204],[129,136,128,160,128,160,140,181],[1,128,128,128,128,128,128,132],[1,160,128,128,128,160,128,132],[1,128,128,128,128,132,181,181],[53,129,160,128,128,128,160,132],[181,1,128,128,128,130,134,181],[131,2,130,130,130,186,186,198]],"pathMap":[[66,65,33,66,33,66,65,33],[132,18,40,130,40,132,18,40],[0,130,40,130,72,33,130,40],[0,130,40,132,18,40,130,40],[0,130,72,33,130,40,130,40],[32,132,18,40,130,40,130,40],[40,0,130,40,130,72,129,40],[72,65,129,72,129,4,20,24]]}';
jsonMap = prompt("Mettimi la mappa in json", jsonMap);

jsonAsoc = JSON.parse(jsonMap);

let cols = parseInt(jsonAsoc['cols']);
let rows = parseInt(jsonAsoc['rows']);
if (cols > 0 && rows > 0) {


    let starMap = jsonAsoc['starMap'];
    let pathMap = jsonAsoc['pathMap'];

    let mapTable = document.getElementById('charMap');
    for (let i = 0; i < rows; i++) {
        let row = mapTable.insertRow(i);

        for (let j = 0; j < cols; j++) {
            let cell = row.insertCell(j);
            cell.title = i + "," + j;
            if (starMap[i][j] & WALL_UP) {
                cell.classList.add("WALL_UP");
            }
            if (starMap[i][j] & WALL_RIGHT) {
                cell.classList.add("WALL_RIGHT");
            }
            if (starMap[i][j] & WALL_DOWN) {
                cell.classList.add("WALL_DOWN");
            }
            if (starMap[i][j] & WALL_LEFT) {
                cell.classList.add("WALL_LEFT");
            }
            if (starMap[i][j] & CELL_BLACK) {
                cell.classList.add("CELL_BLACK");
            }
            if (starMap[i][j] & CELL_WHITE) {
                cell.classList.add("CELL_WHITE");
            }

            if (starMap[i][j] & CELL_PREPROCESSED) {
                cell.classList.add("CELL_PREPROCESSED");
            }
            if (starMap[i][j] & CELL_VISITED) {
                cell.classList.add("CELL_VISITED");
            }


            if (pathMap[i][j]) {
                let inPath = document.createElement("div");
                inPath.classList.add("pathIn");
                if (pathMap[i][j] & PATH_IN_UP) {
                    inPath.classList.add("pathUp");
                }
                if (pathMap[i][j] & PATH_IN_RIGHT) {
                    inPath.classList.add("pathRight");
                }
                if (pathMap[i][j] & PATH_IN_DOWN) {
                    inPath.classList.add("pathDown");
                }
                if (pathMap[i][j] & PATH_IN_LEFT) {
                    inPath.classList.add("pathLeft");
                }
                cell.appendChild(inPath);

                let outPath = document.createElement("div");
                outPath.classList.add("pathOut");

                if (pathMap[i][j] & PATH_OUT_UP) {
                    outPath.classList.add("pathUp");
                }
                if (pathMap[i][j] & PATH_OUT_RIGHT) {
                    outPath.classList.add("pathRight");
                }
                if (pathMap[i][j] & PATH_OUT_DOWN) {
                    outPath.classList.add("pathDown");
                }
                if (pathMap[i][j] & PATH_OUT_LEFT) {
                    outPath.classList.add("pathLeft");
                }
                cell.appendChild(outPath);


            }

        }

    }
}