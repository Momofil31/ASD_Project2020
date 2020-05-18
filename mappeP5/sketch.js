// let data;
let dataArray = [];
let header;
let squareSize = 10;

function preload() {
	for (let i = 10; i < 20; i++) {
		dataArray.push(loadStrings("../input/input" + i + ".txt"));
	}
}

class Cell {
	constructor(x, y) {
		this.x = x;
		this.y = y;
	}
}

function setup() {
	dataArray.forEach((data) => {
		header = data[0];
		//console.log(data)
		//console.log(header);
		let tmp = header.split(" ");
		console.log(tmp);

		let x = parseInt(tmp[0]);
		let y = parseInt(tmp[1]);

		console.log(x);
		console.log(y);

		let black = [];
		for (let i = 1; i <= parseInt(tmp[2]); i++) {
			black.push(new Cell(data[i].split(" ")[0], data[i].split(" ")[1]));
		}
		console.log(black);

		let white = [];
		for (let i = parseInt(tmp[2]) + 1; i <= parseInt(tmp[3]) + parseInt(tmp[2]); i++) {
			white.push(new Cell(data[i].split(" ")[0], data[i].split(" ")[1]));
		}
		console.log(white);

		let c = createCanvas(x * squareSize, y * squareSize);

		let count = 0;

		for (i = 0; i < x; i++) {
			for (j = 0; j < y; j++) {
				let currentX = (count % x) * squareSize;
				let currentY = Math.floor(count / x) * squareSize;

				fill(255, 255, 255);
				square(currentX, currentY, squareSize);
				count++;
			}
		}

		black.forEach((cs) => {
			let circleSize = squareSize;
			fill(0, 0, 0);
			circle(cs.y * squareSize + squareSize / 2, cs.x * squareSize + squareSize / 2, circleSize - circleSize / 5);
		});

		white.forEach((cs) => {
			let circleSize = squareSize;
			fill(255, 255, 255);
			circle(cs.y * squareSize + squareSize / 2, cs.x * squareSize + squareSize / 2, circleSize - circleSize / 5);
		});
		saveCanvas(c, "img", "jpg");
	});
}

function draw() {}
