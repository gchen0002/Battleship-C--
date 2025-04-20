#include "header.h"
#include <cctype> 
#include <string>     // Added for std::string
#include <sstream>    // Added for std::ostringstream
#include "libs/nlohmann/json.hpp" // Added for JSON
#include <cstdio>     // Added for printf

// Computer attack global variables
std::vector<Point> lastHitCells;
std::vector<Point> potentialTargets;
bool huntMode = true;

// --- Global Game State ---
PlayerBoard player1;       // Human player
PlayerBoard player2;       // Computer player
int currentPlayer = 1;   // 1 for Player 1 (Human), 2 for Player 2 (Computer)
bool gameOver = false;     // Game status flag

// Display Boards Function - REMOVED (Handled by JS Frontend)
/*
void displayBoards(char playerBoard[][10], char enemyBoard[][10]) {
    // ... implementation removed ...
}
*/

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

// Board Setup Function - MODIFIED (Removed file input and display calls)
// INPUT: player1 and player2 objects
// OUTPUT: Boards are set up internally
// PROCESSING: Loops through each ship of each player and places them on the board
void boardSetup(PlayerBoard& player1, PlayerBoard& player2) {
    // REMOVED: Player 1 Ship Placement from input.txt
    // This will need to be replaced by calls from JS using player-provided coordinates

    // TODO: Add logic here to place Player 1 ships based on JS input
    // For now, we'll just initialize the computer

    // REMOVED: cout << "Player 1 Ship Placement:\n";
    // REMOVED: Loop calling placeShip(player1, i, inputFile) and displayBoards

    // Computer Ship Placement
    // REMOVED: cout << "Computer Ship Placement:\n";
    for (int i = 0; i < FLEET_SIZE; i++) {
        while (!computerPlaceShip(player2, i)) {
            // REMOVED: cout << "Retrying computer ship placement ...";
            // Consider adding a max retry count or error handling here
        }
        // REMOVED: displayBoards(player1.board, player2.board);
    }
}

// Overloaded placeShip function to accept input stream - REMOVED
/*
bool placeShip(PlayerBoard& player, int shipIndex, istream& inputStream) {
    // ... implementation removed ...
}
*/

// Place Ship Function - Keep this version, will be adapted later
// INPUT: player object and ship index
// OUTPUT: True if ship is successfully placed, false otherwise
// PROCESSING: Loops until a valid ship placement is found, then marks the ship on the board
// REMOVED: Unused placeShip function definition
/*
bool placeShip(PlayerBoard& player, int shipIndex) {
    int row, col;
    char orientation;

    // MODIFIED: Removed loop and validation call - This function will now assume valid input is passed
    // The validation logic from getValidShipInfo will be reused or adapted elsewhere
    // when creating the JS-callable placement function.

    // REMOVED: while (!getValidShipInfo(row, col, orientation, player, shipIndex)) {
    // REMOVED:    cout << "Invalid ship placement. Try again.\n";
    // REMOVED: }

    // TODO: This function needs parameters for row, col, orientation passed from JS
    // For now, its structure remains but it's not directly usable for player placement.


    Ship& ship = player.fleet[shipIndex];
    ship.occupiedCells.clear();

    // This part remains, placing the ship based on (currently undefined) row, col, orientation
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
*/

// Get Valid Ship Info Function (istream version) - REMOVED
/*
bool getValidShipInfo(int& row, int& col, char& orientation, PlayerBoard& player, int shipIndex, istream& inputStream) {
    // ... implementation removed ...
}
*/

// Get Valid Ship Info Function (console version) - REMOVED (Logic will be reused/adapted)
/*
bool getValidShipInfo(int& row, int& col, char& orientation, PlayerBoard& player, int shipIndex) {
    // ... implementation removed ...
    // Contains validation logic we need to reuse in a JS-callable function
}
*/


// Space Occupied Function - REMAINS UNCHANGED
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

// Check if a ship is sunk - REMAINS UNCHANGED
// INPUT: ship object
// OUTPUT: True if ship is sunk, false otherwise
// PROCESSING: Checks if the ship's hit count is equal to its size
bool isShipSunk(const Ship& ship) {
    return ship.hitCount == ship.size;
}

// Play Game Function - REMOVED (Handled by JS interaction)
/*
void playGame() {
    // ... implementation removed ...
}
*/


