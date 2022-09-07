#ifndef PUZZLE_HPP
#define PUZZLE_HPP
#pragma once

#include <cmath>
#include <iomanip>
#include <utility>
#include <vector>

class Puzzle
{
    // --- Ctor, Dtor
public:
    virtual ~Puzzle() = default;
    Puzzle(std::vector<size_t> vec1d);
    // interface to codewars
    Puzzle(const std::vector<std::vector<int>> &vec2d);

    Puzzle()                          = delete;
    Puzzle(const Puzzle &)            = default;
    Puzzle(Puzzle &&)                 = default;
    Puzzle &operator=(const Puzzle &) = delete;
    Puzzle &operator=(Puzzle &&)      = delete;

private:
    static auto linearise(const std::vector<std::vector<int>> &vec2d) -> std::vector<size_t>;
    auto extractIndices() -> void;

    // --- Public Interface
public:
    auto hasSolution() -> bool;
    auto moveToC(size_t iDesC) -> void;
    auto moveToR(size_t iDesR) -> void;
    auto moveTileToC(const size_t iTile, const size_t iDesC) -> void;
    auto moveTileToR(const size_t iTile, const size_t iDesR) -> void;
    auto orderRow(const size_t row) -> void;
    auto orderCol(const size_t col, const size_t start_row = 0) -> void;
    auto solve() -> void;

    auto getBoard() const -> const std::vector<size_t> & { return board_; }
    auto getTileCoos(const size_t iTile) const -> std::pair<size_t, size_t> { return idx2D(indices_.at(iTile)); }
    auto getMoves() const -> const std::vector<size_t> & { return moves_; }
    auto trimMoves() -> void;
    auto getSize() const -> size_t { return size_; }
    auto getSide() const -> size_t { return side_; }
    auto printMoves() const -> void;
    auto printBoard() const -> void;

    // interface to codewars
    auto getMovesShifted() const -> std::vector<int>;

private:
    auto iEmpty_() const -> size_t { return indices_[nEmpty_]; }
    auto swap(const size_t idx1, const size_t idx2) -> void;
    auto move(int count, size_t dist) -> void;
    auto moveNextToTileR(const size_t iTile, const int signR) -> void;
    auto moveNextToTileC(const size_t iTile, const int signC) -> void;

    auto idx2D(const size_t idx) const -> std::pair<size_t, size_t>;
    auto idx1D(const size_t iRow, const size_t iCol) const -> size_t;

    // --- Data Members
private:
    std::vector<size_t> board_ {};
    const size_t size_ { board_.size() };
    std::vector<size_t> indices_ { std::vector<size_t>(size_, { 0 }) };
    const size_t side_ { static_cast<size_t>(std::sqrt(size_)) };
    const size_t edge_ { side_ - 1 };
    const size_t nEmpty_ { size_ - 1 };
    std::vector<size_t> moves_ {};
};

#endif    // PUZZLE_HPP
