// script.js

// Wait for the Emscripten module to be ready
createBattleshipModule().then(Module => {
    console.log("Battleship WASM Module Loaded");
    window.BattleshipModule = Module; // Make Module globally accessible (or handle scope differently)

    // --- DOM Element References ---
    const playerBoardDiv = document.getElementById('player-board');
    const computerBoardDiv = document.getElementById('computer-board');
    const gameStatusP = document.getElementById('game-status');
    const messageP = document.getElementById('message'); // For short-lived messages
    const messageLogDiv = document.getElementById('message-log'); // New log element
    const newGameBtn = document.getElementById('new-game-btn');
    const placementControlsDiv = document.getElementById('placement-controls');
    const shipSelect = document.getElementById('ship-select');
    const orientationSelect = document.getElementById('orientation-select');
    const finalizePlacementBtn = document.getElementById('finalize-placement-btn');

    // --- Game State (JavaScript side) ---
    let gameState = 'loading'; // loading, placement, player_turn, computer_turn, game_over
    let playerShipsPlaced = [false, false, false, false, false]; // Track which player ships are placed
    const shipData = [
        { name: "Carrier", size: 5 },
        { name: "Battleship", size: 4 },
        { name: "Cruiser", size: 3 },
        { name: "Submarine", size: 3 },
        { name: "Destroyer", size: 2 }
    ];

    // --- Initialization ---
    initializeUI();
    setupEventListeners();
    updateStatus("WASM Module Loaded. Click 'New Game' to start.");

    // --- Logging Function ---
    function logMessage(text) {
        if (!messageLogDiv) return;
        const p = document.createElement('p');
        p.textContent = text;
        messageLogDiv.appendChild(p);
        // Auto-scroll to bottom
        messageLogDiv.scrollTop = messageLogDiv.scrollHeight;
        // Also briefly show in single message area for immediate feedback
        // messageP.textContent = text;
    }

    // --- Core Functions ---

    function initializeUI() {
        createGrid(playerBoardDiv, false); // Player board non-clickable initially
        createGrid(computerBoardDiv, true); // Computer board clickable
        populateShipSelect();
        placementControlsDiv.style.display = 'none';
    }

    function createGrid(gridDiv, isClickable) {
        gridDiv.innerHTML = ''; // Clear existing grid
        for (let r = 0; r < 10; r++) {
            for (let c = 0; c < 10; c++) {
                const cell = document.createElement('div');
                cell.classList.add('cell');
                cell.dataset.row = r;
                cell.dataset.col = c;
                if (isClickable) {
                     // Listener added later based on game state
                     // cell.addEventListener('click', handleCellClick);
                     cell.style.cursor = 'pointer';
                } else {
                    cell.style.cursor = 'default';
                }
                gridDiv.appendChild(cell);
            }
        }
    }

    function populateShipSelect() {
        shipSelect.innerHTML = '';
        shipData.forEach((ship, index) => {
            const option = document.createElement('option');
            option.value = index;
            option.textContent = `${ship.name} (${ship.size})`;
            option.disabled = playerShipsPlaced[index]; // Disable if already placed
            shipSelect.appendChild(option);
        });
    }

    function setupEventListeners() {
        newGameBtn.addEventListener('click', startGame);
        finalizePlacementBtn.addEventListener('click', finalizePlacement);
        // Add listeners for player board clicks during placement phase (in startGame)
        // Add listeners for computer board clicks during attack phase (in finalizePlacement or updateStatus)
    }

    function startGame() {
        console.log("Starting new game...");
        // Clear previous log
        if (messageLogDiv) messageLogDiv.innerHTML = ''; 
        logMessage("Starting new game..."); // Log it
        updateStatus("Initializing game...");
        // Call C++ initializeGame directly
        BattleshipModule.initializeGame();

        playerShipsPlaced = [false, false, false, false, false];
        populateShipSelect();
        gameState = 'placement';
        placementControlsDiv.style.display = 'block'; // Show placement controls
        finalizePlacementBtn.disabled = true; // Disabled until all ships are placed
        updateStatus("Place your ships. Select ship and click on your board.");
        messageP.textContent = ''; // Clear temporary message area
        addPlacementListeners(); // Enable clicks on player board for placement
        clearBoardHighlights(); // Clear highlights from previous game
        refreshBoardState(); // Get initial empty board state
    }

     function addPlacementListeners() {
        playerBoardDiv.querySelectorAll('.cell').forEach(cell => {
            cell.style.cursor = 'pointer';
            // Remove old listener if exists
            cell.removeEventListener('click', handlePlacementClick);
            // Add new listener
            cell.addEventListener('click', handlePlacementClick);
        });
         // Disable clicking computer board during placement
         computerBoardDiv.querySelectorAll('.cell').forEach(cell => {
             cell.style.cursor = 'default';
             cell.removeEventListener('click', handleCellClick);
         });
    }
     function removePlacementListeners() {
         playerBoardDiv.querySelectorAll('.cell').forEach(cell => {
            cell.style.cursor = 'default';
            cell.removeEventListener('click', handlePlacementClick);
         });
         // Re-enable clicking computer board for attack phase
         computerBoardDiv.querySelectorAll('.cell').forEach(cell => {
             cell.style.cursor = 'pointer';
             // Remove old attack listener if exists
             cell.removeEventListener('click', handleCellClick); 
             // Add new attack listener
             cell.addEventListener('click', handleCellClick); 
         });
    }


    function handlePlacementClick(event) {
        if (gameState !== 'placement') return;

        const cell = event.target;
        const row = parseInt(cell.dataset.row);
        const col = parseInt(cell.dataset.col);
        const shipIndex = parseInt(shipSelect.value);
        const orientation = orientationSelect.value; // 'H' or 'V'

        if (isNaN(shipIndex)) {
            messageP.textContent = "Please select a ship type."; // Keep this short-lived
            return;
        }
         if (playerShipsPlaced[shipIndex]) {
             messageP.textContent = "You have already placed the " + shipData[shipIndex].name; // Keep this short-lived
             return;
        }

        console.log(`Attempting to place ship ${shipIndex} at (${row}, ${col}), orientation: ${orientation}`);

        // Call C++ placePlayerShip directly
        const success = BattleshipModule.placePlayerShip(shipIndex, row, col, orientation);

        if (success) {
            // messageP.textContent = `${shipData[shipIndex].name} placed successfully.`;
            logMessage(`${shipData[shipIndex].name} placed successfully.`); // Log it
            playerShipsPlaced[shipIndex] = true;
            populateShipSelect(); // Update dropdown to disable placed ship
            refreshBoardState(); // Update the visual board
             // Check if all ships are placed
             if (playerShipsPlaced.every(placed => placed)) {
                 // messageP.textContent = "All ships placed. Click 'Finalize Placement'.";
                 logMessage("All ships placed. Click 'Finalize Placement'."); // Log it
                 finalizePlacementBtn.disabled = false; // Enable finalize button
             } else {
                 // Find the next available ship to select
                 const nextShipIndex = playerShipsPlaced.findIndex(placed => !placed);
                 if (nextShipIndex !== -1) {
                     shipSelect.value = nextShipIndex;
                 }
             }
        } else {
            messageP.textContent = `Invalid placement for ${shipData[shipIndex].name}. Try again.`; // Keep this short-lived
        }
    }

     function finalizePlacement() {
         if (gameState !== 'placement' || !playerShipsPlaced.every(placed => placed)) {
             messageP.textContent = "Please place all your ships first.";
             return;
         }

         console.log("Finalizing placement... Placing computer ships.");
         // Call C++ to place computer ships directly
         logMessage("Player placement complete. Placing computer ships..."); // Log it
         BattleshipModule.placeComputerShips();
         logMessage("Computer ships placed."); // Log it

         gameState = 'player_turn';
         placementControlsDiv.style.display = 'none';
         removePlacementListeners(); // Disable placement clicks, enable attack clicks
         updateStatus("Your turn. Click on the enemy board to attack.");
         messageP.textContent = '';
         refreshBoardState(); // Show initial state for attack phase
     }


    function handleCellClick(event) {
        if (gameState !== 'player_turn') {
            // Prevent clicks if not player's turn or game over
            let currentStatus = "";
            if (gameState === 'placement') currentStatus = "Place your ships first.";
            else if (gameState === 'computer_turn') currentStatus = "Wait for the computer's turn.";
            else if (gameState === 'game_over') currentStatus = "The game is over.";
            else currentStatus = "It's not your turn.";
            messageP.textContent = currentStatus;
            return;
        }

        const cell = event.target;
        const row = parseInt(cell.dataset.row);
        const col = parseInt(cell.dataset.col);

        console.log(`Player attacks (${row}, ${col})`);
        updateStatus("Processing your attack...");
        logMessage(`Player attacks (${row + 1}, ${String.fromCharCode(65 + col)})...`); // Log attack coordinates

        // Call C++ makePlayerAttack directly
        const result = BattleshipModule.makePlayerAttack(row, col);

        console.log("Player attack result:", result);
        processAttackResult(result, 'Player 1', cell); // Process result and update UI

        // If the game didn't end and it's now computer's turn
        if (gameState === 'computer_turn') {
             // Add a small delay for visual feedback before computer moves
             setTimeout(handleComputerTurn, 600); // Increased delay slightly
        }
    }

    function handleComputerTurn() {
        if (gameState !== 'computer_turn') return;

        console.log("Computer's turn...");
        updateStatus("Computer is thinking...");
        logMessage("Computer's turn..."); // Log it

        // Call C++ makeComputerAttack directly
        const result = BattleshipModule.makeComputerAttack();

        console.log("Computer attack result:", result);

        // Find the target cell based on the result string (e.g., "hit A5", "miss B10")
        const resultParts = result.split(' ');
        const coordStr = resultParts[resultParts.length - 1]; // Last part is coordinate
        const targetCell = findCellByCoordString(coordStr); // Helper function needed

        processAttackResult(result, 'Computer', targetCell); // Process result and update UI
    }

    // Helper function to find a cell element from coordinate string like "A1", "J10"
    function findCellByCoordString(coordStr) {
        if (!coordStr || coordStr.length < 2) return null;
        const rowChar = coordStr.charAt(0).toUpperCase();
        const colStr = coordStr.substring(1);
        const row = rowChar.charCodeAt(0) - 'A'.charCodeAt(0);
        const col = parseInt(colStr) - 1;

        if (row < 0 || row >= 10 || isNaN(col) || col < 0 || col >= 10) return null;

        // Find the cell in the player's grid (computer attacks player)
        return playerBoardDiv.querySelector(`.cell[data-row="${row}"][data-col="${col}"]`);
    }

    function processAttackResult(result, attacker, targetCell) {
        let message = "";
        let gameOver = false;
        let winner = null; // 1 for Player 1, 2 for Computer

        // Parse the result string (e.g., "hit", "miss", "sunk Carrier", "win Player 1", "hit A5", "miss J10" etc.)
        const resultParts = result.split(' ');
        const type = resultParts[0]; // hit, miss, sunk, win, invalid_*


        // Update cell appearance based on hit/miss ONLY if targetCell is valid
        if (targetCell) {
            targetCell.classList.remove('highlight-potential'); // Remove potential highlight
             if (type === 'hit' || type === 'sunk' || (type === 'win' && attacker === 'Computer') ) { // Mark computer's winning shot too
                targetCell.classList.add('hit');
                targetCell.textContent = 'X';
            } else if (type === 'miss') {
                targetCell.classList.add('miss');
                targetCell.textContent = '•';
            }
            // Make attacked cell non-clickable again
             targetCell.style.cursor = 'default';
             targetCell.removeEventListener('click', handleCellClick); // Specific to computer board clicks
             targetCell.removeEventListener('click', handlePlacementClick); // Specific to player board clicks
        } else if (attacker === 'Computer' && (type === 'hit' || type === 'miss' || type === 'sunk' || type === 'win')) {
             console.error("Error: Could not find target cell for computer attack:", resultParts[resultParts.length - 1]);
        }


        // Construct message and update game state
        switch (type) {
            case 'hit':
                message = `${attacker} scored a hit!`;
                break;
            case 'miss':
                message = `${attacker} missed.`;
                break;
            case 'sunk':
                const shipName = resultParts[1];
                message = `${attacker} hit and sunk the ${shipName}!`;
                break;
            case 'win':
                const winnerName = resultParts[1]; // "Player" or "Computer"
                winner = (winnerName === 'Player') ? 1 : 2;
                message = `Game Over! ${winnerName} wins!`;
                gameOver = true;
                break;
            case 'invalid_already_attacked':
                 message = "You already attacked that location.";
                 break; // Don't switch turn
             case 'invalid_bounds':
                 // Log invalid player moves, but maybe not show in primary message?
                 if (attacker === 'Player 1') {
                     logMessage("Invalid coordinates selected.");
                 } else {
                     // Should not happen for computer, indicates C++ error
                     logMessage(`Error: Computer attacked invalid bounds: ${result}`);
                 }
                 message = "Invalid coordinates.";
                 break; // Don't switch turn
             case 'invalid_turn':
                 logMessage("Error: Attack attempted during wrong turn."); // Log error
                 message = "It's not your turn!"; // Should ideally not happen with UI controls
                 break; // Don't switch turn
             case 'invalid_game_over':
                 logMessage("Error: Attack attempted after game over."); // Log error
                  message = "The game is already over.";
                   break; // Don't switch turn
            default:
                message = `Unknown result: ${result}`;
                 console.warn("Received unknown result from C++:", result);
                break;
        }

        // Log the primary outcome message
        logMessage(message); 
        // Clear the temporary message area if it wasn't an error kept there
        if (type !== 'invalid_already_attacked' && type !== 'invalid_bounds') {
             messageP.textContent = ''; 
        } else {
            messageP.textContent = message; // Keep invalid move message visible briefly
        }

        // Check game status from C++ if the game *might* be over
        if (type === 'hit' || type === 'sunk' || type === 'win') {
             // Call C++ getGameStatus directly
             const statusJson = BattleshipModule.getGameStatus();
             const status = JSON.parse(statusJson);
             if (status.gameOver) {
                gameOver = true;
                if (!winner) winner = status.winner; // If win wasn't direct result string
                if (winner && !message.includes("Game Over")) { // Update message if win detected via status
                     const winnerText = `Game Over! ${winner === 1 ? 'Player 1' : 'Computer'} wins!`;
                     logMessage(winnerText); // Log it
                }
             }
        }


        // Update overall game state
        if (gameOver) {
            gameState = 'game_over';
            updateStatus(`Game Over! ${winner === 1 ? 'Player 1' : 'Computer'} wins!`);
            // Disable further interaction
            removePlacementListeners(); // Ensure all board listeners removed
        } else {
            // Switch turns if it was a valid move
             if (!type.startsWith('invalid_')) {
                 gameState = (attacker === 'Player 1') ? 'computer_turn' : 'player_turn';
                 updateStatus(gameState === 'player_turn' ? "Your turn." : "Computer's turn.");
             } else {
                  // If invalid move, revert status message if needed
                  updateStatus(gameState === 'player_turn' ? "Your turn." : "Computer's turn.");
             }
        }

        // Refresh boards (especially computer's hidden ships might be revealed on hits)
        // This might be redundant if processAttackResult updates the cell directly,
        // but refreshing ensures consistency with C++ state.
         if (!gameOver) refreshBoardState();

    }


    function updateStatus(text) {
        gameStatusP.textContent = text;
    }

    function refreshBoardState() {
        if (!window.BattleshipModule) return; // Ensure module is loaded

        // Call C++ getBoardState directly
        const stateJson = BattleshipModule.getBoardState();
        try {
            const state = JSON.parse(stateJson);
            renderBoard(playerBoardDiv, state.player1);
            renderBoard(computerBoardDiv, state.player2_masked); // Render masked computer board
        } catch (e) {
            console.error("Failed to parse board state:", e);
            console.error("Received JSON string:", stateJson);
            updateStatus("Error retrieving board state.");
        }
    }

     function renderBoard(gridDiv, boardData) {
        if (!boardData) return;
        const cells = gridDiv.querySelectorAll('.cell');
        cells.forEach(cell => {
            const r = parseInt(cell.dataset.row);
            const c = parseInt(cell.dataset.col);
            // Ensure we handle potential variations in data (string vs char)
            const cellState = String(boardData[r][c]).trim(); 

            // Reset classes first
            cell.classList.remove('ship', 'hit', 'miss');
            cell.textContent = ''; // Clear text content

            // Apply new classes based on state
            switch(cellState) {
                case 's': // Player's own ship (only show on player board)
                    if (gridDiv.id === 'player-board') {
                         cell.classList.add('ship');
                         // Optionally add text: cell.textContent = 'S';
                    } else {
                         // Don't show 's' on computer board (it's masked)
                    }
                    break;
                case 'H':
                    cell.classList.add('hit');
                    cell.textContent = '🔥'; // Or 'H'
                    break;
                case 'M':
                    cell.classList.add('miss');
                     cell.textContent = '●'; // Or 'M'
                    break;
                case '': // Empty space
                default:
                    // No specific styling needed for empty water
                    break;
            }
        });
    }

     function clearBoardHighlights() {
         document.querySelectorAll('.grid .cell').forEach(cell => {
             cell.classList.remove('ship', 'hit', 'miss');
             cell.textContent = '';
         });
     }


    // --- Error Handling ---
    Module.onAbort = (reason) => {
        console.error("WASM Aborted:", reason);
        updateStatus("Fatal WASM Error. Please reload.");
        gameState = 'game_over';
    };

}).catch(err => {
    console.error("Error loading Battleship WASM Module:", err);
    document.getElementById('game-status').textContent = 'Error loading WASM. Check console.';
}); 