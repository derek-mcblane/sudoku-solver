#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <iostream>
#include <sstream>
#include <vector>

#ifdef DEBUG
#include <thread>
#endif


class Sudoku
{
    typedef int64_t value_t;
public:
    static const size_t GRID_SIZE = 9;
    static const size_t SUBGRID_DIVISIONS = 3;
    static const size_t SUBGRID_SIZE = GRID_SIZE/SUBGRID_DIVISIONS;
    static_assert((GRID_SIZE%SUBGRID_SIZE == 0), "GRID_SIZE must be divisible by SUBGRID_SIZE");

    static const value_t EMPTY = 0;

    typedef std::array<std::array<value_t, Sudoku::GRID_SIZE>, GRID_SIZE> grid_t;

    Sudoku() = default;
    Sudoku(const Sudoku&) = default;

    void solve();

    friend std::ostream& operator<<(std::ostream& stream, const Sudoku& object);
    friend std::istream& operator>>(std::istream& stream, Sudoku& object);

    std::vector<grid_t> solutions;

private:
    struct Position 
    {
        size_t row;
        size_t col;
    };

    const Position subgrid_origin(const Position& p) const;    

    bool _solve(grid_t& grid);
    bool is_value_possible(const grid_t& g, const Position& p, const value_t& v) const;
    bool is_position_in_grid(const Position& p) const;
    bool is_value_in_range(const value_t& v) const;

    grid_t values;
};

std::ostream& operator<<(std::ostream& stream, const Sudoku::grid_t& object)
{
    for (size_t row = 0; row < Sudoku::GRID_SIZE; row += 1)
    {
        stream << "\n";
        for (size_t col = 0; col < Sudoku::GRID_SIZE; col += 1)
        {
            stream << object.at(row).at(col);
        }
    }
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const Sudoku& object)
{
    stream << object.values;
    return stream;
}

std::istream& operator>>(std::istream& stream, Sudoku& object)
{
    for (size_t row = 0; row < Sudoku::GRID_SIZE; row += 1)
    {
        for (size_t col = 0; col < Sudoku::GRID_SIZE; col += 1)
        {
            stream >> object.values.at(row).at(col);
        }
    }
    return stream;
}

#ifdef DEBUG
#define SOLVE_DELAY 1000
#endif

void Sudoku::solve()
{
    bool dead_end = false;

    size_t row;
    size_t col;

    for (row = 0; !dead_end && row < Sudoku::GRID_SIZE; row += 1)
    {
        for (col = 0; !dead_end && col < Sudoku::GRID_SIZE; col += 1)
        {
            if (this->values.at(row).at(col) == EMPTY)
            {
                for (value_t guess = 1; guess <= Sudoku::GRID_SIZE; guess += 1)
                {
                    Position p = {.row=row, .col=col};
                    if (is_value_possible(this->values, p, guess))
                    {
#ifdef DEBUG
                        std::clog << "Current state: " << *this << std::endl;
                        std::this_thread::sleep_for(std::chrono::milliseconds(SOLVE_DELAY));
#endif

                        this->values.at(row).at(col) = guess;
                        solve();                                //recurse
                        this->values.at(row).at(col) = EMPTY;   //backtrack
                    }
                }
                dead_end = true; // break out of loops
            }
        }
    }

    if (!dead_end)
    {
        this->solutions.push_back(this->values);
#ifdef DEBUG
        std::cout << "Solution:\n" << this->values;
        int w; std::cin >> w;
#endif
    }

    return;
}

bool Sudoku::is_value_possible(const Sudoku::grid_t& g, const Sudoku::Position& p, const value_t& v) const
{
    assert(is_position_in_grid(p) && "Position p must be in grid");
    assert(is_value_in_range(v) && "value_t v must be in range [1,Sudoku::GRID_SIZE]");    

    bool is_value_possible = true; // assume, check if not

    for (int32_t row = 0; is_value_possible && row < Sudoku::GRID_SIZE; row +=1)
        if (g.at(row).at(p.col) == v)
            is_value_possible = false;

    for (int32_t col = 0; is_value_possible && col < Sudoku::GRID_SIZE; col += 1)
        if (g.at(p.row).at(col) == v)
            is_value_possible = false;
    
    const Sudoku::Position subgrid_origin = Sudoku::subgrid_origin(p);
    for (int32_t row = subgrid_origin.row; is_value_possible && row < subgrid_origin.row + Sudoku::SUBGRID_SIZE; row += 1)
        for (int32_t col = subgrid_origin.col; is_value_possible && col < subgrid_origin.col + Sudoku::SUBGRID_SIZE; col += 1)
            if (g.at(row).at(col) == v)
                is_value_possible = false;

    return is_value_possible;
}

const Sudoku::Position Sudoku::subgrid_origin(const Position& p) const
{
    Sudoku::Position sg_pos;
    sg_pos.row = p.row - p.row%Sudoku::SUBGRID_DIVISIONS;
    sg_pos.col = p.col - p.col%Sudoku::SUBGRID_DIVISIONS;
    return sg_pos;
}

bool Sudoku::is_position_in_grid(const Position& p) const
{
    return (p.row >= 0 && p.row < Sudoku::GRID_SIZE &&
            p.col >= 0 && p.col < Sudoku::GRID_SIZE);
}

bool Sudoku::is_value_in_range(const value_t& v) const
{
    return (v >= 1 && v <= Sudoku::GRID_SIZE);
}

int main(int argc, char* argv[])
{
    Sudoku sudoku;

    std::cin >> sudoku;

    sudoku.solve();

    std::cout << "Found " << sudoku.solutions.size() << " solutions.\n";
    std::for_each(sudoku.solutions.begin(), sudoku.solutions.end(),
        [](const Sudoku::grid_t& solution)
        {
            std::cout << solution << std::endl;
        }
    );


    return 0;
}

