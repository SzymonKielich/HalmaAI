# Halma Game AI

This project implements an AI for the game of Halma using the **Minimax algorithm** with **Alpha-Beta pruning**. The AI plays on a 16x16 board, aiming to move all pieces to the opponent's starting camp. It supports human vs AI and AI vs AI modes, with adaptive strategies based on game phases.

## Features

- **Minimax Algorithm**: Evaluates possible moves to choose the best one.
- **Alpha-Beta Pruning**: Optimizes Minimax by reducing the number of nodes evaluated.
- **Heuristics**:
  - **Closest to Opponent Camp**: Encourages pieces to move toward the opponent's camp.
  - **Most Moves**: Favors positions with more possible moves.
  - **Pawn Clustering**: Encourages pieces to stay close together.
- **Adaptive Strategies**: AI adjusts strategies based on the game phase (early, mid, late).
- **Human vs AI Mode**: Play against the AI in the console.
- **Tournament Mode**: Run AI vs AI matches to test strategies.
