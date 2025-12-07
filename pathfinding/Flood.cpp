#include "Flood.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <map>
#include <queue> 

using namespace std;

unsigned char maze[N][N] = 
{{14, 13, 12, 11, 10, 9, 8, 7, 7, 8, 9, 10, 11, 12, 13, 14},
 {13, 12, 11, 10,  9, 8, 7, 6, 6, 7, 8,  9, 10, 11, 12, 13},
 {12, 11, 10,  9,  8, 7, 6, 5, 5, 6, 7,  8,  9, 10, 11, 12},   
 {11, 10,  9,  8,  7, 6, 5, 4, 4, 5, 6,  7,  8,  9, 10, 11},   
 {10,  9,  8,  7,  6, 5, 4, 3, 3, 4, 5,  6,  7,  8,  9, 10},   
 {9,   8,  7,  6,  5, 4, 3, 2, 2, 3, 4,  5,  6,  7,  8,  9},   
 {8,   7,  6,  5,  4, 3, 2, 1, 1, 2, 3,  4,  5,  6,  7,  8},   
 {7,   6,  5,  4,  3, 2, 1, 0, 0, 1, 2,  3,  4,  5,  6,  7},   
 {7,   6,  5,  4,  3, 2, 1, 0, 0, 1, 2,  3,  4,  5,  6,  7},   
 {8,   7,  6,  5,  4, 3, 2, 1, 1, 2, 3,  4,  5,  6,  7,  8},   
 {9,   8,  7,  6,  5, 4, 3, 2, 2, 3, 4,  5,  6,  7,  8,  9},   
 {10,  9,  8,  7,  6, 5, 4, 3, 3, 4, 5,  6,  7,  8,  9, 10},   
 {11, 10,  9,  8,  7, 6, 5, 4, 4, 5, 6,  7,  8,  9, 10, 11},   
 {12, 11, 10,  9,  8, 7, 6, 5, 5, 6, 7,  8,  9, 10, 11, 12},   
 {13, 12, 11, 10,  9, 8, 7, 6, 6, 7, 8,  9, 10, 11, 12, 13},  
 {14, 13, 12, 11, 10, 9, 8, 7, 7, 8, 9, 10, 11, 12, 13, 14}};

std::stack<configuration> cellStack;
openCells walls[N][N];
configuration currentCfg;
configuration poppedCfg;
std::stack<configuration> pathTaken;

// NEW: Initialize bias
int searchBias = 0;

void initialize() {
    currentCfg.x = 0;
    currentCfg.y = 0;
    currentCfg.dir = 'N';

#ifdef REAL
    pinMode(memory_button, INPUT_PULLDOWN);
    pinMode(memory_switch, INPUT_PULLDOWN);

    if(digitalRead(memory_switch)) {
        loadMazeFromEEPROM(maze);
        loadWallsFromEEPROM(walls);
        Serial.println("loaded");
        digitalWrite(LED_BUILTIN, LOW); delay(200);
        digitalWrite(LED_BUILTIN, HIGH); delay(200);
        digitalWrite(LED_BUILTIN, LOW); delay(200);
        digitalWrite(LED_BUILTIN, HIGH);
    } else {
#endif

    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++) {
            walls[i][j] = openCells();
        }
    }

    for(int i = 0; i < 16; i++) {
        walls[i][0].openS = false;
        walls[i][15].openN = false;
        walls[0][i].openW = false;
        walls[15][i].openE = false;
	
#ifdef SIM			
        visualizeWalls(i, 0, walls[i][0]);
        visualizeWalls(i, 15, walls[i][15]);
        visualizeWalls(0, i, walls[0][i]);
        visualizeWalls(15, i, walls[15][i]);
#endif
    }

#ifdef REAL
    }
#endif
}