// Player Attack Function - MODIFIED
// INPUT: row, col for attack (from JS)
// OUTPUT: String indicating attack result: 
//         "invalid_turn", "invalid_game_over", "invalid_bounds", 
//         "invalid_already_attacked", "miss", "hit", "sunk <ship_name>", "win Player 1"
// PROCESSING: Processes the player's attack on the computer's board (player2)
std::string makePlayerAttack(int row, int col) {
    // --- Initial Checks ---
    if (gameOver) {
        return "invalid_game_over";
    }
    if (currentPlayer != 1) {
        return "invalid_turn";
    }
    // Validate coordinate bounds (essential as input comes from JS)
    if (row < 0 || row >= 10 || col < 0 || col >= 10) {
        return "invalid_bounds";
    }
    // Check if coordinates have already been attacked
    if (player2.board[row][col] == 'H' || player2.board[row][col] == 'M') {
        return "invalid_already_attacked";
    }

    // --- Process Attack --- 
    std::string result = "miss"; // Default result

    if (player2.board[row][col] == 's') { // It's a hit!
        player2.board[row][col] = 'H';
        result = "hit";

        // Check which ship was hit and if it sunk
        for (Ship& ship : player2.fleet) {
            for (Point& p : ship.occupiedCells) {
                if (p.row == row && p.col == col) {
                    ship.hitCount++;
                    if (isShipSunk(ship)) {
                        // Use ostringstream to build the sunk message
                        std::ostringstream oss;
                        oss << "sunk " << ship.name;
                        result = oss.str(); 
                    }
                    goto post_attack_check; // Skip further ship checks
                }
            }
        }
    } else if (player2.board[row][col] == ' ') { // It's a miss
        player2.board[row][col] = 'M';
        result = "miss";
    } else {
        // Should not happen if already_attacked check works, but as a fallback:
        return "invalid_internal_state"; 
    }

post_attack_check:;

    // --- Post-Attack Updates ---

    // Check for win condition ONLY if it was a hit/sunk
    if (result.rfind("hit", 0) == 0 || result.rfind("sunk", 0) == 0) { // Check if result starts with "hit" or "sunk"
        if (checkWinCondition(player2)) {
            gameOver = true;
            return "win Player 1"; // Game over, Player 1 wins
        }
    }

    // Switch turn to computer
    currentPlayer = 2;

    return result; // Return "hit", "miss", or "sunk <ship_name>"
}

// Computer Attack Function - MODIFIED
// INPUT: None (uses global state)
// OUTPUT: String indicating attack result and coordinates:
//         "invalid_turn", "invalid_game_over", 
//         "miss <coord>", "hit <coord>", "sunk <ship_name> <coord>", "win Computer <coord>"
// PROCESSING: Implements computer's attack logic on player1's board
std::string makeComputerAttack() {
    // --- Initial Checks ---
    if (gameOver) {
        return "invalid_game_over";
    }
    if (currentPlayer != 2) {
        return "invalid_turn";
    }

    int row, col;
    
    // --- Choose Target Coordinates --- 
    // Smart targeting logic (hunt/target mode)
    if (!huntMode && !potentialTargets.empty()) {
        // Target adjacent cells of previous hits
        Point target = potentialTargets.back();
        potentialTargets.pop_back();
        row = target.row;
        col = target.col;
        
        // Basic validation for potential target (in case board state changed unexpectedly)
        if (row < 0 || row >= 10 || col < 0 || col >= 10 || player1.board[row][col] == 'H' || player1.board[row][col] == 'M') {
             // Invalid target, fall back to random or pick next potential
             // For simplicity, just retry the whole attack sequence
             return makeComputerAttack(); 
        }

    } else {
        // Random targeting (hunt mode)
        huntMode = true; // Ensure hunt mode if potential targets were empty
        lastHitCells.clear();
        int attempts = 0;
        do {
            row = rand() % 10;
            col = rand() % 10;
            attempts++;
            if (attempts > 200) { // Prevent infinite loop on a full board (unlikely)
                return "invalid_internal_state_cannot_find_target"; 
            }
        } while (player1.board[row][col] == 'H' || player1.board[row][col] == 'M');
    }

    // --- Process Attack --- 
    std::string resultType = "miss"; // Default result type
    std::string sunkShipName = "";

    if (player1.board[row][col] == 's') { // It's a hit!
        player1.board[row][col] = 'H';
        resultType = "hit";
        
        // Record the hit for smart targeting
        lastHitCells.push_back(Point(row, col));
        huntMode = false;

        // Add potential adjacent targets
        std::vector<Point> adjacentCells = {
            Point(row-1, col), Point(row+1, col), Point(row, col-1), Point(row, col+1)
        };
        for (const Point& p : adjacentCells) {
            if (p.row >= 0 && p.row < 10 && p.col >= 0 && p.col < 10 &&
                player1.board[p.row][p.col] != 'H' && player1.board[p.row][p.col] != 'M') {
                // Avoid adding duplicates
                bool already_potential = false;
                for(const auto& pt : potentialTargets) {
                     if (pt.row == p.row && pt.col == p.col) { already_potential = true; break; }
                }
                if (!already_potential) potentialTargets.push_back(p);
            }
        }

        // Check which ship was hit and if it sunk
        for (Ship& ship : player1.fleet) {
            for (Point& p : ship.occupiedCells) {
                if (p.row == row && p.col == col) {
                    ship.hitCount++;
                    if (isShipSunk(ship)) {
                        resultType = "sunk";
                        sunkShipName = ship.name;
                        // Reset targeting state after sinking a ship
                        huntMode = true;
                        potentialTargets.clear();
                        lastHitCells.clear();
                    }
                    goto post_computer_attack_check; // Skip further ship checks
                }
            }
        }

    } else if (player1.board[row][col] == ' ') { // It's a miss
        player1.board[row][col] = 'M';
        resultType = "miss";

        // If we missed a potential target, remove it (if it exists)
        potentialTargets.erase(std::remove_if(potentialTargets.begin(), 
                                              potentialTargets.end(), 
                                              [row, col](const Point& p){ return p.row == row && p.col == col; }),
                             potentialTargets.end());

        // If no more potential targets, go back to hunt mode
        if (potentialTargets.empty() && !huntMode) { 
            huntMode = true;
            lastHitCells.clear();
        }
    } else {
        // Should not happen if target selection logic works, but as a fallback:
        return "invalid_internal_state_attacked_hit_cell";
    }

post_computer_attack_check:;

    // --- Format Coordinate String --- 
    std::ostringstream coord_oss;
    coord_oss << char(row + 'A') << (col + 1);
    std::string coordStr = coord_oss.str();

    // --- Check for Win Condition --- 
    if (resultType == "hit" || resultType == "sunk") {
        if (checkWinCondition(player1)) {
            gameOver = true;
            return "win Computer " + coordStr; // Game over, Computer wins
        }
    }

    // --- Build Final Result String --- 
    std::ostringstream result_oss;
    result_oss << resultType;
    if (resultType == "sunk") {
        result_oss << " " << sunkShipName;
    }
    result_oss << " " << coordStr;
    
    // --- Switch Turn --- 
    currentPlayer = 1;

    return result_oss.str();
}

