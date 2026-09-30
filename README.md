# advanced-algorithms-a1

Advanced Algorithms, Assignment 1 (SPR26)


## 15 Puzzle Solver

A command line solver for the 15 puzzle using A* search with the Manhattan distance heuristic.

Programming Assignment 1, Track B.


## Building

C++23 compiler.

```
make
```

This produces a program called `puzzle`. To delete it, run `make clean`.

Or compile it directly:

```
g++ -std=c++23 -O2 -o puzzle main.cpp puzzle.cpp
```


## Running

```
./puzzle solve [file]    solve a board read from a file, or from standard input
./puzzle random [seed]   scramble a board and solve it
```

A board is sixteen numbers from 0 to 15, separated by any whitespace, where 0 is the blank square. Read the grid left to right and top to bottom. Line breaks are ignored, so you can lay a board out as a grid:

```
 1  2  3  4
 5  6  7  8
 9 10  0 11
13 14 15 12
```

`random` takes an optional seed. With a seed the same board is produced every time. Without one the current time is used, so each run produces a different board.

The output includes the board, the shortest solution length, the number of boards processed, the time taken, and the solution as a sequence of moves (U, D, L, R), which describe where the blank slides.


## Example

```
$ echo "1 2 3 4 5 6 7 8 9 10 0 11 13 14 15 12" | ./puzzle solve
  1  2  3  4
  5  6  7  8
  9 10  _ 11
 13 14 15 12

Solving...
Solved in 2 moves
Boards processed: 2
Time: 0.00s
Moves: R D

Goal:
  1  2  3  4
  5  6  7  8
  9 10 11 12
 13 14 15  _
```

The blank slides right, swapping with tile 11, then down, swapping with tile 12.


## Files

- `puzzle.hpp` - the Board class, SearchResult, and the functions
- `puzzle.cpp` - the Board methods and the A* search
- `main.cpp` - argument handling, reading input, printing results
- `Makefile` - the build
- `report.pdf` - the written report
