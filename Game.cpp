#include <iostream>
#include <algorithm>
#include <queue>
#include "Game.h"
#include "Heuristic.h"
#include <fstream>
#include <sstream>
#include "pprint.h"
Game::Game() {

    for (const Position& campPos: PLAYER1_CAMP) {
        board[campPos.x][campPos.y] = Piece::PLAYER1;
    }

    for (const Position& campPos: PLAYER2_CAMP) {
        board[campPos.x][campPos.y] = Piece::PLAYER2;
    }
}

Game::Game(const Game& other) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            board[i][j] = other.board[i][j];
        }
    }
}

//Game::Game(char inputBoard[BOARD_SIZE][BOARD_SIZE]) {
//    for (int i = 0; i < BOARD_SIZE; i++) {
//        for (int j = 0; j < BOARD_SIZE; j++) {
//            switch (inputBoard[i][j]) {
//                case '-':
//                    board[i][j] = Piece::EMPTY;
//                    break;
//                case '1':
//                    board[i][j] = Piece::PLAYER1;
//                    break;
//                case '2':
//                    board[i][j] = Piece::PLAYER2;
//                    break;
//            }
//        }
//    }
//}
Game::Game(const std::string& filename) {
    // Initialize the game board
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            board[i][j] = Piece::EMPTY;
        }
    }

    std::ifstream file(filename);
    if (!file) {
        print(filename);
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    std::string line;
    int row = 0;
    while (std::getline(file, line) && row < BOARD_SIZE) {
        std::stringstream lineStream(line);
        std::string cell;
        print(line);
        int col = 0;
        while (std::getline(lineStream, cell, ',') && col < BOARD_SIZE) {
            if (cell.size() == 1) {
                print("cell:",cell);
                print(cell[0]);
                switch (cell[0]) {
                    case '1':
                        board[row][col] = Piece::PLAYER1;
                        break;
                    case '2':
                        board[row][col] = Piece::PLAYER2;
                        break;
                    default:
                        break;
                }
            }
            ++col;
        }
        if (col != BOARD_SIZE) {
            print("col",col);
            std::cerr << "Unexpected number of columns in row " << row << std::endl;
        }
        ++row;
    }
    if (row != BOARD_SIZE) {
        print("row",row);
        std::cerr << "Unexpected number of rows in file" << std::endl;
    }

    file.close();
}

bool Game::isGameOver() {
    bool isPlayer1InEnemyCamp = std::ranges::all_of(PLAYER2_CAMP, [this](const Position& campPos) {
        return board[campPos.x][campPos.y] == Piece::PLAYER1;
    });
    if (isPlayer1InEnemyCamp) return true;
    bool isPlayer2InEnemyCamp = std::ranges::all_of(PLAYER1_CAMP, [this](const Position& campPos) {
        return board[campPos.x][campPos.y] == Piece::PLAYER2;
    });
    return isPlayer2InEnemyCamp;
}

void Game::printBoard() {
    for (auto& i: board) {
        for (Piece& j: i) {
            switch (j) {
                case Piece::EMPTY:
                    std::cout << "-";
                    break;
                case Piece::PLAYER1:
                    std::cout << "1";
                    break;
                case Piece::PLAYER2:
                    std::cout << "2";
                    break;
            }
            std::cout << " ";
        }
        std::cout << std::endl;
    }
}

void Game::makeMove(Move& move) {
    board[move.to.x][move.to.y] = board[move.from.x][move.from.y];
    board[move.from.x][move.from.y] = Piece::EMPTY;
}

void Game::revertMove(Move& move) {
    board[move.from.x][move.from.y] = board[move.to.x][move.to.y];
    board[move.to.x][move.to.y] = Piece::EMPTY;
}

bool Game::isPosInsideEnemyCamp(Position& pos, Piece player) {
    // Get enemy camp based on player
    if(player==Piece::PLAYER1) {
        return std::any_of(std::begin(PLAYER2_CAMP), std::end(PLAYER2_CAMP),
                           [pos](const Position& campPos) { return pos == campPos; });
    } else {
        return std::any_of(std::begin(PLAYER1_CAMP), std::end(PLAYER1_CAMP),
                           [pos](const Position& campPos) { return pos == campPos; });
    }
//    const auto& enemyCamp = player == Piece::PLAYER1 ? PLAYER2_CAMP : PLAYER1_CAMP;
}

