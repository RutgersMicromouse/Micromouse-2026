#include "API.h"

#ifdef SIM

#include <iostream>
#include <string>
#include <stack>
#include <algorithm>
#include "Flood.h"

using namespace std;

void log(const std::string& text) {
    std::cerr << text << std::endl;

}

int main(int argc, char* argv[]) {
    log("Running...");
    API::setColor(0, 0, 'G');
    API::setText(0, 0, "abc");
    initialize();

    // --- RUN 1 ---
    searchBias = 0; // Standard Logic (Priority N -> E)
    std::cerr << "Run #1: Search to Center (Standard)" << std::endl;
    runMaze('c'); 
    std::cerr << "Run #1: Discover Return" << std::endl;
    runMaze('s'); 

    // --- RUN 2 ---
    // Change Bias to "Diagonal Surfer"
    // This forces the mouse to try and hug the x=y line
    searchBias = 1; 
    std::cerr << "Run #2: Verify Center (Diagonal Surfer)" << std::endl;
    runMaze('c'); 
    std::cerr << "Run #2: Verify Return" << std::endl;
    runMaze('s'); 

    // --- RUN 3 ---
    // Change Bias to "Diagonal Surfer"
    searchBias = 2; 
    std::cerr << "Run #3: Verify Center (Diagonal Surfer)" << std::endl;
    runMaze('c'); 
    std::cerr << "Run #3: Verify Return" << std::endl;

    runMaze('s'); 

    // --- RUN 4 ---
    // Change Bias to "Diagonal Surfer"
    searchBias = 1; 
    std::cerr << "Run #3: Verify Center (Diagonal Surfer)" << std::endl;
    runMaze('c'); 
    std::cerr << "Run #3: Verify Return" << std::endl;
    runMaze('s'); 
 
    // --- ENSURE PROPER STARTING POSITION FOR SPEEDRUN ---
    switch(currentCfg.dir) {
        case 'S': API::turnLeft(); API::turnLeft(); break;
        case 'E': API::turnLeft(); break;
        case 'W': API::turnRight(); break;
        case 'N': break; // Already facing North
    }
    currentCfg.dir = 'N';
    std::cerr << "Mouse at position (" << currentCfg.x << "," << currentCfg.y 
              << ") facing " << currentCfg.dir << std::endl;
    // --- SPEEDRUN ---
    std::cerr << "Speedrun" << std::endl;
    speedrun();
}
#endif