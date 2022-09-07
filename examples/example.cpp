#include <Puzzle.hpp>

#include <fmt/format.h>

#include <iostream>

auto main(int /*argc*/, char ** /*argv*/) -> int    // NOLINT (bugprone-exception-escape)
{                                                   // NOLINTBEGIN
    fmt::print("--- Example Start\n");
    Puzzle puzzle {
        {3, 0, 2, 1, 7, 8, 6, 5, 4}
    };
    puzzle.printBoard();
    puzzle.solve();
    puzzle.trimMoves();
    puzzle.printMoves();
    puzzle.printBoard();
    fmt::print("--- Example End\n");

    return 0;
}    // NOLINTEND