bool Game::isPosInsideBoard(Position& pos) {
    return pos.x >= 0 && pos.x < BOARD_SIZE && pos.y >= 0&&  pos.y < BOARD_SIZE;
}

bool Game::isPosEmpty(Position& pos) {

    return board[pos.x][pos.y] == Piece::EMPTY;
}

std::vector<Move> Game::allPossibleMoves(Piece player) {
    std::vector<Move> allJumpMoves;
    allJumpMoves.reserve(250);
    std::vector<Move> allAdjacentMoves;
    allAdjacentMoves.reserve(250);

    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == player) {
                Position pos = {i, j};

                std::vector<Move> jmpMoves = possibleJumpMovesFromPos(pos, player);

                allJumpMoves.insert(allJumpMoves.end(), jmpMoves.begin(), jmpMoves.end());

                std::vector<Move> adjMoves = possibleAdjacentMovesFromPos(pos, player);
//                print("possibleadjacent");
//                for (const auto& d: adjMoves) {
//                    print("adj from",d.from.x, d.from.y, "to",d.to.x, d.to.y);
//                }
                allAdjacentMoves.insert(allAdjacentMoves.end(), adjMoves.begin(), adjMoves.end());
            }
        }
    }

    std::vector<Move> allMoves;

    allMoves.reserve(allJumpMoves.size()+allAdjacentMoves.size());
    allMoves.insert(allMoves.end(), allJumpMoves.begin(), allJumpMoves.end());
    allMoves.insert(allMoves.end(), allAdjacentMoves.begin(), allAdjacentMoves.end());

    return allMoves;
}

std::vector<Move> Game::possibleJumpMovesFromPos(Position& pos, Piece player) {
    std::queue<Position> queue({pos});
    std::vector<Position> visited;
    visited.reserve(50);

    while (!queue.empty()) {
        Position p = queue.front();
        queue.pop();

        bool insideEnemyCamp = isPosInsideEnemyCamp(p, player);

        for (const auto& d: moveDirections) {
            Position np = player == Piece::PLAYER1 ? p.add(-d.x, -d.y) : p.add(d.x, d.y);
            Position nnp = player == Piece::PLAYER1 ? p.add(-2 * d.x, -2 * d.y) : p.add(2 * d.x, 2 * d.y);
            if (isPosInsideBoard(nnp) && !isPosEmpty(np) && isPosEmpty(nnp) && !isPosVisited(nnp, visited)) {
                // Check if position is inside enemy camp and jump position is outside enemy camp - skip
                if (insideEnemyCamp && !isPosInsideEnemyCamp(nnp, player)) continue;
                visited.push_back(nnp);
                queue.push(nnp);
            }
        }
    }

    std::vector<Move> moves;
    moves.reserve(visited.size());
    // Transform visited positions to moves (move from initial position to visited position)
    std::transform(visited.begin(), visited.end(), std::back_inserter(moves),
                   [pos](const Position& v) { return Move{pos, v}; });
    return moves;
}

bool Game::isPosVisited(Position& pos, std::vector<Position>& visited) {
    return std::find(visited.begin(), visited.end(), pos) != visited.end();
}

std::vector<Move> Game::possibleAdjacentMovesFromPos(Position& pos, Piece player) {
    std::vector<Move> moves;
    bool insideEnemyCamp = isPosInsideEnemyCamp(pos, player);

    for (const auto& d: moveDirections) {
        Position np = player == Piece::PLAYER1 ? pos.add(-d.x, -d.y) : pos.add(d.x, d.y);
        if (isPosInsideBoard(np) && isPosEmpty(np)) {
            // Check if initial position is inside enemy camp and adjacent position is outside enemy camp - skip
            if (insideEnemyCamp && !isPosInsideEnemyCamp(np, player)) continue;
            Move move = {pos, np};
            moves.push_back(move);
        }
    }
    return moves;
}

void Game::makeBestMoveAlphaBeta() {
    bool isMaximizing = currentTurnPlayer == Piece::PLAYER1;
//    minimax(maxDepth, isMaximizing, currentTurnPlayer);
    minimaxAlphaBeta(maxDepth, isMaximizing, currentTurnPlayer, INT_MIN, INT_MAX);
    makeMove(bestMove);
    currentTurnPlayer = currentTurnPlayer == Piece::PLAYER1 ? Piece::PLAYER2 : Piece::PLAYER1;
}
void Game::makeBestMoveMiniMax() {
    bool isMaximizing = currentTurnPlayer == Piece::PLAYER1;
//    minimax(maxDepth, isMaximizing, currentTurnPlayer);
    minimax(maxDepth, isMaximizing, currentTurnPlayer);
    makeMove(bestMove);
    currentTurnPlayer = currentTurnPlayer == Piece::PLAYER1 ? Piece::PLAYER2 : Piece::PLAYER1;
}


