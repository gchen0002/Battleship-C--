#ifndef BATTLESHIP_H
#define BATTLESHIP_H

#include <iomanip>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <limits> //for error handling

// Global Constants
const int CARRIER_SIZE = 5;
const int BATTLESHIP_SIZE = 4;
const int CRUISER_SIZE = 3;
const int SUBMARINE_SIZE = 3;
const int DESTROYER_SIZE = 2;
const int FLEET_SIZE = 5;
const int BOARD_SIZE = 10;

// Point Struct
struct Point {
    int row, col;
    Point(int r, int c) : row(r), col(c) {}
    Point() : row(0), col(0) {}
    
    bool operator==(const Point& other) const {
        return row == other.row && col == other.col;
    }
};

// Ship Struct
struct Ship {
    std::string name;
    int size;
    int hitCount;
    std::vector<Point> occupiedCells;
    
    Ship() : name(""), size(0), hitCount(0) {}
    Ship(std::string n, int s) : name(n), size(s), hitCount(0) {}
};

// PlayerBoard Struct
struct PlayerBoard {
    char board[10][10];
    Ship fleet[FLEET_SIZE];
};

// function prototypes
void displayBoards(char [][10], char [][10]);
void initFleet(PlayerBoard&);
void boardSetup(PlayerBoard&, PlayerBoard&);

// REMOVED: Obsolete declarations using std::cin
// bool placeShip(PlayerBoard&, int, std::istream& = std::cin);
// bool getValidShipInfo(int&, int&, char&, PlayerBoard&, int, std::istream& = std::cin);

// Add back missing declarations
bool spaceOccupied(const PlayerBoard&, int, int, char, int);
bool computerPlaceShip(PlayerBoard&, int);

// Updated prototype for placePlayerShip
bool placePlayerShip(int shipIndex, int row, int col, std::string orientationInput);

void playGame();
void playerAttack(PlayerBoard&, PlayerBoard&);
void computerAttack(PlayerBoard&, PlayerBoard&);
bool checkWinCondition(const PlayerBoard&);
bool isShipSunk(const Ship&);

#endif