// Check Win Condition Function - REMAINS UNCHANGED
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

// Computer Place Ship Function - MODIFIED (Removed output)
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
        // REMOVED: cout << "Failed to place ship randomly\n";
        return false; // Indicate failure
    }

    // Mark ship on board (logic remains)
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

// Initializes the game state, clears boards, sets fleets, resets computer AI.
void initializeGame() {
    // Seed random number generator (should only be called once)
    std::srand(std::time(nullptr));

    // Initialize player boards and fleets
    initFleet(player1);
    initFleet(player2);

    // Reset game status
    currentPlayer = 1; // Player 1 starts
    gameOver = false;

    // Reset computer AI state
    lastHitCells.clear();
    potentialTargets.clear();
    huntMode = true;

    // Note: Ship placement is handled separately after initialization.
    // The original boardSetup logic is split.
}

// Places all computer ships randomly on the board.
// Called after player placement is complete.
void placeComputerShips() {
    for (int i = 0; i < FLEET_SIZE; i++) {
        // computerPlaceShip includes logic to retry placement
        if (!computerPlaceShip(player2, i)) {
            // Optional: Handle the unlikely case where placement fails after many attempts
            // e.g., log an error, or maybe even reset and retry all computer placements.
            // For now, we'll just proceed, but the board might be invalid.
            #ifdef DEBUG_MODE // Example conditional logging 
            // std::cerr << "Warning: Failed to place computer ship: " << player2.fleet[i].name << std::endl; // Commented out std::cerr/endl
            #endif
        }
    }
}

// --- Interface Functions ---

