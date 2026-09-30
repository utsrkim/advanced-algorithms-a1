#ifndef PUZZLE_HPP_
#define PUZZLE_HPP_

#include <array>
#include <cstdint>
#include <iostream>
#include <vector>

// A state of the 15 puzzle: a 4x4 grid holding tiles 1..15 and one blank.
class Board {
public:
    // Positions are numbered 0..15 in reading order,
    // left to right and top to bottom:
    //    0  1  2  3
    //    4  5  6  7
    //    8  9 10 11
    //   12 13 14 15
    // The value 0 denotes the blank square.
    using Tiles = std::array<std::uint8_t, 16>;

private:
    // tiles[p] is the tile sitting at position p
    Tiles tiles {};

public:
    // default constructor: the goal state 1,...,15, blank
    Board();

    // construct from an explicit arrangement
    explicit Board(const Tiles& tileValues);

    // does the arrangement use each of 0,...,15 exactly once?
    bool isValid() const;

    // is this arrangement reachable from the goal by legal slides?
    bool isSolvable() const;

    // sum over the fifteen tiles of the distance from a tile to its
    // goal square; this is the A* heuristic
    int manhattan() const;

    // position of the blank square
    int blank() const;

    // the board obtained by sliding the blank to position target;
    // target must be horizontally or vertically adjacent to the blank
    Board slide(int target) const;

    // give read-only access to the underlying array, for hashing
    const Tiles& values() const { return tiles; }

    auto operator<=>(const Board&) const = default;
};

// hash a Board so it can be a key of std::unordered_map
template <>
struct std::hash<Board> {
    std::size_t operator()(const Board& b) const {
        std::size_t h {0};
        for (std::uint8_t tile : b.values()) {
            h = h * 31 + tile;
        }
        return h;
    }
};

// what solve() reports back
struct SearchResult {
    bool solved {};
    bool unsolvable {};             // the parity test rejected the input
    bool exceeded {};               // the search hit its state budget
    std::vector<char> moves {};     // 'U', 'D', 'L', 'R': how the blank slid
    int boardsProcessed {};
};

// find a shortest solution with A*, using Board::manhattan as the heuristic
SearchResult solve(const Board& start);

// scramble the goal state with scrambleMoves random slides
Board randomBoard(unsigned seed, int scrambleMoves = 200);

// print a board as a 4x4 grid, with '_' for the blank
std::ostream& operator<<(std::ostream&, const Board&);

#endif // PUZZLE_HPP_