double Game::minimax(int depth, bool isMaximizing, Piece player) {
    Piece opponent = player == Piece::PLAYER1 ? Piece::PLAYER2 : Piece::PLAYER1;

    if (depth == 0 || isGameOver()) {
        double eval = 0;
        std::vector<Heuristic *>& heuristics = currentTurnPlayer == Piece::PLAYER1 ? player1Strategy : player2Strategy;
        std::vector<double>& playerWeights = currentTurnPlayer == Piece::PLAYER1 ? player1Weights : player2Weights;
        for (int i = 0; i < heuristics.size(); i++){
            eval += heuristics[i]->evaluate(*this) * playerWeights[i];
        }
        return eval;
    }

    std::vector<Move> moves = allPossibleMoves(player);

    if (isMaximizing) {
        double maxEval = INT_MIN;
        for (Move& move: moves) {
            makeMove(move);
            double eval = minimax(depth - 1, false, opponent);
            revertMove(move);

            if (eval > maxEval) {
                maxEval = eval;
                if (depth == maxDepth) bestMove = move;
            }
        }
        return maxEval;
    } else {
        double minEval = INT_MAX;
        for (Move& move: moves) {
            makeMove(move);
            double eval = minimax(depth - 1, true, opponent);
            revertMove(move);

            if (eval < minEval) {
                minEval = eval;
                if (depth == maxDepth) bestMove = move;
            }
        }
        return minEval;
    }
}

double Game::minimaxAlphaBeta(int depth, bool isMaximizing, Piece player, double alpha, double beta) {
    Piece opponent = player == Piece::PLAYER1 ? Piece::PLAYER2 : Piece::PLAYER1;

    if (depth == 0 || isGameOver()) {
        double eval = 0;
        std::vector<Heuristic *>& heuristics = currentTurnPlayer == Piece::PLAYER1 ? player1Strategy : player2Strategy;
        std::vector<double>& playerWeights = currentTurnPlayer == Piece::PLAYER1 ? player1Weights : player2Weights;
        for (int i = 0; i < heuristics.size(); i++){
            eval += heuristics[i]->evaluate(*this) * playerWeights[i];
        }
        return eval;
    }

    std::vector<Move> moves = allPossibleMoves(player);

    if (isMaximizing) {
        double maxEval = INT_MIN;
        for (Move& move: moves) {
            makeMove(move);
            double eval = minimaxAlphaBeta(depth - 1, false, opponent, alpha, beta);
            revertMove(move);

            if (eval > maxEval) {
                maxEval = eval;
                if (depth == maxDepth) bestMove = move;
            }
            alpha = std::max(alpha, maxEval);
            if (maxEval >= beta) break;
        }
        return maxEval;
    } else {
        double minEval = INT_MAX;
        for (Move& move: moves) {
            makeMove(move);
            double eval = minimaxAlphaBeta(depth - 1, true, opponent, alpha, beta);
            revertMove(move);

            if (eval < minEval) {
                minEval = eval;
                if (depth == maxDepth) bestMove = move;
            }
            beta = std::min(beta, minEval);
            if (minEval <= alpha) break;
        }
        return minEval;
    }
}

void Game::printBestMove() const {
    std::cout << "Best move: from (" << bestMove.from.x << ", " << bestMove.from.y << ") to ("
              << bestMove.to.x << ", " << bestMove.to.y << ")" << std::endl;
}

void Game::printCurrentTurnPlayer() {
    std::cout << "Current turn player: " << (currentTurnPlayer == Piece::PLAYER1 ? "Player 1" : "Player 2")
              << std::endl;
}

void Game::setPlayerStrategy(std::vector<Heuristic *>& playerStrategy, Piece player) {
    if (player == Piece::PLAYER1) player1Strategy = playerStrategy;
    else player2Strategy = playerStrategy;
}

void Game::setPlayerWeights(std::vector<double>& playerWeights, Piece player) {
    if (player == Piece::PLAYER1) player1Weights = playerWeights;
    else player2Weights = playerWeights;
}
