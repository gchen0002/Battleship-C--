# Battleship C++

A simple command-line implementation of the classic Battleship game in C++.

This program implements a classic Battleship game where two players, one human and one computer, engage in a turn-based battle on a 10x10 grid. The objective is to strategically place ships and successfully attack the opponent's ships until one player has no ships remaining.

## Prerequisites

- A C++ compiler that supports C++11 or later (e.g., g++, clang++).
- Git (optional, for version control).

## Building

To compile the project, navigate to the project directory in your terminal and run:

```bash
g++ main.cpp functions.cpp -o battleship.exe
```

*(Note: If `g++` is not in your PATH, you might need to provide the full path to the executable, e.g., `C:\msys64\ucrt64\bin\g++.exe`)*

## Running

After successful compilation, run the game from the project directory:

```bash
./battleship.exe
```

Or on Windows Command Prompt/PowerShell:

```powershell
.\battleship.exe
```

## Input File

The game expects an `input.txt` file in the same directory for Player 1's ship placements. See the existing `input.txt` for the format. 