#ifdef REAL
void saveMazeToEEPROM(unsigned char maze[N][N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            EEPROM.write(i * N + j, maze[i][j]);
        }
    }
}
void loadMazeFromEEPROM(unsigned char maze[N][N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            maze[i][j] = EEPROM.read(i * N + j);
        }
    }
}
void saveWallsToEEPROM(openCells walls[N][N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            int index = i * N + j + 256;
            EEPROM.write(index, walls[i][j].openN | (walls[i][j].openS << 1) | (walls[i][j].openE << 2) | (walls[i][j].openW << 3));
        }
    }
}
void loadWallsFromEEPROM(openCells walls[N][N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            int index = i * N + j + 256;
            unsigned char data = EEPROM.read(index);
            walls[i][j].openN = data & 0x01;
            walls[i][j].openS = (data >> 1) & 0x01;
            walls[i][j].openE = (data >> 2) & 0x01;
            walls[i][j].openW = (data >> 3) & 0x01;
        }
    }
}
#endif

openCells checkOpenCells(configuration currentCfg) {
    openCells temp;
    temp.openN = false; temp.openS = false; temp.openE = false; temp.openW = false;

    int x = currentCfg.x;
    int y = currentCfg.y;
    char dir = currentCfg.dir;

    switch(dir) {
        case 'N':
            temp.openS = true;
            if(!API::wallFront()) temp.openN = true;
            if(!API::wallLeft()) temp.openW = true;
            if(!API::wallRight()) temp.openE = true;
            break;
        case 'S':
            temp.openN = true;
            if(!API::wallFront()) temp.openS = true;
            if(!API::wallLeft()) temp.openE = true;
            if(!API::wallRight()) temp.openW = true;
            break;
        case 'E':
            temp.openW = true;
            if(!API::wallFront()) temp.openE = true;
            if(!API::wallLeft()) temp.openN = true;
            if(!API::wallRight()) temp.openS = true;
            break;
        case 'W':
            temp.openE = true;
            if(!API::wallFront()) temp.openW = true;
            if(!API::wallLeft()) temp.openS = true;
            if(!API::wallRight()) temp.openN = true;
            break;
    }

    walls[x][y].openN = temp.openN;
    walls[x][y].openS = temp.openS;
    walls[x][y].openE = temp.openE;
    walls[x][y].openW = temp.openW;

    if(y+1 <= 15 && !temp.openN) walls[x][y+1].openS = temp.openN; 
    if(y-1 >= 0 && !temp.openS) walls[x][y-1].openN = temp.openS; 
    if(x+1 <= 15 && !temp.openE) walls[x+1][y].openW = temp.openE; 
    if(x-1 >= 0 && !temp.openW) walls[x-1][y].openE = temp.openW; 

#ifdef SIM
    visualizeWalls(x, y, walls[x][y]);
#endif
    return temp;
}

