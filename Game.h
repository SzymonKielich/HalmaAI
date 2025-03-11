#ifndef LISTA2_AI_KIELICH_GAME_H
#define LISTA2_AI_KIELICH_GAME_H

#include <vector>

class Heuristic;

struct Position {
    int x, y;

    bool operator==(const Position& ) const = default;

    Position add(int dx, int dy) const {
        return {x + dx, y + dy};
    }
};

struct Move {
    Position from, to;
};

enum class Piece : int {
    EMPTY = 0,
    PLAYER1 = 1,
    PLAYER2 = 2
};

class Game {
public:
    static constexpr int TOTAL_PLAYER_PIECES = 19;
    static constexpr int BOARD_SIZE = 16;
    Piece board[BOARD_SIZE][BOARD_SIZE]{};
    int maxDepth = 4;

    static constexpr Position PLAYER1_CAMP[] = {{0, 0},
                                                {0, 1},
                                                {0, 2},
                                                {0, 3},
                                                {0, 4},
                                                {1, 0},
                                                {1, 1},
                                                {1, 2},
                                                {1, 3},
                                                {1, 4},
                                                {2, 0},
                                                {2, 1},
                                                {2, 2},
                                                {2, 3},
                                                {3, 0},
                                                {3, 1},
                                                {3, 2},
                                                {4, 0},
                                                {4, 1}};

    static constexpr Position PLAYER2_CAMP[] = {{15, 15},
                                                {15, 14},
                                                {15, 13},
                                                {15, 12},
                                                {15, 11},
                                                {14, 15},
                                                {14, 14},
                                                {14, 13},
                                                {14, 12},
                                                {14, 11},
                                                {13, 15},
                                                {13, 14},
                                                {13, 13},
                                                {13, 12},
                                                {12, 15},
                                                {12, 14},
                                                {12, 13},
                                                {11, 15},
                                                {11, 14}};

    Game();

    Game(const Game& other);

//    Game(char inputBoard[BOARD_SIZE][BOARD_SIZE]);
    Game(const std::string& filename);

    void printBoard();

    void makeBestMoveAlphaBeta();

    void printBestMove() const;

    void printCurrentTurnPlayer();

    bool isGameOver();

    std::vector<Move> allPossibleMoves(Piece player);

    std::vector<Move> possibleJumpMovesFromPos(Position& pos, Piece player);

    void setPlayerStrategy(std::vector<Heuristic *>& playerStrategy, Piece player);

    void setPlayerWeights(std::vector<double>& playerWeights, Piece player);
    Piece currentTurnPlayer = Piece::PLAYER1;
    void makeMove(Move& move);

    double minimaxAlphaBeta(int depth, bool isMaximizing, Piece player, double alpha, double beta);

    double minimax(int depth, bool isMaximizing, Piece player);

    void makeBestMoveMiniMax();

    Move bestMove;
private:

    std::vector<Heuristic *> player1Strategy;
    std::vector<double> player1Weights;
    std::vector<Heuristic *> player2Strategy;
    std::vector<double> player2Weights;

    static constexpr Position moveDirections[] = {{-1, -1},
                                                  {-1, 0},
                                                  {-1, 1},
                                                  {0,  -1},
                                                  {0,  1},
                                                  {1,  -1},
                                                  {1,  0},
                                                  {1,  1}};

    bool isPosInsideEnemyCamp(Position& pos, Piece player);

    bool isPosInsideBoard(Position& pos);

    bool isPosEmpty(Position& pos);

    std::vector<Move> possibleAdjacentMovesFromPos(Position& pos, Piece player);

    bool isPosVisited(Position& pos, std::vector<Position>& visited);


    void revertMove(Move& move);

};

#endif //LISTA2_AI_KIELICH_GAME_H
