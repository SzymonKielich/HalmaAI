#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <tuple>
#include <chrono>
#include "Game.h"
#include "Heuristic.h"
#include "pprint.h"
void printHeuristicVector(const std::vector<Heuristic*>& heuristics) {
    std::cout << "[ ";
    for (const auto& heuristic : heuristics) {
        std::cout << heuristic->toString()<< ", ";
    }
    std::cout << "]";
}

void printDoubleVector(const std::vector<double>& weights) {
    std::cout << "[ ";
    for (const auto& weight : weights) {
        std::cout << weight << ", ";
    }
    std::cout << "]";
}

void printPlayerPhases(const std::vector<std::tuple<std::vector<Heuristic*>, std::vector<double>>>& playerPhases) {
    for (const auto& phase : playerPhases) {
        const auto& heuristics = std::get<0>(phase);
        const auto& weights = std::get<1>(phase);
        printHeuristicVector(heuristics);
        std::cout << " , ";
        printDoubleVector(weights);
        std::cout << std::endl;
    }
}
int countPawnsInOppositeBase(const Game& game, Piece player) {
    int count = 0;
    int startRow, endRow;

    if (player == Piece::PLAYER1) {
        startRow = 0;
        endRow = 4;
    } else {
        startRow = 11;
        endRow = 15;
    }

    for (int i = startRow; i <= endRow; ++i) {
        for (int j = 0; j < Game::BOARD_SIZE; ++j) {
            if (game.board[i][j] == player) {
                count++;
            }
        }
    }

    return count;
}
void getHumanMove(Game& game, Move& move) {
    bool validMove = false;

    while (!validMove) {
        std::cout << "Enter your move in the format 'from_x from_y to_x to_y': ";
        int from_x, from_y, to_x, to_y;
        std::cin >> from_x >> from_y >> to_x >> to_y;
        std::vector<Move> possibleMoves = game.allPossibleMoves(Piece::PLAYER2);

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //
            std::cout << "Invalid move. Please try again. Possible moves:" << std::endl;
            for (const auto& m : possibleMoves) {

                print("from", m.from.x,m.from.y, "to", m.to.x,m.to.y);
            }
            std::cout << "Invalid input. Please enter integers in the format 'from_x from_y to_x to_y'." << std::endl;
            continue;
        }
        move = Move{{from_x, from_y}, {to_x, to_y}};

        for (const auto& m : possibleMoves) {

            if (m.from == move.from && m.to == move.to) {
                validMove = true;
                break;
            }
        }
        if (!validMove) {
            std::cout << "Invalid move. Please try again. Possible moves:" << std::endl;
            for (const auto& m : possibleMoves) {

                print("from", m.from.x,m.from.y, "to", m.to.y,m.to.y);
            }
        }

    }
}
int playWithHuman() {
    ClosestToOpponentCampEuclideanDistanceHeuristic closestToOpponentBaseHeuristic;
    MostMovesHeuristic mostMovesHeuristic;
    PawnClusteringHeuristic pawnClusteringHeuristic;

    char initialBoard[Game::BOARD_SIZE][Game::BOARD_SIZE] = {
            {'-', '1', '-', '-', '1', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
            {'1', '-', '-', '1', '1', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
            {'-', '2', '1', '1', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
            {'-', '-', '1', '-', '-', '1', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
            {'1', '1', '-', '-', '1', '-', '1', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
            {'-', '-', '-', '-', '-', '1', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
            {'-', '-', '-', '-', '-', '-', '1', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
            {'-', '-', '-', '-', '-', '-', '1', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
            {'-', '-', '-', '-', '-', '1', '-', '-', '2', '-', '-', '-', '-', '-', '-', '-'},
            {'-', '-', '-', '-', '-', '-', '-', '-', '2', '2', '2', '2', '-', '-', '-', '-'},
            {'-', '-', '-', '-', '-', '-', '-', '2', '-', '-', '2', '-', '-', '-', '-', '-'},
            {'-', '-', '-', '-', '-', '-', '-', '-', '1', '-', '-', '2', '-', '-', '2', '2'},
            {'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '1', '-', '-', '2', '-'},
            {'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '2', '2', '-', '-'},
            {'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '2', '-', '2', '-', '2'},
            {'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '2', '-', '-', '2', '-'}
    };

    // Game with players' pawns like in initialBoard defined above
    Game game;

    std::vector<Heuristic*> player1Strategy = {&closestToOpponentBaseHeuristic, &pawnClusteringHeuristic};
    std::vector<double> player1Weights = {1, 0.4};
    game.setPlayerStrategy(player1Strategy, Piece::PLAYER1);
    game.setPlayerWeights(player1Weights, Piece::PLAYER1);

    std::vector<Heuristic*> player2Strategy = {&closestToOpponentBaseHeuristic, &pawnClusteringHeuristic};
    std::vector<double> player2Weights = {1, 0.1};
    game.setPlayerStrategy(player2Strategy, Piece::PLAYER2);
    game.setPlayerWeights(player2Weights, Piece::PLAYER2);

    std::cout << "Initial board:" << std::endl;
//    game.printBoard();

    for (int turn = 1; turn <= 500; ++turn) {
        if (game.isGameOver()) {
            std::cout << "Game over!" << std::endl;
            break;
        }
        std::cout << "Turn: " << turn << std::endl;
        game.printCurrentTurnPlayer();

        if (game.currentTurnPlayer == Piece::PLAYER2) {
            Move humanMove;
            getHumanMove(game, humanMove);
            game.makeMove(humanMove);
            std::cout << "Player 2 made the move: ";
            game.printBestMove();
        } else {

            game.makeBestMoveAlphaBeta();
            game.printBestMove();
        }

//        game.printBoard();

        // Adjust strategies after certain turns if needed
        if (turn == 50) {
            game.maxDepth = 3;
            player1Strategy = {&closestToOpponentBaseHeuristic, &pawnClusteringHeuristic};
            player1Weights = {1, 0.4};
            game.setPlayerStrategy(player1Strategy, Piece::PLAYER1);
            game.setPlayerWeights(player1Weights, Piece::PLAYER1);

            player2Strategy = {&closestToOpponentBaseHeuristic, &pawnClusteringHeuristic};
            player2Weights = {1, 0.1};
            game.setPlayerStrategy(player2Strategy, Piece::PLAYER2);
            game.setPlayerWeights(player2Weights, Piece::PLAYER2);
        } else if (turn == 100) {
            game.maxDepth = 3;
            player1Strategy = {&closestToOpponentBaseHeuristic};
            player1Weights = {1, 0.3};
            game.setPlayerStrategy(player1Strategy, Piece::PLAYER1);
            game.setPlayerWeights(player1Weights, Piece::PLAYER1);

            player2Strategy = {&closestToOpponentBaseHeuristic};
            player2Weights = {1, 0.1};
            game.setPlayerStrategy(player2Strategy, Piece::PLAYER2);
            game.setPlayerWeights(player2Weights, Piece::PLAYER2);
        }
    }

    return 0;
}
void runTournament(int numGames, int depth1, int depth2, bool useAlphaBeta1, bool useAlphaBeta2) {
    ClosestToOpponentCampEuclideanDistanceHeuristic closestToOpponentBaseHeuristic;
    MostMovesHeuristic mostMovesHeuristic;
    PawnClusteringHeuristic pawnClusteringHeuristic;

    std::vector<Heuristic*> strategy1 = {&closestToOpponentBaseHeuristic, &pawnClusteringHeuristic};
    std::vector<Heuristic*> strategy2 = {&closestToOpponentBaseHeuristic, &pawnClusteringHeuristic};
    std::vector<double> weights1 = {1, 0.3};
    std::vector<double> weights2 = {1, 0.3};

    int wins1 = 0;
    int wins2 = 0;
    double totalTime1 = 0;
    double totalTime2 = 0;

    std::ofstream logFile("C:\\Users\\szymo\\CLionProjects\\Lista2_AI_Kielich\\move_times.csv", std::ios::out | std::ios::trunc);
    if (!logFile.is_open()) {
        std::cerr << "Failed to open file: move_times.csv" << std::endl;
        return;
    }
    logFile << "Game,Turn,Player,Depth,AlphaBeta,MoveTime\n";
    for (int gameNum = 0; gameNum < numGames; ++gameNum) {
        Game game;
        game.setPlayerStrategy(strategy1, Piece::PLAYER1);
        game.setPlayerWeights(weights1, Piece::PLAYER1);
        game.setPlayerStrategy(strategy2, Piece::PLAYER2);
        game.setPlayerWeights(weights2, Piece::PLAYER2);

        for (int turn = 1; turn <= 500; ++turn) {
            if (game.isGameOver()) {
                if (game.currentTurnPlayer == Piece::PLAYER2) {
                    wins1++;
                } else {
                    wins2++;
                }
                break;
            }

            auto start = std::chrono::high_resolution_clock::now();
            std::cout << "Game: " << gameNum + 1 << " Turn: " << turn << std::endl;
            game.printCurrentTurnPlayer();

            game.printBestMove();
//            game.printBoard();
            int depth = (game.currentTurnPlayer == Piece::PLAYER1) ? depth1 : depth2;
            game.maxDepth = depth;

            if (game.currentTurnPlayer==Piece::PLAYER1&&useAlphaBeta1) {
                game.makeBestMoveAlphaBeta();
            } else {
                game.makeBestMoveMiniMax();
            }
            if (game.currentTurnPlayer==Piece::PLAYER2&&useAlphaBeta2) {
                game.makeBestMoveAlphaBeta();
            } else {
                game.makeBestMoveMiniMax();
            }

            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> duration = end - start;

            if (game.currentTurnPlayer == Piece::PLAYER1) {
                totalTime1 += duration.count();
            } else {
                totalTime2 += duration.count();
            }
            if (turn == 100) {
                game.maxDepth = 3;
//            strategy = {&closestToOpponentBaseHeuristic};
                strategy1 = {&closestToOpponentBaseHeuristic};
                weights1 = {1, 0.3};
                game.setPlayerStrategy(strategy1, Piece::PLAYER1);
                game.setPlayerWeights(weights1, Piece::PLAYER1);

//            strategy = {&closestToOpponentBaseHeuristic};
                strategy2 = {&closestToOpponentBaseHeuristic};
                weights2 = {1, 0.1};
                game.setPlayerStrategy(strategy2, Piece::PLAYER2);
                game.setPlayerWeights(weights2, Piece::PLAYER2);
            }

            logFile << gameNum + 1 << "," << turn << "," << (game.currentTurnPlayer == Piece::PLAYER1 ? "Player 1" : "Player 2")
                    << "," << depth << "," << (useAlphaBeta1 ? "AlphaBeta" : "MiniMax") << "," << duration.count() << "\n";

        }
    }

    logFile.close();

    std::cout << "Results for depth " << depth1 << (useAlphaBeta2 ? " with alpha-beta:" : " with minimax:") << std::endl;
    std::cout << "Player 1 wins: " << wins1 << "/" << numGames << std::endl;
    std::cout << "Player 2 wins: " << wins2 << "/" << numGames << std::endl;
    std::cout << "Average time for Player 1: " << totalTime1 / numGames << " seconds" << std::endl;
    std::cout << "Average time for Player 2: " << totalTime2 / numGames << " seconds" << std::endl;
}
//void runTournament(int numGames, const std::vector<std::vector<std::tuple<std::vector<Heuristic*>, std::vector<double>>>>& playerPhasesList) {
//    std::vector<int> wins(playerPhasesList.size(), 0);
//
//    for (size_t i = 0; i < playerPhasesList.size(); ++i) {
//        for (size_t j = i + 1; j < playerPhasesList.size(); ++j) {
//            int player1Wins = 0;
//            int player2Wins = 0;
//
//            for (int gameNum = 0; gameNum < numGames; ++gameNum) {
//                Game game;
//                game.setPlayerStrategy(std::get<0>(playerPhasesList[i][0]), Piece::PLAYER1);
//                game.setPlayerWeights(std::get<1>(playerPhasesList[i][0]), Piece::PLAYER1);
//                game.setPlayerStrategy(std::get<0>(playerPhasesList[j][0]), Piece::PLAYER2);
//                game.setPlayerWeights(std::get<1>(playerPhasesList[j][0]), Piece::PLAYER2);
//
//                for (int turn = 1; turn <= 300; ++turn) {
//                    if (game.isGameOver()) {
//                        break;
//                    }
//
//                    game.makeBestMoveAlphaBeta();
//
//                    if (turn == 50 || turn == 100) {
//                        game.setPlayerStrategy(std::get<0>(playerPhasesList[i][turn == 50 ? 1 : 2]), Piece::PLAYER1);
//                        game.setPlayerWeights(std::get<1>(playerPhasesList[i][turn == 50 ? 1 : 2]), Piece::PLAYER1);
//                        game.setPlayerStrategy(std::get<0>(playerPhasesList[j][turn == 50 ? 1 : 2]), Piece::PLAYER2);
//                        game.setPlayerWeights(std::get<1>(playerPhasesList[j][turn == 50 ? 1 : 2]), Piece::PLAYER2);
//                    }
//                }
//
//                int player1PawnsInBase = countPawnsInOppositeBase(game, Piece::PLAYER1);
//                int player2PawnsInBase = countPawnsInOppositeBase(game, Piece::PLAYER2);
//
//                if (player1PawnsInBase > player2PawnsInBase) {
//                    player1Wins++;
//                } else if (player2PawnsInBase > player1PawnsInBase) {
//                    player2Wins++;
//                }
//            }
//
//            wins[i] += player1Wins;
//            wins[j] += player2Wins;
//
//            std::cout << "Match between Strategy " << i + 1 << " and Strategy " << j + 1 << ":\n";
//            std::cout << "Strategy " << i + 1 << " wins: " << player1Wins << "\n";
//            std::cout << "Strategy " << j + 1 << " wins: " << player2Wins << "\n";
//        }
//    }
//
//    std::cout << "Final Results:\n";
//    for (size_t i = 0; i < wins.size(); ++i) {
//        std::cout << "Strategy " << i + 1 << " total wins: " << wins[i] << "\n";
//    }
//}
int playMatch(const std::vector<std::tuple<std::vector<Heuristic*>, std::vector<double>>>& player1Phases,
              const std::vector<std::tuple<std::vector<Heuristic*>, std::vector<double>>>& player2Phases) {
    print("match started");
    ClosestToOpponentCampEuclideanDistanceHeuristic closestToOpponentBaseHeuristic;
    MostMovesHeuristic mostMovesHeuristic;
    PawnClusteringHeuristic pawnClusteringHeuristic;

    Game game;

    std::vector<Heuristic*> player1Strategy = std::get<0>(player1Phases[0]);
    std::vector<double> player1Weights = std::get<1>(player1Phases[0]);
    game.setPlayerStrategy(player1Strategy, Piece::PLAYER1);
    game.setPlayerWeights(player1Weights, Piece::PLAYER1);

    std::vector<Heuristic*> player2Strategy = std::get<0>(player2Phases[0]);
    std::vector<double> player2Weights = std::get<1>(player2Phases[0]);
    game.setPlayerStrategy(player2Strategy, Piece::PLAYER2);
    game.setPlayerWeights(player2Weights, Piece::PLAYER2);
    game.maxDepth=2;
    for (int turn = 1; turn <= 150; ++turn) {
        if (game.isGameOver()) {
            return (game.currentTurnPlayer == Piece::PLAYER1) ? 1 : 2;
        }
        game.makeBestMoveAlphaBeta();

        if (turn == 50) {
            player1Strategy = std::get<0>(player1Phases[1]);
            player1Weights = std::get<1>(player1Phases[1]);
            game.setPlayerStrategy(player1Strategy, Piece::PLAYER1);
            game.setPlayerWeights(player1Weights, Piece::PLAYER1);

            player2Strategy = std::get<0>(player2Phases[1]);
            player2Weights = std::get<1>(player2Phases[1]);
            game.setPlayerStrategy(player2Strategy, Piece::PLAYER2);
            game.setPlayerWeights(player2Weights, Piece::PLAYER2);
        } else if (turn == 100) {
            game.maxDepth = 2;
//            print("sto ruchów");
            player1Strategy = std::get<0>(player1Phases[2]);
            player1Weights = std::get<1>(player1Phases[2]);
            game.setPlayerStrategy(player1Strategy, Piece::PLAYER1);
            game.setPlayerWeights(player1Weights, Piece::PLAYER1);

            player2Strategy = std::get<0>(player2Phases[2]);
            player2Weights = std::get<1>(player2Phases[2]);
            game.setPlayerStrategy(player2Strategy, Piece::PLAYER2);
            game.setPlayerWeights(player2Weights, Piece::PLAYER2);
        }
//        game.printBoard();
//        print(turn);
    }

    int player1PawnsInBase = countPawnsInOppositeBase(game, Piece::PLAYER1);
    int player2PawnsInBase = countPawnsInOppositeBase(game, Piece::PLAYER2);

    if (player1PawnsInBase > player2PawnsInBase) {
        print("1");
        return 1;
    } else if (player2PawnsInBase > player1PawnsInBase) {
        print("2");
        return 2;
    } else {
        print("0");
        return 0; // It's a draw
    }
}

void runTournament(const std::vector<std::vector<std::tuple<std::vector<Heuristic*>, std::vector<double>>>>& playersPhases) {
    std::vector<int> winners;

    for (int i = 0; i < 8; i += 2) {
        print("---------------------------------");
        printPlayerPhases(playersPhases[i]);
        print("vs");
        printPlayerPhases(playersPhases[i+1]);

        int winner = playMatch(playersPhases[i], playersPhases[i + 1]);
        int winner_ind = winner == 1 ? i : i + 1;
        printPlayerPhases(playersPhases[winner_ind]);
        std::printf("player%d WON!!\n", winner_ind);
        winners.push_back(winner);
    }


    std::vector<int> semiFinalWinners;
    for (int i = 0; i < 4; i += 2) {
        print("----------SEMIFINALS--------------");
        printPlayerPhases(playersPhases[winners[i]]);
        print("vs");
        printPlayerPhases(playersPhases[winners[i + 1]]);
        int winner = playMatch(playersPhases[winners[i]], playersPhases[winners[i + 1]]);

        printPlayerPhases(playersPhases[winner == 1 ? winners[i] : winners[i + 1]]);
        semiFinalWinners.push_back(winner == 1 ? winners[i] : winners[i + 1]);
    }

    int finalWinner = playMatch(playersPhases[semiFinalWinners[0]], playersPhases[semiFinalWinners[1]]);
    finalWinner = finalWinner == 1 ? semiFinalWinners[0] + 1 : semiFinalWinners[1] + 1;
    std::cout << "The winner of the tournament is Player " << (finalWinner) << std::endl;

    printPlayerPhases(playersPhases[finalWinner]);
}



int test(int depth, const std::vector<std::tuple<std::vector<Heuristic*>, std::vector<double>>>& player1Phases,
         const std::vector<std::tuple<std::vector<Heuristic*>, std::vector<double>>>& player2Phases) {

    ClosestToOpponentCampEuclideanDistanceHeuristic closestToOpponentBaseHeuristic;
    MostMovesHeuristic mostMovesHeuristic;
    PawnClusteringHeuristic pawnClusteringHeuristic;
//    - 1 - - 1 - - - - - - - - - - -
//    1 - - 1 1 - - - - - - - - - - -
//    - 2 1 1 - - - - - - - - - - - -
//    - - 1 - - 1 - - - - - - - - - -
//    1 1 - - 1 - 1 - - - - - - - - -
//    - - - - - 1 - - - - - - - - - -
//    - - - - - - 1 - - - - - - - - -
//    - - - - - - 1 - - - - - - - - -
//    - - - - - 1 - - 2 - - - - - - -
//    - - - - - - - - 2 2 2 2 - - - -
//    - - - - - - - 2 - - 2 - - - - -
//    - - - - - - - - 1 - - 2 - - 2 2
//    - - - - - - - - - - - 1 - - 2 -
//    - - - - - - - - - - - - 2 2 - -
//    - - - - - - - - - - - 2 - 2 - 2
//    - - - - - - - - - - - 2 - - 2 -

    char initialBoard[Game::BOARD_SIZE][Game::BOARD_SIZE] = {
            {'-', '1', '-', '-', '1', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
            {'1', '-', '-', '1', '1', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
            {'-', '2', '1', '1', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
            {'-', '-', '1', '-', '-', '1', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
            {'1', '1', '-', '-', '1', '-', '1', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
            {'-', '-', '-', '-', '-', '1', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
            {'-', '-', '-', '-', '-', '-', '1', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
            {'-', '-', '-', '-', '-', '-', '1', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
            {'-', '-', '-', '-', '-', '1', '-', '-', '2', '-', '-', '-', '-', '-', '-', '-'},
            {'-', '-', '-', '-', '-', '-', '-', '-', '2', '2', '2', '2', '-', '-', '-', '-'},
            {'-', '-', '-', '-', '-', '-', '-', '2', '-', '-', '2', '-', '-', '-', '-', '-'},
            {'-', '-', '-', '-', '-', '-', '-', '-', '1', '-', '-', '2', '-', '-', '2', '2'},
            {'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '1', '-', '-', '2', '-'},
            {'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '2', '2', '-', '-'},
            {'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '2', '-', '2', '-', '2'},
            {'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '2', '-', '-', '2', '-'}
    };

//    Game game("C:\\Users\\szymo\\CLionProjects\\Lista2_AI_Kielich\\start.csv");
    Game game;
//    Game game(initialBoard);

    // Weights from 0 to 1
    std::vector<Heuristic *> player1Strategy = std::get<0>(player1Phases[0]);
    std::vector<double> player1Weights =std::get<1>(player1Phases[0]);
    game.setPlayerStrategy(player1Strategy, Piece::PLAYER1);
    game.setPlayerWeights(player1Weights, Piece::PLAYER1);

    std::vector<Heuristic *> player2Strategy = std::get<0>(player2Phases[0]);
    std::vector<double> player2Weights = std::get<1>(player2Phases[0]);
    game.setPlayerStrategy(player2Strategy, Piece::PLAYER2);
    game.setPlayerWeights(player2Weights, Piece::PLAYER2);
    game.setPlayerStrategy(player1Strategy, Piece::PLAYER1);
    game.setPlayerWeights(player1Weights, Piece::PLAYER1);


    std::cout << "Initial board:" << std::endl;
    game.maxDepth=depth;
    for (int turn = 1; turn <= 500; ++turn) {
        if (game.isGameOver()) {
            std::cout << "Game over!" << std::endl;
            break;
        }
        std::cout << "Turn: " << turn << std::endl;
        game.printCurrentTurnPlayer();
        std::vector<Move> possibleMoves = game.allPossibleMoves(Piece::PLAYER2);
//        for (const auto& m : possibleMoves) {
//
//            print("from", m.from.x,m.from.y, "to", m.from.y,m.to.y);
//        }
        game.makeBestMoveAlphaBeta();
        game.printBestMove();
//        game.printBoard();


        if (turn == 50) {
            game.maxDepth = 3;
//            player1Strategy = {&closestToOpponentBaseHeuristic,& mostMovesHeuristic};
//            player1Strategy = {&closestToOpponentBaseHeuristic};
            player1Strategy = std::get<0>(player1Phases[1]);
            player1Weights = std::get<1>(player1Phases[1]);
            game.setPlayerStrategy(player1Strategy, Piece::PLAYER1);
            game.setPlayerWeights(player1Weights, Piece::PLAYER1);

//            player2Strategy = {&closestToOpponentBaseHeuristic,& mostMovesHeuristic};
//            player1Strategy = {&closestToOpponentBaseHeuristic};
            player2Strategy = std::get<0>(player2Phases[1]);
            player2Weights = std::get<1>(player2Phases[1]);
            game.setPlayerStrategy(player2Strategy, Piece::PLAYER2);
            game.setPlayerWeights(player2Weights, Piece::PLAYER2);
        } else if (turn == 100) {
            game.maxDepth = 3;
//            player1Strategy = {&closestToOpponentBaseHeuristic};
            player1Strategy = std::get<0>(player1Phases[2]);
            player1Weights = std::get<1>(player1Phases[2]);
            game.setPlayerStrategy(player1Strategy, Piece::PLAYER1);
            game.setPlayerWeights(player1Weights, Piece::PLAYER1);

//            player2Strategy = {&closestToOpponentBaseHeuristic};
            player2Strategy = std::get<0>(player2Phases[2]);
            player2Weights = std::get<1>(player2Phases[2]);
            game.setPlayerStrategy(player2Strategy, Piece::PLAYER2);
            game.setPlayerWeights(player2Weights, Piece::PLAYER2);
        }
    }

    return 0;
}
int main() {
    ClosestToOpponentCampEuclideanDistanceHeuristic closestToOpponentBaseHeuristic;
    MostMovesHeuristic mostMovesHeuristic;
    PawnClusteringHeuristic pawnClusteringHeuristic;
    std::vector<std::tuple<std::vector<Heuristic*>, std::vector<double>>> player1Phases = {
            {{&closestToOpponentBaseHeuristic, &pawnClusteringHeuristic}, {1, 0.4}},
            {{&closestToOpponentBaseHeuristic}, {1, 0.3}},
            {{&closestToOpponentBaseHeuristic, &mostMovesHeuristic}, {1, 0.5}}
    };

    std::vector<std::tuple<std::vector<Heuristic*>, std::vector<double>>> player2Phases = {
            {{&closestToOpponentBaseHeuristic, &pawnClusteringHeuristic}, {1, 0.1}},
            {{&closestToOpponentBaseHeuristic}, {1, 0.1}},
            {{&closestToOpponentBaseHeuristic, &mostMovesHeuristic}, {1, 0.2}}
    };

    std::vector<std::tuple<std::vector<Heuristic*>, std::vector<double>>> player3Phases = {
            {{&closestToOpponentBaseHeuristic, &pawnClusteringHeuristic, &mostMovesHeuristic}, {0.8, 0.2, 0.5}},
            {{&closestToOpponentBaseHeuristic}, {0.6}},
            {{&closestToOpponentBaseHeuristic, &mostMovesHeuristic}, {0.9, 0.1}},
    };

    std::vector<std::tuple<std::vector<Heuristic*>, std::vector<double>>> player4Phases = {
            {{&closestToOpponentBaseHeuristic, &mostMovesHeuristic}, {0.7, 0.3}},
            {{&closestToOpponentBaseHeuristic}, {0.9}},
            {{&closestToOpponentBaseHeuristic, &pawnClusteringHeuristic}, {0.6, 0.4}},
    };

    std::vector<std::tuple<std::vector<Heuristic*>, std::vector<double>>> player5Phases = {
            {{&closestToOpponentBaseHeuristic, &pawnClusteringHeuristic}, {0.9, 0.1}},
            {{&closestToOpponentBaseHeuristic}, {0.7}},
            {{&closestToOpponentBaseHeuristic, &mostMovesHeuristic}, {0.5, 0.5}},
    };

    std::vector<std::tuple<std::vector<Heuristic*>, std::vector<double>>> player6Phases = {
            {{&closestToOpponentBaseHeuristic, &mostMovesHeuristic}, {0.6, 0.4}},
            {{&closestToOpponentBaseHeuristic}, {0.8}},
            {{&closestToOpponentBaseHeuristic, &pawnClusteringHeuristic}, {0.4, 0.6}},
    };
    std::vector<std::tuple<std::vector<Heuristic*>, std::vector<double>>> player7Phases = {
            {{&closestToOpponentBaseHeuristic, &pawnClusteringHeuristic}, {1, 0.4}},
            {{&closestToOpponentBaseHeuristic,& pawnClusteringHeuristic}, {1, 0.4}},
            {{&closestToOpponentBaseHeuristic}, {1, 0.3}}
    };

    std::vector<std::tuple<std::vector<Heuristic*>, std::vector<double>>> player8Phases = {
            {{&closestToOpponentBaseHeuristic, &pawnClusteringHeuristic}, {1, 0.1}},
            {{&closestToOpponentBaseHeuristic,& pawnClusteringHeuristic}, {1, 0.1}},
            {{&closestToOpponentBaseHeuristic}, {1, 0.1}}
    };

    int depth = 2;
//    runTournament(5, 2, 2, true, false);
//    test(depth, player7Phases, player8Phases);
//    playWithHuman();
    std::vector<std::vector<std::tuple<std::vector<Heuristic*>, std::vector<double>>>> playersPhases = {
            player1Phases, player2Phases, player3Phases, player4Phases,
            player5Phases, player6Phases, player7Phases, player8Phases
    };
//    for (int i=0; i<3; i++) {
//        print(player8Phases[i]);
//    }
    printPlayerPhases(player8Phases);

    runTournament(playersPhases);
//    runTournament(numGames, 3, 4, false); // Alpha-beta depth 3 vs depth 4

    return 0;
}
