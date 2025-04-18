#include "header.h"
//Computer attack global variables
vector<Point> lastHitCells;
vector<Point> potentialTargets;
bool huntMode = true;

// Display Boards Function
// INPUT: playerBoard and enemyBoard
// OUTPUT: Display of both boards
// PROCESSING: Loops through each row and column of the boards and prints the corresponding characters
void displayBoards(char playerBoard[][10], char enemyBoard[][10]) {
    // Board labeling and numbering
    cout << endl;
    cout << setw(28) << "Your Board" << setw(53) << "Enemy Board" << endl;
    cout << "   ";
    for (int i = 1; i <= 10; i++) {
        cout << setw(4) << i;
    }
    cout << setw(13) << "";
    for (int i = 1; i <= 10; i++) {
        cout << setw(4) << i;
    }
    cout << endl;

    cout << "  -----------------------------------------";
    cout << setw(53) << " -----------------------------------------" << endl;
    // Rows labeling 
    char rowLabels[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J'};

    for (int row = 0; row < 10; row++) {
        cout << rowLabels[row] << " ";
        for (int col = 0; col < 10; col++) {
            cout << " | " << playerBoard[row][col];
        }
        cout << " |";
        cout << setw(11);
        cout << rowLabels[row] << " ";
        // Displaying enemy board
        for (int col = 0; col < 10; col++) {
            if (enemyBoard[row][col] == 'H' || enemyBoard[row][col] == 'M') {
                cout << " | " << enemyBoard[row][col];
            } else {
                cout << " " << " | ";
            }
        }
        cout << " |";
        cout << endl;
        cout << "  -----------------------------------------";
        cout << setw(54) << "   -----------------------------------------" << endl;
    }
}

// Initialize Fleet Function
// INPUT: player object
// OUTPUT: Initialized player object with fleet and empty board
// PROCESSING: Initializes the fleet of the player and sets the board to empty
void initFleet(PlayerBoard& player) {
    player.fleet[0] = Ship("Carrier", CARRIER_SIZE);
    player.fleet[1] = Ship("Battleship", BATTLESHIP_SIZE);
    player.fleet[2] = Ship("Cruiser", CRUISER_SIZE);
    player.fleet[3] = Ship("Submarine", SUBMARINE_SIZE);
    player.fleet[4] = Ship("Destroyer", DESTROYER_SIZE);

    // Initialize board with empty spaces
    for (int row = 0; row < 10; row++) {
        for (int col = 0; col < 10; col++) {
            player.board[row][col] = ' ';
        }
    }
}

// Board Setup Function
// INPUT: player1 and player2 objects
// OUTPUT: Display of both boards after ship placement
// PROCESSING: Loops through each ship of each player and places them on the board
void boardSetup(PlayerBoard& player1, PlayerBoard& player2) {
    // Open input file for Player 1's ship placement
    ifstream inputFile("input.txt");
    if (!inputFile) {
        cerr << "Unable to open input.txt\n";
        return;
    }

    cout << "Player 1 Ship Placement:\n";
    for (int i = 0; i < FLEET_SIZE; i++) {
        while (!placeShip(player1, i, inputFile)) {
            cout << "Retrying ship placement for " << player1.fleet[i].name << ".\n";
            // Reset file stream if reading failed
            inputFile.clear();
            inputFile.seekg(0);
        }
        displayBoards(player1.board, player2.board);
    }
    inputFile.close();

    cout << "Computer Ship Placement:\n";
    for (int i = 0; i < FLEET_SIZE; i++) {
        while (!computerPlaceShip(player2, i)) {
            cout << "Retrying computer ship placement for " << player2.fleet[i].name << ".\n";
        }
        displayBoards(player1.board, player2.board);
    }
}

// Overloaded placeShip function to accept input stream
// INPUT: player object, ship index, and input stream
// OUTPUT: True if ship is successfully placed, false otherwise
// PROCESSING: Loops until a valid ship placement is found, then marks the ship on the board
bool placeShip(PlayerBoard& player, int shipIndex, istream& inputStream) {
    int row, col;
    char orientation;

    while (!getValidShipInfo(row, col, orientation, player, shipIndex, inputStream)) {
        cout << "Invalid ship placement. Try again.\n";
        if (&inputStream != &cin) return false;
    }

    // Mark ship on board
    Ship& ship = player.fleet[shipIndex];
    ship.occupiedCells.clear();

    for (int i = 0; i < ship.size; i++) {
        if (orientation == 'H') {
            player.board[row][col + i] = 's';
            ship.occupiedCells.push_back(Point(row, col + i));
        } else {
            player.board[row + i][col] = 's';
            ship.occupiedCells.push_back(Point(row + i, col));
        }
    }

    return true;
}
// Place Ship Function
// INPUT: player object and ship index
// OUTPUT: True if ship is successfully placed, false otherwise
// PROCESSING: Loops until a valid ship placement is found, then marks the ship on the board
bool placeShip(PlayerBoard& player, int shipIndex) {
    int row, col;
    char orientation;

    while (!getValidShipInfo(row, col, orientation, player, shipIndex)) {
        cout << "Invalid ship placement. Try again.\n";
    }

    // Mark ship on board
    Ship& ship = player.fleet[shipIndex];
    ship.occupiedCells.clear();

    for (int i = 0; i < ship.size; i++) {
        if (orientation == 'H') {
            player.board[row][col + i] = 's';
            ship.occupiedCells.push_back(Point(row, col + i));
        } else {
            player.board[row + i][col] = 's';
            ship.occupiedCells.push_back(Point(row + i, col));
        }
    }

    return true;
}

// Get Valid Ship Info Function
// INPUT: row, column, orientation, player object, ship index, and input stream
// OUTPUT: True if ship info is valid, false otherwise
// PROCESSING: Prompts the user for ship placement info and validates it
bool getValidShipInfo(int& row, int& col, char& orientation, PlayerBoard& player, int shipIndex, istream& inputStream) {
    string coordinates;
    Ship& ship = player.fleet[shipIndex];

    // Prompt for ship coordinates
    cout << "Placing " << ship.name << " (Size: " << ship.size << ")\n";
    
    // Use the input stream to read coordinates
    if (!(inputStream >> coordinates)) {
        cout << "Error reading coordinates.\n";
        return false;
    }

    // Convert coordinates to row and column
    if (coordinates.length() < 2) {
        cout << "Invalid coordinate format.\n";
        return false;
    }
    row = coordinates[0] - 'A';
    
    try {
        col = stoi(coordinates.substr(1)) - 1;
    } catch (...) {
        cout << "Invalid column number.\n";
        return false;
    }

    // Validate row and column
    if (row < 0 || row >= 10 || col < 0 || col >= 10) {
        cout << "Coordinates out of bounds.\n";
        return false;
    }

    // Read orientation
    if (!(inputStream >> orientation)) {
        cout << "Error reading orientation.\n";
        return false;
    }
    orientation = toupper(orientation);

    if (orientation != 'H' && orientation != 'V') {
        cout << "Invalid orientation. Use H or V.\n";
        return false;
    }

    // Check if ship fits on board
    if (orientation == 'H' && col + ship.size > 10) {
        cout << "Ship extends beyond board horizontally.\n";
        return false;
    }
    if (orientation == 'V' && row + ship.size > 10) {
        cout << "Ship extends beyond board vertically.\n";
        return false;
    }

    // Check if space is occupied
    if (spaceOccupied(player, row, col, orientation, ship.size)) {
        cout << "Space is already occupied.\n";
        return false;
    }

    return true;
}

// Space Occupied Function
// INPUT: player object, row, column, orientation, and ship size
// OUTPUT: True if space is occupied, false otherwise
// PROCESSING: Checks if the space on the board is already occupied
bool spaceOccupied(const PlayerBoard& player, int row, int col, char orientation, int shipSize) {
    for (int i = 0; i < shipSize; i++) {
        if (orientation == 'H') {
            if (player.board[row][col + i] != ' ') {
                return true;
            }
        } else {
            if (player.board[row + i][col] != ' ') {
                return true;
            }
        }
    }
    return false;
}

// Check if a ship is sunk
// INPUT: ship object
// OUTPUT: True if ship is sunk, false otherwise
// PROCESSING: Checks if the ship's hit count is equal to its size
bool isShipSunk(const Ship& ship) {
    return ship.hitCount == ship.size;
}

// Play Game Function
// INPUT: None
// OUTPUT: Game result
// PROCESSING: Initializes the players, sets up the board, and starts the game loop
void playGame() {
    PlayerBoard player1, player2;
    initFleet(player1);
    initFleet(player2);

    boardSetup(player1, player2);

    bool gameOver = false;
    int currentPlayer = 1;

    while (!gameOver) {
        if (currentPlayer == 1) {
            displayBoards(player1.board, player2.board);
            playerAttack(player1, player2);
            if (checkWinCondition(player2)) {
                cout << "Player 1 wins!\n";
                gameOver = true;
            }
            currentPlayer = 2;
        } else {
            displayBoards(player1.board, player2.board);
            computerAttack(player1, player2);
            if (checkWinCondition(player1)) {
                cout << "Computer wins!\n";
                gameOver = true;
            }
            currentPlayer = 1;
        }
    }
}

// Player Attack Function
// INPUT: Attacker and defender player objects
// OUTPUT: Attack result
// PROCESSING: Prompts the player for attack coordinates, validates them, and processes the attack
void playerAttack(PlayerBoard& attacker, PlayerBoard& defender) {
    while (true) {
        try {
            string coordinates;
            cout << "Enter attack coordinates (e.g., A5): ";
            cin >> coordinates;

            // Validate coordinate length
            if (coordinates.length() < 2) {
                throw invalid_argument("Invalid coordinate format. Use a letter (A-J) followed by a number (1-10).");
            }

            // Convert row
            char rowChar = toupper(coordinates[0]);
            if (rowChar < 'A' || rowChar > 'J') {
                throw invalid_argument("Invalid row. Use a letter between A and J.");
            }
            int row = rowChar - 'A';

            // Convert column
            int col;
            try {
                col = stoi(coordinates.substr(1)) - 1;
            } catch (...) {
                throw invalid_argument("Invalid column. Use a number between 1 and 10.");
            }

            // Validate column
            if (col < 0 || col >= 10) {
                throw invalid_argument("Column out of bounds. Use a number between 1 and 10.");
            }

            // Check if coordinates have already been attacked
            if (defender.board[row][col] == 'H' || defender.board[row][col] == 'M') {
                throw invalid_argument("You have already attacked these coordinates. Choose a different location.");
            }

            // Proceed with attack
            if (defender.board[row][col] == 's') {
                cout << "Hit!\n";
                defender.board[row][col] = 'H';

                // Check which ship was hit
                for (Ship& ship : defender.fleet) {
                    for (Point& p : ship.occupiedCells) {
                        if (p.row == row && p.col == col) {
                            ship.hitCount++;
                            
                            // Check if ship is sunk
                            if (isShipSunk(ship)) {
                                cout << ship.name << " is sunk!\n";
                            }
                            break;
                        }
                    }
                }
            } else if (defender.board[row][col] == ' ') {
                cout << "Miss!\n";
                defender.board[row][col] = 'M';
            }

            // Successfully completed attack, exit the loop
            break;

        } catch (const std::invalid_argument& e) {
            // Catch and display specific error message
            cout << "Error: " << e.what() << endl;
            cout << "Please try again.\n";
            
            // Clear input stream to prevent infinite loop
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}

// Computer Attack Function
// INPUT: player and computer player objects
// OUTPUT: Attack result
// PROCESSING: Implements smart targeting logic for the computer's attack
void computerAttack(PlayerBoard& player, PlayerBoard& computer) {
    int row, col;
    
    // Smart targeting logic
    if (!huntMode && !potentialTargets.empty()) {
        // Target adjacent cells of previous hits
        Point target = potentialTargets.back();
        potentialTargets.pop_back();
        
        row = target.row;
        col = target.col;
    } else {
        // Random targeting (hunt mode)
        do {
            row = rand() % 10;
            col = rand() % 10;
        } while (player.board[row][col] == 'H' || player.board[row][col] == 'M');
    }

    if (player.board[row][col] == 's') {
        cout << "Computer hit at " << char(row + 'A') << col + 1 << "!\n";
        player.board[row][col] = 'H';
        
        // Record the hit
        lastHitCells.push_back(Point(row, col));
        huntMode = false;

        // Add potential adjacent targets
        vector<Point> adjacentCells = {
            Point(row-1, col), // Up
            Point(row+1, col), // Down
            Point(row, col-1), // Left
            Point(row, col+1)  // Right
        };

        for (const Point& p : adjacentCells) {
            // Check if adjacent cell is valid and not already attacked
            if (p.row >= 0 && p.row < 10 && p.col >= 0 && p.col < 10 &&
                player.board[p.row][p.col] != 'H' && 
                player.board[p.row][p.col] != 'M') {
                potentialTargets.push_back(p);
            }
        }
    } else if (player.board[row][col] == ' ') {
        cout << "Computer missed at " << char(row + 'A') << col + 1 << "!\n";
        player.board[row][col] = 'M';

        // If we missed a potential target, remove it
        auto it = find(potentialTargets.begin(), potentialTargets.end(), Point(row, col));
        if (it != potentialTargets.end()) {
            potentialTargets.erase(it);
        }

        // Check if we've exhausted all potential targets
        if (potentialTargets.empty()) {
            huntMode = true;
            lastHitCells.clear();
        }
    } else {
        // If the cell was already hit or missed, try again
        computerAttack(player, computer);
        return;
    }

    // Check which ship was hit
    for (Ship& ship : player.fleet) {
        for (Point& p : ship.occupiedCells) {
            if (p.row == row && p.col == col) {
                ship.hitCount++;
                
                // Check if ship is sunk
                if (isShipSunk(ship)) {
                    cout << ship.name << " is sunk!\n";
                    
                    // Reset targeting when a ship is sunk
                    huntMode = true;
                    potentialTargets.clear();
                    lastHitCells.clear();
                }
                break;
            }
        }
    }
}

// Check Win Condition Function
// INPUT: player object
// OUTPUT: True if player has won, false otherwise
// PROCESSING: Checks if all of the player's ships have been sunk
bool checkWinCondition(const PlayerBoard& player) {
    for (const Ship& ship : player.fleet) {
        if (ship.hitCount < ship.size) {
            return false;
        }
    }
    return true;
}

// Computer Place Ship Function
// INPUT: computer object and ship index
// OUTPUT: True if ship is successfully placed, false otherwise
// PROCESSING: Randomly places the computer's ships on the board
bool computerPlaceShip(PlayerBoard& computer, int shipIndex) {
    int row, col;
    char orientation;
    int attempts = 0;

    do {
        // Randomly choose orientation
        orientation = (rand() % 2 == 0) ? 'H' : 'V';
        
        // Randomly choose starting position
        row = rand() % 10;
        col = rand() % 10;

        // Ensure ship fits on board and doesn't overlap
        if (orientation == 'H' && col + computer.fleet[shipIndex].size <= 10) {
            if (!spaceOccupied(computer, row, col, orientation, computer.fleet[shipIndex].size)) {
                break;
            }
        } else if (orientation == 'V' && row + computer.fleet[shipIndex].size <= 10) {
            if (!spaceOccupied(computer, row, col, orientation, computer.fleet[shipIndex].size)) {
                break;
            }
        }

        attempts++;
    } while (attempts < 100);

    if (attempts >= 100) {
        cout << "Failed to place ship randomly\n";
        return false;
    }

    // Mark ship on board
    Ship& ship = computer.fleet[shipIndex];
    ship.occupiedCells.clear();

    for (int i = 0; i < ship.size; i++) {
        if (orientation == 'H') {
            computer.board[row][col + i] = 's';
            ship.occupiedCells.push_back(Point(row, col + i));
        } else {
            computer.board[row + i][col] = 's';
            ship.occupiedCells.push_back(Point(row + i, col));
        }
    }

    return true;
}