// Attempts to place a single player ship based on input from JS.
// Returns true if placement is valid and successful, false otherwise.
bool placePlayerShip(int shipIndex, int row, int col, std::string orientationInput) {
    printf("placePlayerShip called: ship=%d, row=%d, col=%d, orient='%s'\n", shipIndex, row, col, orientationInput.c_str());

    // Validate shipIndex
    if (shipIndex < 0 || shipIndex >= FLEET_SIZE) {
        printf("placePlayerShip failed: Invalid shipIndex %d\n", shipIndex);
        return false; 
    }

    // Extract and validate orientation from string
    if (orientationInput.empty()) {
        printf("placePlayerShip failed: Orientation string is empty\n");
        return false;
    }
    char orientation = toupper(orientationInput[0]); // Get first char and convert to upper

    Ship& ship = player1.fleet[shipIndex];
    printf("  -> Processing ship '%s' (size %d), orientation %c\n", ship.name.c_str(), ship.size, orientation);

    // --- Validation Logic (adapted from getValidShipInfo) --- 

    // Validate row and column bounds
    if (row < 0 || row >= 10 || col < 0 || col >= 10) {
        printf("placePlayerShip failed: Coordinates (%d, %d) out of bounds\n", row, col);
        return false;
    }

    // Validate orientation
    if (orientation != 'H' && orientation != 'V') {
        printf("placePlayerShip failed: Invalid orientation '%c'\n", orientation);
        return false;
    }

    // Check if ship fits on board
    if (orientation == 'H' && col + ship.size > 10) {
        printf("placePlayerShip failed: Ship does not fit horizontally at col %d\n", col);
        return false;
    }
    if (orientation == 'V' && row + ship.size > 10) {
        printf("placePlayerShip failed: Ship does not fit vertically at row %d\n", row);
        return false;
    }

    // Check if space is occupied
    if (spaceOccupied(player1, row, col, orientation, ship.size)) {
        printf("placePlayerShip failed: Space is occupied at (%d, %d) orient %c\n", row, col, orientation);
        // Optional: Add more detailed logging inside spaceOccupied if needed
        return false;
    }

    // --- Placement Logic (adapted from original placeShip) --- 
    printf("placePlayerShip: All validation passed. Placing ship.\n");

    // Clear previous placement if any
    ship.occupiedCells.clear(); 
    // Note: We might need to clear the board symbols ('s') if allowing re-placement during setup.
    // For now, assume this is the first placement for this ship.

    // Mark ship on board
    for (int i = 0; i < ship.size; i++) {
        if (orientation == 'H') {
            player1.board[row][col + i] = 's';
            ship.occupiedCells.push_back(Point(row, col + i));
        } else {
            player1.board[row + i][col] = 's';
            ship.occupiedCells.push_back(Point(row + i, col));
        }
    }
    printf("placePlayerShip successful.\n");
    return true; // Placement successful
}

// Returns the current state of both boards as a JSON string.
// Hides un-hit computer ships. Uses manual string building.
std::string getBoardState() {
    std::ostringstream oss;
    oss << "{";

    // Player 1 board
    oss << "\"player1\":[";
    for (int r = 0; r < 10; ++r) {
        oss << "[";
        for (int c = 0; c < 10; ++c) {
            // Represent char as a JSON string
            oss << "\"" << player1.board[r][c] << "\""; 
            if (c < 9) oss << ",";
        }
        oss << "]";
        if (r < 9) oss << ",";
    }
    oss << "],"; // End of player1 array

    // Player 2 board (masked)
    oss << "\"player2_masked\":[";
    for (int r = 0; r < 10; ++r) {
        oss << "[";
        for (int c = 0; c < 10; ++c) {
            char cell = player2.board[r][c];
            if (cell == 'H' || cell == 'M') {
                oss << "\"" << cell << "\""; // Show Hit or Miss
            } else {
                oss << "null"; // Use JSON null for hidden/empty
            }
            if (c < 9) oss << ",";
        }
        oss << "]";
        if (r < 9) oss << ",";
    }
    oss << "]"; // End of player2_masked array

    oss << "}"; // End of JSON object
    return oss.str();
}

// Returns the current game status as a JSON string.
std::string getGameStatus() {
    nlohmann::json status;
    status["currentPlayer"] = currentPlayer; // 1 for Player, 2 for Computer
    status["gameOver"] = gameOver;
    
    if (gameOver) {
        // Determine winner by checking who's turn it WOULD be if game continued
        // (or more robustly, check who has ships left)
        if (checkWinCondition(player2)) {
            status["winner"] = 1; // Player 1 won
        } else if (checkWinCondition(player1)) {
            status["winner"] = 2; // Player 2 won
        } else {
             status["winner"] = 0; // Should not happen in a normal win scenario
        }
    } else {
        status["winner"] = 0; // Game not over, no winner yet
    }

    return status.dump();
}

#include <emscripten/bind.h> // Moved here, just before bindings
EMSCRIPTEN_BINDINGS(my_module) {
    emscripten::function("initializeGame", emscripten::select_overload<void()>(&initializeGame)); 
    emscripten::function("placePlayerShip", emscripten::select_overload<bool(int, int, int, std::string)>(&placePlayerShip));
    // function("finalizePlayerPlacement", &finalizePlayerPlacement); // Still needs implementation
    emscripten::function("placeComputerShips", emscripten::select_overload<void()>(&placeComputerShips)); // Bind this too
    emscripten::function("makePlayerAttack", emscripten::select_overload<std::string(int, int)>(&makePlayerAttack)); 
    emscripten::function("makeComputerAttack", emscripten::select_overload<std::string()>(&makeComputerAttack)); 
    emscripten::function("getBoardState", emscripten::select_overload<std::string()>(&getBoardState)); 
    emscripten::function("getGameStatus", emscripten::select_overload<std::string()>(&getGameStatus)); 
    
    emscripten::value_object<Point>("Point")
        .field("row", &Point::row)
        .field("col", &Point::col);
}