void flowElevation() {
    int x = currentCfg.x; 
    int y = currentCfg.y; 
    walls[x][y].visited = true;

    openCells checkOpen = checkOpenCells(currentCfg);
    bool openN = checkOpen.openN;
    bool openS = checkOpen.openS;
    bool openE = checkOpen.openE;
    bool openW = checkOpen.openW;

    int N = 1337, S = 1337, E = 1337, W = 1337;

    if(y+1 <= 15 && openN) N = (unsigned char)maze[x][y+1];
    if(y-1 >= 0 && openS) S = (unsigned char)maze[x][y-1];
    if(x+1 <= 15 && openE) E = (unsigned char)maze[x+1][y];
    if(x-1 >= 0 && openW) W = (unsigned char)maze[x-1][y];

    int arraySort[4] = {N, S, E, W};
    std::sort(arraySort, arraySort + 4);
    int min = arraySort[0];
    
    bool Nvisited = walls[x][y+1].visited;
    bool Svisited = walls[x][y-1].visited;
    bool Evisited = walls[x+1][y].visited;
    bool Wvisited = walls[x-1][y].visited;

    int currentVal = (unsigned char)maze[x][y];

    char facing = currentCfg.dir;
    // Keep momentum if possible
    switch(facing) {
        case 'N': if(N == min && currentVal == min + 1 && openN && !Nvisited) { move('N'); return; } break;
        case 'S': if(S == min && currentVal == min + 1 && openS && !Svisited) { move('S'); return; } break;
        case 'E': if(E == min && currentVal == min + 1 && openE && !Evisited) { move('E'); return; } break;
        case 'W': if(W == min && currentVal == min + 1 && openW && !Wvisited) { move('W'); return; } break;
    }

    // --- DIAGONAL SURFER LOGIC ---
    char dirs[4];
    
    if (searchBias == 0) {
        // Run 1: Standard Search (Prioritize North)
        dirs[0] = 'N'; dirs[1] = 'E'; dirs[2] = 'S'; dirs[3] = 'W';
    } 
    else {
        // Run 2: Diagonal Surfer
        // If we are below the diagonal (x > y), move North to catch up
        // If we are above the diagonal (y > x), move East to catch up
        // This creates a zig-zag that discovers the center line!
        if (x > y) {
            dirs[0] = 'N'; dirs[1] = 'E'; dirs[2] = 'W'; dirs[3] = 'S';
        } else {
            dirs[0] = 'E'; dirs[1] = 'N'; dirs[2] = 'S'; dirs[3] = 'W';
        }
    }

    // 1. Check Unvisited Neighbors (Exploration)
    for (int i = 0; i < 4; i++) {
        char d = dirs[i];
        if (d == 'N' && N == min && currentVal == min + 1 && openN && !Nvisited) { move('N'); return; }
        if (d == 'S' && S == min && currentVal == min + 1 && openS && !Svisited) { move('S'); return; }
        if (d == 'E' && E == min && currentVal == min + 1 && openE && !Evisited) { move('E'); return; }
        if (d == 'W' && W == min && currentVal == min + 1 && openW && !Wvisited) { move('W'); return; }
    }

    // 2. Check Visited Neighbors (Backtracking/Pathing)
    for (int i = 0; i < 4; i++) {
        char d = dirs[i];
        if (d == 'N' && N == min && currentVal == min + 1 && openN) { move('N'); return; }
        if (d == 'S' && S == min && currentVal == min + 1 && openS) { move('S'); return; }
        if (d == 'E' && E == min && currentVal == min + 1 && openE) { move('E'); return; }
        if (d == 'W' && W == min && currentVal == min + 1 && openW) { move('W'); return; }
    }
}

void checkNeigboringOpen(configuration poppedCfg) {
    int x = poppedCfg.x;
    int y = poppedCfg.y;
    
    bool openN = walls[x][y].openN;
    bool openS = walls[x][y].openS;
    bool openE = walls[x][y].openE;
    bool openW = walls[x][y].openW;

    int N = 1337, S = 1337, E = 1337, W = 1337;

    if(y+1 <= 15 && openN) N = (unsigned char)maze[x][y+1];
    if(y-1 >= 0 && openS) S = (unsigned char)maze[x][y-1];
    if(x+1 <= 15 && openE) E = (unsigned char)maze[x+1][y];
    if(x-1 >= 0 && openW) W = (unsigned char)maze[x-1][y];

    int arraySort[4] = {N, S, E, W};
    std::sort(arraySort, arraySort + 4);
    int min = arraySort[0];

    if(min != (unsigned char)maze[x][y] - 1) {
        maze[x][y] = min + 1;
        configuration pushCfg = poppedCfg;

        if(x+1 <= 15 && !((x+1==7 && y==7) || (x+1==7 && y==8) || (x+1==8 && y==7) || (x+1==8 && y==8))) {
            pushCfg.x += 1; cellStack.push(pushCfg); pushCfg.x -= 1;
        }
        if(x-1 >= 0 && !((x-1==7 && y==7) || (x-1==7 && y==8) || (x-1==8 && y==7) || (x-1==8 && y==8))) {
            pushCfg.x -= 1; cellStack.push(pushCfg); pushCfg.x += 1;
        }
        if(y+1 <= 15 && !((x==7 && y+1==7) || (x==7 && y+1==8) || (x==8 && y+1==7) || (x==8 && y+1==8))) {
            pushCfg.y += 1; cellStack.push(pushCfg); pushCfg.y -= 1;
        }
        if(y-1 >= 0 && !((x==7 && y-1==7) || (x==7 && y-1==8) || (x==8 && y-1==7) || (x==8 && y-1==8))) {
            pushCfg.y -= 1; cellStack.push(pushCfg); pushCfg.y += 1;
        }
    }
    return;
}

