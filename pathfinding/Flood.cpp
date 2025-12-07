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
    std::cerr << "--- SPEEDRUN DEBUG MODE ACTIVE ---" << std::endl;
    std::cerr << "Speedrun start: (" << currentCfg.x << "," << currentCfg.y 
              << ") " << currentCfg.dir << std::endl;
    
    // 1. USE STATIC MEMORY
    static bool highResMaze[33][33]; 
    static Node highResMazeNode[33][33];
    static bool inOpenSet[33][33];

    // 2. RESET DATA STRUCTURES
    for(int i=0; i<33; i++) {
        for(int j=0; j<33; j++) {
            highResMaze[i][j] = false;
            inOpenSet[i][j] = false;
            
            highResMazeNode[i][j].X = i;
            highResMazeNode[i][j].Y = j;
            highResMazeNode[i][j].parentX = -1;
            highResMazeNode[i][j].parentY = -1;
            highResMazeNode[i][j].gCost = 10000.0f;
            highResMazeNode[i][j].fCost = 10000.0f;
        }
    }

    // 3. MAP WALLS
    for(int i = 0; i < 33; i++) {
        highResMaze[i][0] = true; highResMaze[i][32] = true; 
        highResMaze[0][i] = true; highResMaze[32][i] = true; 
    }

    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            int hX = 2*i+1;
            int hY = 2*j+1;
            openCells cell = walls[i][j];

            if(!cell.openN) { highResMaze[hX][hY+1]=true; highResMaze[hX-1][hY+1]=true; highResMaze[hX+1][hY+1]=true; }
            if(!cell.openS) { highResMaze[hX][hY-1]=true; highResMaze[hX-1][hY-1]=true; highResMaze[hX+1][hY-1]=true; }
            if(!cell.openE) { highResMaze[hX+1][hY]=true; highResMaze[hX+1][hY-1]=true; highResMaze[hX+1][hY+1]=true; }
            if(!cell.openW) { highResMaze[hX-1][hY]=true; highResMaze[hX-1][hY-1]=true; highResMaze[hX-1][hY+1]=true; }

            if(!cell.visited) {
                for(int dx=-1; dx<=1; dx++) 
                    for(int dy=-1; dy<=1; dy++) 
                        highResMaze[hX+dx][hY+dy] = true;
            }
        }
    }

    // 4. INITIALIZE HEURISTICS
    for (int i = 1; i < 32; i++) {
        for (int j = 1; j < 32; j++) {
            highResMazeNode[i][j].hCost = calculateH(i, j);
        }
    }
    
    highResMazeNode[1][1].parentX = 1;
    highResMazeNode[1][1].parentY = 0;
    highResMazeNode[1][1].gCost = 0;
    highResMazeNode[1][1].fCost = highResMazeNode[1][1].hCost; 

    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openSet;
    openSet.push(highResMazeNode[1][1]);
    inOpenSet[1][1] = true;
    
    std::vector<std::pair<int, int>> directions = {
        {0,1}, {0,-1}, {-1,0}, {1,0}, {-1,1}, {1,1}, {-1,-1}, {1,-1}  
    };

    while(!openSet.empty()) {
        Node current = openSet.top();
        openSet.pop();
        inOpenSet[current.X][current.Y] = false;
        
        // TARGET CHECK
        if((current.X >= 15 && current.X <= 17) && (current.Y >= 15 && current.Y <= 17)) {
            std::cerr << "Path found! Reconstructing..." << std::endl;
            std::vector<std::pair<int, int>> path;
            
            // Reconstruct
            int safety = 0;
            while (!(current.X == 1 && current.Y == 1)) {
                
                path.push_back({current.X, current.Y});

                // --- CRITICAL SAFETY CHECK ---
                if (current.parentX < 0 || current.parentX > 32 || 
                    current.parentY < 0 || current.parentY > 32) {
                     std::cerr << "CRITICAL ERROR: Invalid Parent Index! Path broken." << std::endl;
                     return; 
                }

                current = highResMazeNode[current.parentX][current.parentY];
                
                if(++safety > 2000) {
                    std::cerr << "CRITICAL ERROR: Infinite Loop in reconstruction." << std::endl;
                    return; 
                }
            }
            path.push_back({1, 1});
            path.push_back({1, 0});
            std::reverse(path.begin(), path.end());

            if (path.size() < 2) {
                 std::cerr << "ERROR: Path too short!" << std::endl;
                 return;
            }

            // Generate Commands
            std::vector<std::pair<char, double>> commands;
            std::map<std::pair<int, int>, double> dirAngles = {
                {{0,1}, 0.0}, {{0,-1}, 180.0}, {{-1,0}, 270.0}, {{1,0}, 90.0},
                {{-1,1}, 315.0}, {{1,1}, 45.0}, {{-1,-1}, 225.0}, {{1,-1}, 135.0}
            };

            for (size_t i = 2; i < path.size(); ++i) {
                int dx = path[i].first - path[i-1].first;
                int dy = path[i].second - path[i-1].second;
                int pdx = path[i-1].first - path[i-2].first;
                int pdy = path[i-1].second - path[i-2].second;
                
                if (dx == pdx && dy == pdy) {
                    if (!commands.empty() && commands.back().first == 'F') 
                        commands.back().second += 1;
                    else 
                        commands.push_back({'F', 1.0});
                } else {
                    double angle = dirAngles[{dx, dy}];
#ifdef SIM
                    double prevAngle = dirAngles[{pdx, pdy}];
                    double diff = angle - prevAngle;
                    if (diff > 180.0) diff -= 360.0;
                    else if (diff < -180.0) diff += 360.0;
                    while(abs(diff) > 1.0) {
                        if (abs(diff) > 45.0) { 
                            commands.push_back(diff > 0 ? std::make_pair('R',90.0) : std::make_pair('L',90.0));
                            diff += (diff > 0 ? -90.0 : 90.0);
                        } else {
                            commands.push_back(diff > 0 ? std::make_pair('R',45.0) : std::make_pair('L',45.0));
                            diff += (diff > 0 ? -45.0 : 45.0);
                        }
                    }
#endif
#ifdef REAL
                    commands.push_back({'T', angle});
#endif
                    commands.push_back({'F', 1});
                }
            }
            
            std::cerr << "Executing " << commands.size() << " commands." << std::endl;

            for (auto cmd : commands) {
                if(cmd.first == 'F') API::moveForwardHalf((int)cmd.second);
                else if(cmd.first == 'L') (cmd.second == 45) ? API::turnLeft45() : API::turnLeft();
                else if(cmd.first == 'R') (cmd.second == 45) ? API::turnRight45() : API::turnRight();
            }
            return;
        }

        for (auto d : directions) {
            // --- CRITICAL FIX: SAFETY START ---
            // If we are at the Start (1,1), DO NOT allow diagonal moves.
            // This forces the robot to move North (0,1) first, preventing a crash into the start post.
            if (current.X == 1 && current.Y == 1 && (d.first != 0 && d.second != 0)) continue;
            // ----------------------------------

            int nX = current.X + d.first;
            int nY = current.Y + d.second;

            if (nX <= 0 || nX >= 32 || nY <= 0 || nY >= 32) continue;
            if(highResMaze[nX][nY]) continue;

            bool isDiag = (d.first != 0 && d.second != 0);
            float newGCost = highResMazeNode[current.X][current.Y].gCost + (isDiag ? 1.414f : 1.0f);

            int cDX = highResMazeNode[current.X][current.Y].X - highResMazeNode[current.X][current.Y].parentX;
            int cDY = highResMazeNode[current.X][current.Y].Y - highResMazeNode[current.X][current.Y].parentY;
            if (cDX != d.first || cDY != d.second) newGCost += 0.5f;

            if (newGCost < highResMazeNode[nX][nY].gCost) {
                highResMazeNode[nX][nY].parentX = current.X;
                highResMazeNode[nX][nY].parentY = current.Y;
                highResMazeNode[nX][nY].gCost = newGCost;
                
                highResMazeNode[nX][nY].fCost = newGCost + calculateH(nX, nY);
                
                if (!inOpenSet[nX][nY]) {
                    openSet.push(highResMazeNode[nX][nY]);
                    inOpenSet[nX][nY] = true;
                }
            }
        }
    }
    std::cerr << "Speedrun failed: No path." << std::endl;
}