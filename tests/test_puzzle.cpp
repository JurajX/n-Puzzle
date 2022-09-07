#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "resources/test_data.hpp"
#include <Puzzle.hpp>

#include <algorithm>
#include <exception>
#include <functional>

class TestSolvability : public ::testing::Test
{
protected:
    std::vector<std::vector<size_t>> solvable1D { test::solvable };
    std::vector<std::vector<size_t>> unsolvable1D { test::unsolvable };
    std::vector<std::vector<std::vector<int>>> solvable2D { { test::s3x3 }, { test::s4x4 }, { test::s5x5 } };
    std::vector<std::vector<std::vector<int>>> unsolvable2D { { test::f3x3 }, { test::f4x4 }, { test::f5x5 } };
};

TEST_F(TestSolvability, TestHasSolution)    // NOLINT
{
    auto check { [](const auto &board, bool hasSol) {
        Puzzle p { board };
        EXPECT_EQ(p.hasSolution(), hasSol);
    } };

    using namespace std::placeholders;
    std::for_each(solvable1D.cbegin(), solvable1D.cend(), [check](auto &b) { check(b, true); });
    std::for_each(solvable2D.cbegin(), solvable2D.cend(), [check](auto &b) { check(b, true); });
    std::for_each(unsolvable1D.cbegin(), unsolvable1D.cend(), [check](auto &b) { check(b, false); });
    std::for_each(unsolvable2D.cbegin(), unsolvable2D.cend(), [check](auto &b) { check(b, false); });
}

class TestMoves : public ::testing::Test
{
protected:
    Puzzle puzz { test::emptyMid25 };
    const size_t size { puzz.getSize() };
    const size_t side { puzz.getSide() };
    const size_t emptyNum { size - 1 };
    const std::vector<size_t> rows { (side - 1), (side - 1) / 2, 0, (side - 1) / 3, 0 };
    const std::vector<size_t> cols { 0, (side - 1), (side - 1) / 2, 0, (side - 1) / 3 };
};

TEST_F(TestMoves, TestMoveToC_MoveToR)    // NOLINT
{
    EXPECT_THROW(puzz.moveToC(side), std::invalid_argument);        // NOLINT
    EXPECT_THROW(puzz.moveToR(side + 4), std::invalid_argument);    // NOLINT

    for (size_t row : rows) {
        puzz.moveToR(row);
        for (size_t col : cols) {
            puzz.moveToC(col);
            EXPECT_EQ(puzz.getTileCoos(emptyNum).first, row);
            EXPECT_EQ(puzz.getTileCoos(emptyNum).second, col);
        }
    }
}

TEST_F(TestMoves, TestMoveTileToC)    // NOLINT
{
    EXPECT_THROW(puzz.moveTileToC(size, 2), std::invalid_argument);        // NOLINT
    EXPECT_THROW(puzz.moveTileToC(size + 3, 3), std::invalid_argument);    // NOLINT
    EXPECT_THROW(puzz.moveTileToC(0, side), std::invalid_argument);        // NOLINT
    EXPECT_THROW(puzz.moveTileToC(0, side + 5), std::invalid_argument);    // NOLINT

    for (size_t iTile { 0 }; iTile < size - 1; iTile += 1) {
        for (size_t col : cols) {
            puzz.moveTileToC(iTile, col);
            EXPECT_EQ(puzz.getTileCoos(iTile).second, col);
        }
    }
}

TEST_F(TestMoves, TestMoveTileToR)    // NOLINT
{
    EXPECT_THROW(puzz.moveTileToR(size, 2), std::invalid_argument);        // NOLINT
    EXPECT_THROW(puzz.moveTileToR(size + 3, 3), std::invalid_argument);    // NOLINT
    EXPECT_THROW(puzz.moveTileToR(0, side), std::invalid_argument);        // NOLINT
    EXPECT_THROW(puzz.moveTileToR(0, side + 5), std::invalid_argument);    // NOLINT

    for (size_t iTile { 0 }; iTile < size - 1; iTile += 1) {
        for (size_t row : rows) {
            puzz.moveTileToR(iTile, row);
            EXPECT_EQ(puzz.getTileCoos(iTile).first, row);
        }
    }
}

class TestOrder : public ::testing::Test
{
protected:
    Puzzle puzz { test::solvable.at(2) };
    const size_t size { puzz.getSize() };
    const size_t side { puzz.getSide() };
};

TEST_F(TestOrder, TestOrderRow)    // NOLINT
{
    EXPECT_THROW(puzz.orderRow(size), std::invalid_argument);        // NOLINT
    EXPECT_THROW(puzz.orderRow(size + 3), std::invalid_argument);    // NOLINT

    for (size_t row { 0 }; row < side - 2; row += 1) {
        puzz.orderRow(row);
        for (size_t idx { 0 }; idx < row * side + side; idx += 1) {
            EXPECT_EQ(puzz.getBoard().at(idx), idx);
        }
    }
}

TEST_F(TestOrder, TestOrderCol)    // NOLINT
{
    EXPECT_THROW(puzz.orderCol(size), std::invalid_argument);        // NOLINT
    EXPECT_THROW(puzz.orderCol(size + 3), std::invalid_argument);    // NOLINT
    EXPECT_THROW(puzz.orderCol(size), std::invalid_argument);        // NOLINT
    EXPECT_THROW(puzz.orderCol(size + 5), std::invalid_argument);    // NOLINT

    for (size_t col { 0 }; col < side - 2; col += 1) {
        puzz.orderCol(col);
        for (size_t c { 0 }; c < side - 2; c += 1) {
            for (size_t row { 0 }; row < side; row += 1) {
                size_t idx = row * side + col;
                EXPECT_EQ(puzz.getBoard().at(idx), idx);
            }
        }
    }
}

TEST_F(TestOrder, TestSolve)    // NOLINT
{
    puzz.solve();
    for (size_t row { 0 }; row < side; row += 1) {
        for (size_t col { 0 }; col < side; col += 1) {
            size_t idx = row * side + col;
            EXPECT_EQ(puzz.getBoard().at(idx), idx);
        }
    }
}

TEST_F(TestOrder, TestMoves)    // NOLINT
{
    auto [iEmpR, iEmpC] = puzz.getTileCoos(size - 1);
    puzz.moveToC(0);
    puzz.moveToC(side - 1);
    puzz.moveToC(0);
    puzz.moveToC(iEmpC);
    puzz.moveToR(0);
    puzz.moveToR(side - 1);
    puzz.moveToR(0);
    puzz.moveToR(iEmpR);
    EXPECT_FALSE(puzz.getMoves().empty());
    puzz.trimMoves();
    EXPECT_TRUE(puzz.getMoves().empty());
}

//
// =============================== Main
auto main(int argc, char **argv) -> int
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}