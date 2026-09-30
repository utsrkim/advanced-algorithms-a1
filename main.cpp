#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#include "puzzle.hpp"

namespace {

void printUsage(const char* program) {
    std::cerr << "Usage:\n"
              << "  " << program << " solve [file]   solve a board read from file, or from stdin\n"
              << "  " << program << " random [seed]  scramble a board and solve it\n"
              << "\nA board is 16 numbers 0..15 in reading order, left to right and\n"
              << "top to bottom. 0 is the blank square.\n";
}

// Read sixteen tile values from in.  Returns false if fewer than sixteen
// numbers are available or a value is out of range.
bool readBoard(std::istream& in, Board& board) {
    Board::Tiles tiles {};
    for (int p = 0; p < 16; ++p) {
        int value {};
        if (!(in >> value) || value < 0 || value > 15) {
            return false;
        }
        tiles[p] = static_cast<std::uint8_t>(value);
    }
    board = Board {tiles};
    return true;
}

void printResult(const SearchResult& result, double seconds) {
    if (result.unsolvable) {
        std::cout << "No solution: this board is not reachable from the goal.\n";
        return;
    }
    if (result.exceeded) {
        std::cout << "Gave up after processing " << result.boardsProcessed
                  << " boards; this one needs more than A* can hold in memory.\n";
        return;
    }

    std::cout << "Solved in " << result.moves.size() << " moves\n"
              << "Boards processed: " << result.boardsProcessed << '\n'
              << "Time: " << std::fixed << std::setprecision(2) << seconds << "s\n"
              << "Moves: ";
    for (char move : result.moves) {
        std::cout << move << ' ';
    }
    std::cout << "\n\nGoal:\n" << Board {};
}

} // namespace

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    std::string command {argv[1]};
    Board board {};

    if (command == "solve") {
        if (argc > 2) {
            std::ifstream file {argv[2]};
            if (!file) {
                std::cerr << "Cannot open " << argv[2] << '\n';
                return 1;
            }
            if (!readBoard(file, board)) {
                std::cerr << "Expected 16 numbers 0..15 in " << argv[2] << '\n';
                return 1;
            }
        } else if (!readBoard(std::cin, board)) {
            std::cerr << "Expected 16 numbers 0..15 on standard input\n";
            return 1;
        }
    } else if (command == "random") {
        unsigned seed = (argc > 2) ? static_cast<unsigned>(std::stoul(argv[2]))
                                   : static_cast<unsigned>(std::time(nullptr));
        board = randomBoard(seed);
        std::cout << "Scrambled with seed " << seed << ".\n";
    } else {
        printUsage(argv[0]);
        return 1;
    }

    if (!board.isValid()) {
        std::cerr << "Not a board: each of 0..15 must appear exactly once\n";
        return 1;
    }

    std::cout << board << "\nSolving...\n";

    auto before = std::chrono::steady_clock::now();
    SearchResult result = solve(board);
    auto after = std::chrono::steady_clock::now();

    printResult(result, std::chrono::duration<double>(after - before).count());
    return result.solved ? 0 : 1;
}
