#ifndef BATTLESHIP_H
#define BATTLESHIP_H

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <limits> //for error handling
using namespace std;

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
    string name;
    int size;
    int hitCount;
    vector<Point> occupiedCells;
    
    Ship() : name(""), size(0), hitCount(0) {}
    Ship(string n, int s) : name(n), size(s), hitCount(0) {}
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
bool placeShip(PlayerBoard&, int, istream& = cin);
bool getValidShipInfo(int&, int&, char&, PlayerBoard&, int, istream& = cin);
bool spaceOccupied(const PlayerBoard&, int, int, char, int);
bool computerPlaceShip(PlayerBoard&, int);
void playGame();
void playerAttack(PlayerBoard&, PlayerBoard&);
void computerAttack(PlayerBoard&, PlayerBoard&);
bool checkWinCondition(const PlayerBoard&);
bool isShipSunk(const Ship&);

#endif