void move(char direction) {
    char facing = currentCfg.dir;
    if(facing == direction) {
        API::moveForward();
    } else {
        if(facing == 'N') {
            switch(direction) {
                case 'S': API::turnLeft(); API::turnLeft(); API::moveForward(); break;
                case 'W': API::turnLeft(); API::moveForward(); break;
                case 'E': API::turnRight(); API::moveForward(); break;
            }
        }
        if(facing == 'S') {
            switch(direction) {
                case 'N': API::turnLeft(); API::turnLeft(); API::moveForward(); break;
                case 'E': API::turnLeft(); API::moveForward(); break;
                case 'W': API::turnRight(); API::moveForward(); break;
            }
        }
        if(facing == 'E') {
            switch(direction) {
                case 'W': API::turnLeft(); API::turnLeft(); API::moveForward(); break;
                case 'N': API::turnLeft(); API::moveForward(); break;
                case 'S': API::turnRight(); API::moveForward(); break;
            }
        }
        if(facing == 'W') {
            switch(direction) {
                case 'E': API::turnLeft(); API::turnLeft(); API::moveForward(); break;
                case 'S': API::turnLeft(); API::moveForward(); break;
                case 'N': API::turnRight(); API::moveForward(); break;
            }
        }
    }
    currentCfg.dir = direction;
    switch(direction) {
        case 'N': currentCfg.y++; break;
        case 'S': currentCfg.y--; break;
        case 'E': currentCfg.x++; break;
        case 'W': currentCfg.x--; break;
    }
}

#ifdef SIM
void mazePrintout() {
	std::cerr << std::endl;
	for(int j = 15; j >= 0; j--) {
		for(int i = 0; i < 16; i++) {
			if(currentCfg.x == i && currentCfg.y == j) {
				if(maze[i][j] < 10) std::cerr << "[" << static_cast<int>(maze[i][j]) << "], ";
				else std::cerr << "[" << static_cast<int>(maze[i][j]) << "], ";
			} else {
				if(maze[i][j] < 10) std::cerr << " " << static_cast<int>(maze[i][j]) << ", ";
				else std::cerr << static_cast<int>(maze[i][j]) << ", ";
			}
		}
		std::cerr << std::endl;
	}
	std::cerr << std::endl;        
}

void visualizeMaze(unsigned char maze[N][N]) {
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++) {
			API::setText(i, j, to_string(static_cast<int>(maze[i][j])));
		}
	}
}

void visualizeWalls(int i, int j, openCells cell) {
	if (!walls[i][j].openN) API::setWall(i, j, 'n');
	if (!walls[i][j].openS) API::setWall(i, j, 's');
	if (!walls[i][j].openE) API::setWall(i, j, 'e');
	if (!walls[i][j].openW) API::setWall(i, j, 'w');
}
#endif

#ifdef REAL
void mazePrintout() {
	for(int j = 4; j >= 0; j--) {
		for(int i = 0; i < 5; i++) {
			if(currentCfg.x == i && currentCfg.y == j) {
				Serial.print("["); Serial.print(maze[i][j]); Serial.print("], ");
			} else {
				Serial.print(" "); Serial.print(maze[i][j]); Serial.print(", ");
			}
		}
		Serial.println();
	}
	Serial.println();
}
#endif

