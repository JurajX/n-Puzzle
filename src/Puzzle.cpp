#include <Puzzle.hpp>

#include <algorithm>
#include <cassert>
#include <exception>
#include <iostream>
#include <string>

#include <fmt/core.h>

template<class T>
auto sgn(T val) -> int
{
    return static_cast<int>((T { 0 } < val) - (val < T { 0 }));
}

Puzzle::Puzzle(std::vector<size_t> vec1d)
    : board_ { std::move(vec1d) }
{
    if (side_ * side_ != size_) {
        throw std::invalid_argument(fmt::format("The size of the passed vector must be a perfect square! Got: {}.", size_));
    }
    if (side_ < 2) {
        throw std::invalid_argument(fmt::format("The size of the passed vector, i.e. board, must be at least 2x2! Got: {}.", size_));
    }
    extractIndices();
    size_t avgDistance { edge_ };    // average distance to a tile's place for a random distribution
    moves_.reserve(2 * avgDistance * size_);
}

Puzzle::Puzzle(const std::vector<std::vector<int>> &vec2d)
    : Puzzle(linearise(vec2d))
{ }

auto Puzzle::linearise(const std::vector<std::vector<int>> &vec2d) -> std::vector<size_t>
{
    size_t size { vec2d.size() * vec2d.size() };
    std::vector<size_t> linear {};
    linear.reserve(size);
    for (const auto &row : vec2d) {
        for (const auto &elem : row) {
            if (elem < 0) {
                throw std::invalid_argument(fmt::format("The passed array contains a negative number: {}", elem));
            }
            (elem == 0) ? linear.push_back(size - 1) : linear.push_back(static_cast<size_t>(elem - 1));
        }
    }
    return linear;
}

auto Puzzle::extractIndices() -> void
{
    size_t sum { nEmpty_ * (nEmpty_ + 1) / 2 };
    for (size_t idx { 0 }; idx < size_; idx += 1) {
        size_t num { board_[idx] };
        if (num > nEmpty_) {
            throw std::invalid_argument(fmt::format("Numbers in the passed array cannot be bigger than {}. Got {}.", nEmpty_, num));
        }
        indices_.at(num) = idx;
        sum -= num;
    }
    if (sum != 0) {
        throw std::invalid_argument(fmt::format("The passed array must be a sequence [0..N). Where N is a perfect square."));
    }
}

auto Puzzle::hasSolution() -> bool
{
    auto [iEmpR, iEmpC] = idx2D(iEmpty_());
    moveToC(edge_);
    moveToR(edge_);
    std::vector<size_t> board { board_ };
    moveToR(iEmpR);
    moveToC(iEmpC);
    size_t count { 0 };
    for (size_t idx = 0; idx < nEmpty_; idx += 1) {
        while (board.at(idx) != idx) {
            std::swap(board.at(idx), board.at(board.at(idx)));
            count += 1;
        }
    }
    moves_.clear();
    return count % 2 == 0;
}

auto Puzzle::swap(const size_t idx1, const size_t idx2) -> void
{
    size_t &e1 { board_[idx1] };
    size_t &e2 { board_[idx2] };
    std::swap(e1, e2);
    std::swap(indices_[e1], indices_[e2]);
}

auto Puzzle::moveToC(size_t iDesC) -> void
{
    if (side_ <= iDesC) {
        throw std::invalid_argument(fmt::format("Move empty, column index must be smaller than {}. Got {}.", side_, iDesC));
    }

    auto [_, iEmpC] = idx2D(iEmpty_());
    int count { static_cast<int>(iDesC - iEmpC) };
    move(count, 1);
}

auto Puzzle::moveToR(size_t iDesR) -> void
{
    if (side_ <= iDesR) {
        throw std::invalid_argument(fmt::format("Move empty, row index must be smaller than {}. Got {}.", side_, iDesR));
    }

    auto [iEmpR, _] = idx2D(iEmpty_());
    int count { static_cast<int>(iDesR - iEmpR) };
    move(count, side_);
}

