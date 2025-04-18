#include "header.h"
/**********************************************************
 * This program implements a classic Battleship game where
 * two players, one human and one computer, engage in a turn-based
 * battle on a 10x10 grid. The objective is to strategically
 * place ships and successfully attack the opponent's ships
 * until one player has no ships remaining.
 *
 * INPUT:
 *   - Player 1's ship placements are read from an input file
 *     named "input.txt", which specifies the coordinates and
 *     orientations for each ship.
 *   - During gameplay, the human player inputs attack coordinates
 *     in the format (e.g., A5) to target the opponent's ships.
 *
 * OUTPUT:
 *   - The program displays both players' boards after each turn,
 *     showing the current state of the game, including hits,
 *     misses, and sunk ships.
 *   - It announces the results of each attack and declares the
 *     winner when all ships of one player are sunk.
 *
 * PROCESSING:
 *   - Initializes the fleets for both players with predefined
 *     ship sizes and types.
 *   - Sets up the game board by placing ships for Player 1
 *     based on the input file and randomly for the computer.
 *   - Alternates turns between the human player and the computer,
 *     validating user input for attack coordinates and updating
 *     the game state accordingly.
 *   - Continuously checks for win conditions after each turn,
 *     ensuring a smooth gameplay experience until a winner is
 *     determined.
 **********************************************************/
int main() {
    // Seed random number generator
    srand(time(nullptr));

    // Welcome message
    cout << "Welcome to Battleship!\n";
    
    // Start the game
    playGame();

    return 0;
}