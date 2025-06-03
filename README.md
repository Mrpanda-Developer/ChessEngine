# Chess Game with AI - Readme

## Description
A simple chess game implemented in C++ featuring an AI opponent using the minimax algorithm with an evaluation function. The game supports all basic chess rules, including special moves like castling, en passant, and pawn promotion.

## Features
- Fully functional chess board with all pieces
- Support for special moves:
  - Castling (both kingside and queenside)
  - En passant
  - Pawn promotion (automatically to queen)
- AI opponent using minimax algorithm
- Evaluation function based on piece values
- Best move suggestions during gameplay
- Clear ASCII board display

## Requirements
- C++ compiler (e.g., g++)
- Terminal/command line to run the program

## Installation & Usage
1. Copy the code to a file named `chess-engine.cpp`
2. Compile with:  
   `g++ chess-engine -o chess`
3. Run the program:  
   `./chess`
4. Choose to play as white or black
5. Enter moves in format `e2e4` (from e2 to e4)

## Game Controls
- Each turn shows AI's suggested best move
- Enter moves by specifying start and end positions (e.g., `e2e4`)
- Game automatically alternates between player and AI
- Board displays after each move

## AI Functionality
- Uses minimax algorithm with depth 3
- Evaluation based on material values:
  - Pawn: 100 points
  - Knight: 320 points
  - Bishop: 330 points
  - Rook: 500 points
  - Queen: 900 points
  - King: 20000 points

## Limitations
- No check/checkmate detection
- No stalemate detection
- Pawn always promotes to queen
- Basic evaluation function without positional awareness
- No optimizations like alpha-beta pruning

## Future Improvements
- Implement alpha-beta pruning for better AI performance
- Advanced evaluation function with positional awareness
- Check, checkmate and stalemate detection
- GUI instead of terminal interface

## Author
This program was written as an educational example for chess AI implementations in C++.