auto Puzzle::move(int count, size_t dist) -> void
{
    int sign { sgn(count) };
    int jump { sign * static_cast<int>(dist) };
    for (; count != 0; count -= sign) {
        size_t iOther { static_cast<size_t>(jump + static_cast<int>(iEmpty_())) };
        moves_.push_back(board_[iOther]);
        swap(iEmpty_(), iOther);
    }
}

auto Puzzle::idx2D(const size_t idx) const -> std::pair<size_t, size_t>
{
    assert(idx < size_);    // NOLINT
    size_t iRow { idx / side_ };
    size_t iCol { idx - iRow * side_ };
    return { iRow, iCol };
}

auto Puzzle::idx1D(const size_t iRow, const size_t iCol) const -> size_t
{
    assert((iRow < side_) && (iCol < side_));    // NOLINT
    return iRow * side_ + iCol;
}

auto Puzzle::moveTileToC(const size_t iTile, const size_t iDesC) -> void
{
    if (iTile >= nEmpty_) {
        throw std::invalid_argument(fmt::format("Tiles are numbered from 0 to {}. Got {}.", nEmpty_ - 1, iTile));
    }
    if (iDesC >= side_) {
        throw std::invalid_argument(fmt::format("To move a tile, column index must be smaller than {}. Got {}.", side_, iDesC));
    }

    auto [iCurR, iCurC] = idx2D(indices_[iTile]);
    if (iCurC == iDesC) {
        return;
    }
    int signC { sgn(static_cast<int>(iDesC - iCurC)) };
    int signR { 1 - 2 * static_cast<int>(iCurR == edge_) };
    moveNextToTileR(iTile, signR);
    for (; iCurC != iDesC; iCurC += static_cast<size_t>(signC)) {
        moveToC(iCurC + static_cast<size_t>(signC));
        moveToR(iCurR);
        moveToC(iCurC);
        moveToR(iCurR + static_cast<size_t>(signR));
    }
}

auto Puzzle::moveNextToTileR(const size_t iTile, const int signR) -> void
{
    assert(signR * signR == 1);    // NOLINT
    auto [iCurR, iCurC] = idx2D(indices_[iTile]);
    auto [iEmpR, iEmpC] = idx2D(iEmpty_());
    size_t iDesR { iCurR + static_cast<size_t>(signR) };
    assert(iDesR < side_);    // NOLINT
    if (iEmpR == iDesR) {
        return;
    }
    if (iEmpC == iCurC) {
        size_t idx { iCurC + 1 - 2UL * static_cast<size_t>(iCurC == edge_) };
        moveToC(idx);
    }
    moveToR(iDesR);
}

auto Puzzle::moveTileToR(const size_t iTile, const size_t iDesR) -> void
{
    if (iTile >= nEmpty_) {
        throw std::invalid_argument(fmt::format("Tiles are numbered from 0 to {}. Got {}.", nEmpty_ - 1, iTile));
    }
    if (iDesR >= side_) {
        throw std::invalid_argument(fmt::format("To move a tile, row index must be smaller than {}. Got {}.", side_, iDesR));
    }

    auto [iCurR, iCurC] = idx2D(indices_[iTile]);
    if (iCurR == iDesR) {
        return;
    }
    int signR { sgn(static_cast<int>(iDesR - iCurR)) };
    int signC { 1 - 2 * static_cast<int>((iCurC == edge_)) };
    moveNextToTileC(iTile, signC);
    for (; iCurR != iDesR; iCurR += static_cast<size_t>(signR)) {
        moveToR(iCurR + static_cast<size_t>(signR));
        moveToC(iCurC);
        moveToR(iCurR);
        moveToC(iCurC + static_cast<size_t>(signC));
    }
}

auto Puzzle::moveNextToTileC(const size_t iTile, const int signC) -> void
{
    assert(signC * signC == 1);    // NOLINT
    auto [iCurR, iCurC] = idx2D(indices_[iTile]);
    auto [iEmpR, iEmpC] = idx2D(iEmpty_());
    size_t iDesC { iCurC + static_cast<size_t>(signC) };
    assert(iDesC < side_);    // NOLINT
    if (iEmpC == iDesC) {
        return;
    }
    if (iEmpR == iCurR) {
        size_t idx { iCurR + 1 - 2UL * static_cast<size_t>(iCurR == edge_) };
        moveToR(idx);
    }
    moveToC(iDesC);
}