void reflood(char target) {
    for(int i=0; i<N; i++) for(int j=0; j<N; j++) maze[i][j] = 255;
    
    std::queue<std::pair<int, int>> q;

    if (target == 'c') {
        maze[7][7] = 0; q.push({7,7});
        maze[7][8] = 0; q.push({7,8});
        maze[8][7] = 0; q.push({8,7});
        maze[8][8] = 0; q.push({8,8});
    } else if (target == 's') {
        maze[0][0] = 0; q.push({0,0});
    }

    while(!q.empty()) {
        std::pair<int, int> curr = q.front(); q.pop();
        int x = curr.first;
        int y = curr.second;
        int dist = (unsigned char)maze[x][y];

        if (y < 15 && walls[x][y].openN && (unsigned char)maze[x][y+1] == 255) {
            maze[x][y+1] = dist + 1; q.push({x, y+1});
        }
        if (y > 0 && walls[x][y].openS && (unsigned char)maze[x][y-1] == 255) {
            maze[x][y-1] = dist + 1; q.push({x, y-1});
        }
        if (x < 15 && walls[x][y].openE && (unsigned char)maze[x+1][y] == 255) {
            maze[x+1][y] = dist + 1; q.push({x+1, y});
        }
        if (x > 0 && walls[x][y].openW && (unsigned char)maze[x-1][y] == 255) {
            maze[x-1][y] = dist + 1; q.push({x-1, y});
        }
    }
}

void runMaze(char goal) {
    reflood(goal);

	int loopCondition = 1;
	while(loopCondition) {
		pathTaken.push(currentCfg);
#ifdef SIM
		API::setColor(currentCfg.x, currentCfg.y, 'a');
#endif
		flowElevation();
		
		if(goal == 'c') {
			if((currentCfg.x == 7 || currentCfg.x == 8) && (currentCfg.y == 7 || currentCfg.y == 8)) {
				walls[currentCfg.x][currentCfg.y].visited = true;

#ifdef SIM			
				for (int i = 7; i <= 8; i++) {
					for (int j = 7; j <= 8; j++) {
						visualizeWalls(i, j, walls[i][j]);
					}
				}
#endif
				loopCondition = 0;
			}
		}
        else if (goal == 's') {
            if (currentCfg.x == 0 && currentCfg.y == 0) {
                loopCondition = 0;
            }
        }

		cellStack.push(currentCfg);
		while(!cellStack.empty()) {
			poppedCfg = cellStack.top();
			cellStack.pop();
			checkNeigboringOpen(poppedCfg);
		}
        
#ifdef SIM
        visualizeMaze(maze);
#endif
	}

#ifdef REAL
	while(1) {
		delay(300);
		if(digitalRead(memory_button)) {
			saveMazeToEEPROM(maze);
			saveWallsToEEPROM(walls);
		}
	}
#endif
}

void backTrack() {
	while(!pathTaken.empty()) {
		int x = pathTaken.top().x;
		int y = pathTaken.top().y;
		pathTaken.pop();
		int xDiff = x - currentCfg.x;
		int yDiff = y - currentCfg.y;
		if(yDiff == 1) move('N');
		if(yDiff == -1) move('S');
		if(xDiff == 1) move('E');
		if(xDiff == -1) move('W');
	}
	switch(currentCfg.dir) {
		case 'S': API::turnLeft(); API::turnLeft(); break;
		case 'E': API::turnLeft(); break;
		case 'W': API::turnRight(); break;
	}
	currentCfg.dir = 'N';
}

