puzzle: main.cpp puzzle.cpp puzzle.hpp
	g++ -std=c++23 -O2 -Wall -Wextra -Wpedantic -o puzzle main.cpp puzzle.cpp

clean:
	rm -f puzzle