auto Puzzle::orderRow(const size_t row) -> void
{
    if (row >= side_) {
        throw std::invalid_argument(fmt::format("The row index must be smaller than {}. Got {}.", side_, row));
    }

    for (size_t col { 0 }; col < edge_; col += 1) {
        const size_t iTile { idx1D(row, col) };
        moveTileToC(iTile, col);
        moveTileToR(iTile, row);
    }
    const size_t iTileLast { idx1D(row, edge_) };
    moveToR(row + 1);
    if (indices_[iTileLast] == iTileLast) {
        {
            return;
        }
    }
    moveTileToR(iTileLast, row + 2);
    moveTileToC(iTileLast, edge_);
    moveToC(edge_ - 1);
    moveToR(row + 1);
    moveTileToC(iTileLast - 1, edge_);
    moveTileToR(iTileLast, row + 1);
    moveTileToC(iTileLast - 1, edge_ - 1);
}

auto Puzzle::orderCol(const size_t col, const size_t start_row) -> void
{
    if (col >= side_) {
        throw std::invalid_argument(fmt::format("The column index must be smaller than {}. Got {}.", side_, col));
    }
    if (start_row >= side_) {
        throw std::invalid_argument(fmt::format("The start_row index must be smaller than {}. Got {}.", side_, col));
    }

    for (size_t row { start_row }; row < edge_; row += 1) {
        const size_t iTile { idx1D(row, col) };
        moveTileToR(iTile, row);
        moveTileToC(iTile, col);
    }
    const size_t iTileLast { idx1D(edge_, col) };
    moveToC(col + 1);
    if (indices_[iTileLast] == iTileLast) {
        return;
    }
    moveTileToC(iTileLast, col + 2);
    moveTileToR(iTileLast, edge_);
    moveToC(col + 1);
    moveToR(edge_ - 1);
    moveTileToR(iTileLast - side_, edge_);
    moveTileToC(iTileLast, col + 1);
    moveTileToR(iTileLast - side_, edge_ - 1);
}

auto Puzzle::solve() -> void
{
    for (size_t pin { 0 }; pin < edge_ - 1; pin += 1) {
        orderRow(pin);
        orderCol(pin, pin);
    }
    moveTileToC(nEmpty_ - 1, edge_ - 1);
    moveTileToR(nEmpty_ - 1, edge_);
    moveToC(edge_);
    moveToR(edge_);
}

auto Puzzle::trimMoves() -> void
{
    if (moves_.size() < 2) {
        return;
    }

    std::vector<size_t> mvs {};
    mvs.reserve(moves_.size());
    std::for_each(moves_.cbegin(), moves_.cend(), [&mvs](const size_t &e) {
        if (not mvs.empty() && mvs.back() == e) {
            mvs.pop_back();
        } else {
            mvs.push_back(e);
        }
    });
    moves_ = mvs;
}

auto Puzzle::getMovesShifted() const -> std::vector<int>
{
    std::vector<int> mvs;
    mvs.reserve(moves_.size());
    std::transform(moves_.cbegin(), moves_.cend(), std::back_inserter(mvs), [](auto &e) { return static_cast<int>(e + 1); });
    return mvs;
}

auto Puzzle::printMoves() const -> void
{
    fmt::print("Moves taken to solve the puzzle:\n");
    std::for_each(moves_.cbegin(), moves_.cend(), [](const auto &m) { fmt::print("{}, ", m); });
    fmt::print("\n\n");
}
auto Puzzle::printBoard() const -> void
{
    fmt::print("Current status of the board:\n");
    for (size_t row { 0 }; row < side_; row += 1) {
        for (size_t col { 0 }; col < side_; col += 1) {
            fmt::print("{}, ", board_.at(idx1D(row, col)));
        }
        fmt::print("\n");
    }
    fmt::print("\n");
}
