#ifndef ROBOT_H
#define ROBOT_H

#include <Arduino.h>
#include <vector>
#include <string>

struct Coord {
    int x, y;
    bool operator==(const Coord& o) const { return x == o.x && y == o.y; }
    bool operator!=(const Coord& o) const { return !(*this == o); }
};

class Robot {
public:
    struct Cell {
        std::string previous = "";
        int value = 0; // 0: Unvisited, 1: Visited, 2: Double
    };

    enum Mode { EXPLORE, SEARCH, RACE };
    Mode currentMode = EXPLORE;

    // Constructor declaration only
    Robot(int dim);

    // Main interface
    std::pair<int, int> next_move(int sensors[3], bool beaconDetected = false);

private:
    int posX, posY;
    int lastX, lastY;
    std::string heading;
    bool isBeginning;
    bool isReversing;
    int mazeDim;

    bool goalFound;
    Coord goalCoords;

    std::vector<std::vector<int>> mazeMap;
    std::vector<std::vector<Cell>> pathMap;
    std::vector<std::vector<std::string>> policyGrid;

    // Logic Functions
    void explore(int sensors[3]);
    void updateMap(int sensors[3]);
    void checkBeacon(bool detected);
    bool isUnvisited(std::string direction);
    int calculateRotation(std::string current, std::string target);

    // Placeholders for future logic
    void findShortestPath();
};

#endif