void speedrun() {
    reflood('c');
 // DEBUG: Print current position
    std::cerr << "Speedrun starting from (" << currentCfg.x << "," << currentCfg.y 
              << ") facing " << currentCfg.dir << std::endl;
    
    // DEBUG: Print some maze values to verify reflood worked
    std::cerr << "Maze[0][0] = " << (int)maze[0][0] << std::endl;
    std::cerr << "Maze[7][7] = " << (int)maze[7][7] << std::endl;
    std::cerr << "Maze[8][8] = " << (int)maze[8][8] << std::endl;

	bool highResMaze[33][33] = {}; 

	for(int i = 0; i < 33; i++) {
		highResMaze[i][0] = true; 
		highResMaze[i][32] = true; 
		highResMaze[0][i] = true; 
		highResMaze[32][i] = true; 
	}

	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++) {
			int highResX = 2*i+1;
			int highResY = 2*j+1;

			openCells cell = walls[i][j];
			if(!cell.openN) {
				highResMaze[highResX][highResY + 1] = true; 
				highResMaze[highResX-1][highResY + 1] = true; highResMaze[highResX+1][highResY + 1] = true; 
			}
			if(!cell.openS) {
				highResMaze[highResX][highResY - 1] = true;
				highResMaze[highResX-1][highResY - 1] = true; highResMaze[highResX+1][highResY - 1] = true;
			}
			if(!cell.openE) {
				highResMaze[highResX + 1][highResY] = true;
				highResMaze[highResX + 1][highResY-1] = true; highResMaze[highResX + 1][highResY+1] = true;
			}
			if(!cell.openW) {
				highResMaze[highResX - 1][highResY] = true;
				highResMaze[highResX - 1][highResY-1] = true; highResMaze[highResX - 1][highResY+1] = true;
			}

			if(!cell.visited) {
				highResMaze[highResX-1][highResY-1] = true; highResMaze[highResX][highResY-1] = true; highResMaze[highResX+1][highResY-1] = true;
				highResMaze[highResX-1][highResY] = true; highResMaze[highResX][highResY] = true; highResMaze[highResX+1][highResY] = true;
				highResMaze[highResX-1][highResY+1] = true; highResMaze[highResX][highResY+1] = true; highResMaze[highResX+1][highResY+1] = true;
			}
		}
	}

	Node highResMazeNode[33][33];

	for (int i = 1; i < 33-1; i++) {
		for (int j = 1; j < 33-1; j++) {
			highResMazeNode[i][j].X = i;
			highResMazeNode[i][j].Y = j;
			highResMazeNode[i][j].hCost = calculateH(i, j);
		}
	}
	
	highResMazeNode[1][1].parentX = 1;
	highResMazeNode[1][1].parentY = 0;

	std::vector<std::pair<int, int>> directions = {
		{ 0,  1}, { 0, -1}, {-1,  0}, { 1,  0}, 
		{-1,  1}, { 1,  1}, {-1, -1}, { 1, -1}  
	};

	std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openSet;
	std::set<std::pair<int, int>> openSetCoords;

	highResMazeNode[1][1].gCost = 0;
	highResMazeNode[1][1].fCost = highResMazeNode[1][1].gCost + highResMazeNode[1][1].hCost; 

	openSet.push(highResMazeNode[1][1]);
	openSetCoords.insert({1, 1});
	
	while(!openSet.empty()) {
		Node current = openSet.top();
		openSet.pop();
		openSetCoords.erase({current.X, current.Y});
		
		if((current.X >= 15 && current.X <= 17) && (current.Y >= 15 && current.Y <= 17)) {
			std::vector<std::pair<int, int>> path;
			while (!(current.X == 1 && current.Y == 1)) {
				path.push_back({current.X, current.Y});
				current = highResMazeNode[current.parentX][current.parentY];
			}
			path.push_back({1, 1});
			path.push_back({1, 0});
			std::reverse(path.begin(), path.end());

			std::vector<std::pair<char, double>> commands;
			std::map<std::pair<int, int>, double> directionAngles = {
				{{ 0,  1}, 0.0}, {{ 0, -1}, 180.0}, {{-1,  0}, 270.0}, {{ 1,  0}, 90.0},
				{{-1,  1}, 315.0}, {{ 1,  1}, 45.0}, {{-1, -1}, 225.0}, {{ 1, -1}, 135.0}
			};

			for (std::size_t i = 2; i < path.size(); ++i) {
				std::pair<int, int> nextNodeDirection = {path[i].first - path[i-1].first, path[i].second - path[i-1].second};
				std::pair<int, int> currNodeDirection = {path[i-1].first - path[i-2].first, path[i-1].second - path[i-2].second};
				
				if (nextNodeDirection.first == currNodeDirection.first && nextNodeDirection.second == currNodeDirection.second) {
					if (!commands.empty() && commands.back().first == 'F') {
						commands.back().second += 1;
					} else {
						commands.push_back({'F', 1.0});
					}
				} else {
					double nextNodeDirectionAngle = directionAngles[nextNodeDirection];
#ifdef SIM
					double currNodeDirectionAngle = directionAngles[currNodeDirection];
					double angleDiff = nextNodeDirectionAngle - currNodeDirectionAngle;
					if (angleDiff > 180.0) angleDiff -= 360.0;
					else if (angleDiff < -180.0) angleDiff += 360.0;
						
					while(abs(angleDiff) > 1) {
						double turnAmount = 45.0;
						if (abs(angleDiff) > 45) turnAmount = 90.0;	
						if(angleDiff > 0) {
							angleDiff -= turnAmount;
							commands.push_back({'R', turnAmount});
						} else if (angleDiff < 0) {
							angleDiff += turnAmount;
							commands.push_back({'L', turnAmount});
						}
					}
#endif
#ifdef REAL
					commands.push_back({'T', nextNodeDirectionAngle});
#endif
					commands.push_back({'F', 1});
				}
			}

			for (const auto& command : commands) {
				switch(command.first) {
					case 'F': API::moveForwardHalf(static_cast<int>(command.second)); break;
					case 'L': if (command.second == 45) API::turnLeft45(); else if (command.second == 90) API::turnLeft(); break;
					case 'R': if (command.second == 45) API::turnRight45(); else if (command.second == 90) API::turnRight(); break;
					case 'T': break;
					default: std::cerr << "error" << std::endl;
				}		
			}
			return;
	    }

        for (const auto& direction : directions) {
            int newX = current.X + direction.first;
            int newY = current.Y + direction.second;

            if(highResMaze[newX][newY]) continue;

            bool isDiagonal = (direction.first != 0 && direction.second != 0);
            float moveCost = isDiagonal ? 1.414f : 1.0f;

            int currentNodeDirectionX = highResMazeNode[current.X][current.Y].X - highResMazeNode[current.X][current.Y].parentX;  
            int currentNodeDirectionY = highResMazeNode[current.X][current.Y].Y - highResMazeNode[current.X][current.Y].parentY; 
            bool sameDirection = (currentNodeDirectionX == direction.first) && (currentNodeDirectionY == direction.second);

            float turnCost = 0.0f;
            if(!sameDirection) {
                // Low cost to encourage taking the diagonal now that it is discovered
                turnCost = 0.5f; 
            }

            float tentative_gCost = highResMazeNode[current.X][current.Y].gCost + moveCost + turnCost;
            
            if (tentative_gCost < highResMazeNode[newX][newY].gCost) {
                highResMazeNode[newX][newY].parentX = current.X;
                highResMazeNode[newX][newY].parentY = current.Y;
                highResMazeNode[newX][newY].gCost = tentative_gCost;
                highResMazeNode[newX][newY].fCost = tentative_gCost + calculateH(newX, newY);
                
                if (openSetCoords.find({newX, newY}) == openSetCoords.end()) {
                    openSet.push(highResMazeNode[newX][newY]);
                    openSetCoords.insert({newX, newY});
                }
            }
        }
	}
	std::cerr << "failure" << std::endl;
	return;
}