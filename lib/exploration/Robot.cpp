#include "Robot.h"
#include "MazeLogic.h"

// 1. Constructor implementation
Robot::Robot(int dim) {
    mazeDim = dim;
    posX = 0;
    posY = 0;
    heading = "up";
    isBeginning = true;
    isReversing = false;
    goalFound = false;
    goalCoords = {-1, -1};

    mazeMap.assign(dim, std::vector<int>(dim, 0));
    pathMap.assign(dim, std::vector<Cell>(dim));
    policyGrid.assign(dim, std::vector<std::string>(dim, ""));
}

// 2. Mapping
void Robot::updateMap(int sensors[3]) {
    int& currentCell = mazeMap[posX][posY];
    if (sensors[1] > 0) currentCell |= MazeLogic::getWallBit(heading, "front");
    if (sensors[2] > 0) currentCell |= MazeLogic::getWallBit(heading, "right");
    if (sensors[0] > 0) currentCell |= MazeLogic::getWallBit(heading, "left");
    currentCell |= MazeLogic::getWallBit(heading, "back");
}

// 3. Trémaux Exploration
void Robot::explore(int sensors[3]) {
    updateMap(sensors);

    if (pathMap[posX][posY].value == 0) pathMap[posX][posY].value = 1;

    std::string directions[] = {"up", "right", "down", "left"};
    std::vector<std::string> options;

    for (const std::string& dir : directions) {
        if (MazeLogic::isPermissible(mazeMap[posX][posY], dir)) {
            if (dir != MazeLogic::getOpposite(heading)) {
                options.push_back(dir);
            }
        }
    }

    std::string chosenDir = "";
    if (options.empty()) {
        chosenDir = MazeLogic::getOpposite(heading);
        pathMap[posX][posY].value = 2;
    } else {
        for (const std::string& opt : options) {
            if (isUnvisited(opt)) {
                chosenDir = opt;
                break;
            }
        }
        if (chosenDir == "") {
            chosenDir = MazeLogic::getOpposite(heading);
            pathMap[posX][posY].value = 2;
        }
    }

    heading = chosenDir;
    if (heading == "up") posY++;
    else if (heading == "right") posX++;
    else if (heading == "down") posY--;
    else if (heading == "left") posX--;
}

// 4. Interface
std::pair<int, int> Robot::next_move(int sensors[3], bool beaconDetected) {
    checkBeacon(beaconDetected);
    
    std::string oldHeading = heading;
    if (currentMode == EXPLORE) {
        explore(sensors);
        if (posX == 0 && posY == 0 && !isBeginning) currentMode = SEARCH;
        isBeginning = false;
    }

    int rotation = calculateRotation(oldHeading, heading);
    return {rotation, 1};
}

// 5. Helpers
void Robot::checkBeacon(bool detected) {
    if (detected && !goalFound) {
        goalFound = true;
        goalCoords = {posX, posY};
    }
}

bool Robot::isUnvisited(std::string direction) {
    int nx = posX, ny = posY;
    if (direction == "up") ny++;
    else if (direction == "right") nx++;
    else if (direction == "down") ny--;
    else if (direction == "left") nx--;

    if (nx < 0 || nx >= mazeDim || ny < 0 || ny >= mazeDim) return false;
    return pathMap[nx][ny].value == 0;
}

int Robot::calculateRotation(std::string current, std::string target) {
    if (current == target) return 0;
    std::string dirs[] = {"up", "right", "down", "left"};
    int curIdx = 0, tarIdx = 0;
    for(int i=0; i<4; i++) {
        if(dirs[i] == current) curIdx = i;
        if(dirs[i] == target) tarIdx = i;
    }
    int diff = tarIdx - curIdx;
    if (diff == 1 || diff == -3) return 90;
    if (diff == -1 || diff == 3) return -90;
    return 180;
}

void Robot::findShortestPath() {}