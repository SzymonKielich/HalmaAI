#ifndef LISTA2_AI_KIELICH_HEURISTIC_H
#define LISTA2_AI_KIELICH_HEURISTIC_H

#include <random>
#include <ostream>
#include "Game.h"
#include "cmath"

// Positive heuristic values are good for player1, negative for player2
class Heuristic {
public:
    virtual double evaluate(Game& game) = 0;
    virtual std::string toString() = 0;
    // Scale value to [-1, 1] range
    static double scaleValues(double value, double min, double max) {
        return (value - min) / (max - min) * 2 - 1;
    }




};

class ClosestToOpponentCampEuclideanDistanceHeuristic : public Heuristic {
public:
    double evaluate(Game& game) override {
        double player1score = 0;
        double player2score = 0;
        int bs = Game::BOARD_SIZE - 1;
        for (int i = 0; i <= bs; i++) {
            for (int j = 0; j <= bs; j++) {
                if (game.board[i][j] == Piece::PLAYER1) {
                    player1score += sqrt(pow(bs - i, 2) + pow(bs - j, 2));
                } else if (game.board[i][j] == Piece::PLAYER2) {
                    player2score += sqrt(pow(i, 2) + pow(j, 2));
                }
            }
        }
        return scaleValues(player2score - player1score, -400, 400);
    }

    std::string toString() override{
        return "closestToEnemyBase";
    }
};

class MostMovesHeuristic : public Heuristic {
public:
    double evaluate(Game& game) override {
        unsigned int player1score = game.allPossibleMoves(Piece::PLAYER1).size();
        unsigned int player2score = game.allPossibleMoves(Piece::PLAYER2).size();
        return scaleValues(player1score - player2score, -500, 500);
    }

    std::string toString() override {
        return "mostMoves";
    }
};

// The closer the player's pawns are to each other, the better
class PawnClusteringHeuristic : public Heuristic {
public:
    double evaluate(Game& game) override {
        double player1score = 0;
        double player2score = 0;
        std::vector<Position> player1pawnsPositions;
        player1pawnsPositions.reserve(Game::TOTAL_PLAYER_PIECES);
        std::vector<Position> player2pawnsPositions;
        player2pawnsPositions.reserve(Game::TOTAL_PLAYER_PIECES);
        int bs = Game::BOARD_SIZE - 1;
        for (int i = 0; i <= bs; i++) {
            for (int j = 0; j <= bs; j++) {
                if (game.board[i][j] == Piece::PLAYER1) player1pawnsPositions.push_back({i, j});
                else if (game.board[i][j] == Piece::PLAYER2) player2pawnsPositions.push_back({i, j});
            }
        }

        for (int i = 0; i < player1pawnsPositions.size(); i++) {
            for (int j = i + 1; j < player1pawnsPositions.size(); j++) {
                player1score += sqrt(pow(player1pawnsPositions[i].x - player1pawnsPositions[j].x, 2) +
                                     pow(player1pawnsPositions[i].y - player1pawnsPositions[j].y, 2));
            }
        }
        for (int i = 0; i < player2pawnsPositions.size(); i++) {
            for (int j = i + 1; j < player2pawnsPositions.size(); j++) {
                player2score += sqrt(pow(player2pawnsPositions[i].x - player2pawnsPositions[j].x, 2) +
                                     pow(player2pawnsPositions[i].y - player2pawnsPositions[j].y, 2));
            }
        }
        return scaleValues(player2score - player1score, -400, 400);
    }
    std::string toString() override {
        return "pawnCluster";
    }
};

#endif //LISTA2_AI_KIELICH_HEURISTIC_H
