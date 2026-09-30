#include <algorithm>
#include <cmath>
#include <functional>
#include <iomanip>
#include <queue>
#include <random>
#include <unordered_map>

#include "puzzle.hpp"

namespace {

// The four slides of the blank, as (row, column) offsets.
constexpr std::array<int, 4> rowStep {-1, 1, 0, 0};
constexpr std::array<int, 4> colStep {0, 0, -1, 1};
constexpr std::array<char, 4> moveName {'U', 'D', 'L', 'R'}; 

// Position of (row, col) in the 0..15 numbering, or -1 if off the grid.
int positionOf(int row, int col) {
    if (row < 0 || row > 3 || col < 0 || col > 3) {
        return -1;
    }
    return 4 * row + col;
}

} // namespace

Board::Board() {
    for (int p = 0; p < 15; ++p) {
        tiles[p] = static_cast<std::uint8_t>(p + 1);
    }
    tiles[15] = 0;
}

Board::Board(const Tiles& tileValues) : tiles {tileValues} {}

bool Board::isValid() const {
    std::array<bool, 16> seen {};
    for (std::uint8_t tile : tiles) {
        if (tile > 15 || seen[tile]) {
            return false;
        }
        seen[tile] = true;
    }
    return true;
}

// A board is reachable from the goal only when
// (inversions + blank's row from the bottom) is odd.
bool Board::isSolvable() const {
    int inversions {0};
    for (int p = 0; p < 16; ++p) {
        if (tiles[p] == 0) {
            continue;
        }
        for (int q = p + 1; q < 16; ++q) {
            if (tiles[q] != 0 && tiles[p] > tiles[q]) {
                ++inversions;
            }
        }
    }
    int blankRowFromBottom = 4 - blank() / 4;
    return (inversions + blankRowFromBottom) % 2 == 1; 
}

int Board::manhattan() const {
    int total {0};
    for (int p = 0; p < 16; ++p) {
        if (tiles[p] == 0) {
            continue;   // skip the blank (mentioned in report)
        }
        int goal = tiles[p] - 1;    // tile t belongs at position t-1
        total += std::abs(p / 4 - goal / 4) + std::abs(p % 4 - goal % 4);
    }
    return total;
}

int Board::blank() const {
    for (int p = 0; p < 16; ++p) {
        if (tiles[p] == 0) {
            return p;
        }
    }
    return -1;      // unreachable once isValid() has passed
}

Board Board::slide(int target) const {
    Board next {*this};
    std::swap(next.tiles[blank()], next.tiles[target]);
    return next;
}

// A* search. Sort boards by f = g + manhattan, process the smallest first.
SearchResult solve(const Board& start) {
    const Board goal {};

    if (start == goal) {
        return {true, false, false, {}, 0};
    }
    if (!start.isSolvable()) {
        return {false, true, false, {}, 0};
    }

    struct Node {
        int g {};               // moves from the start board to this board
        Board parent {};        // the board this one was reached from
        char move {};           // the slide that reached it: U, D, L or R
        bool processed {};      // have this board's own slides been worked out?
    };
    std::unordered_map<Board, Node> nodes {};

    // Smallest f on top (std::greater makes it a minimum priority queue).
    using Entry = std::pair<int, Board>;
    std::priority_queue<Entry, std::vector<Entry>, std::greater<Entry> > queue {};

    // The start board took no slides to reach and has no parent.
    nodes[start] = {0, Board {}, ' ', false};
    queue.push({start.manhattan(), start});

    constexpr int maxBoardsToProcess {3'000'000};
    int processed {0};

    while (!queue.empty()) {
        Board current {queue.top().second};
        queue.pop();

        Node& node = nodes[current];
        if (node.processed) {
            continue;           // a stale entry: we already have its best path
        }
        node.processed = true;

        if (current == goal) {
            std::vector<char> moves {};
            for (Board b {current}; b != start; b = nodes[b].parent) {
                moves.push_back(nodes[b].move);
            }
            std::reverse(moves.begin(), moves.end());
            return {true, false, false, moves, processed};
        }

        ++processed;
        if (processed == maxBoardsToProcess) {
            return {false, false, true, {}, processed};
        }

        int g = node.g;
        int blank = current.blank();
        for (int d = 0; d < 4; ++d) {
            int target = positionOf(blank / 4 + rowStep[d], blank % 4 + colStep[d]);
            if (target < 0) {
                continue;
            }

            Board next {current.slide(target)};
            auto found = nodes.find(next);
            if (found != nodes.end() && found->second.processed) {
                continue;
            }
            if (found == nodes.end() || g + 1 < found->second.g) {
                nodes[next] = {g + 1, current, moveName[d], false};
                queue.push({g + 1 + next.manhattan(), next});
            }
        }
    }

    return {false, false, false, {}, processed};     // unreachable
}

Board randomBoard(unsigned seed, int scrambleMoves) {
    Board board {};
    std::mt19937 generator {seed};

    // Slides are reversible, so scrambling the goal can only ever reach
    // boards that are solvable.
    for (int i = 0; i < scrambleMoves; ++i) {
        int blank = board.blank();
        std::vector<int> targets {};
        for (int d = 0; d < 4; ++d) {
            int target = positionOf(blank / 4 + rowStep[d], blank % 4 + colStep[d]);
            if (target >= 0) {
                targets.push_back(target);
            }
        }
        std::uniform_int_distribution<std::size_t> choose {0, targets.size() - 1};
        board = board.slide(targets[choose(generator)]);
    }
    return board;
}

std::ostream& operator<<(std::ostream& os, const Board& board) {
    const Board::Tiles& tiles = board.values();
    for (int p = 0; p < 16; ++p) {
        if (tiles[p] == 0) {
            os << "  _";
        } else {
            os << std::setw(3) << static_cast<int>(tiles[p]);
        }
        if (p % 4 == 3) {
            os << '\n';
        }
    }
    return os;
}