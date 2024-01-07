#pragma once

#include <vector>

class GameOfLife {
public:
    GameOfLife(int rows, int cols);

    int GetColumn() { return cols; }
    int GetRows() { return rows; }
    bool IsStable() { return stable; }
    int GetStableGeneration() { return stableSinceGen; }
    int GetGeneration() { return generation; }

    void evolve();
    int countLivingCells() const;
    bool isTileAlive(int row, int col) const
    {
        // Check if the coordinates are within the grid bounds
        if (row >= 0 && row < grid.size() && col >= 0 && col < grid[0].size()) {
            // Return the state of the specified tile
            return grid[row][col];
        } else {
            // Coordinates are out of bounds, consider the tile as dead
            return false;
        }
    }

    int countLiveNeighbors(int row, int col) const
    {
        int count = 0;

        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                if (i == 0 && j == 0) {
                    continue; // Skip the cell itself
                }

                int newRow = (row + i + rows) % rows;
                int newCol = (col + j + cols) % cols;

                if (grid[newRow][newCol]) {
                    ++count;
                }
            }
        }

        return count;
    }

private:
    std::vector<std::vector<bool>> grid;
    int rows;
    int cols;
    int generation = 1;
    bool stable;
    int stableSinceGen;

    void initializeRandomState()
    {
        // Seed the random number generator
        std::srand(static_cast<unsigned>(std::time(0)));

        // Randomly set cells to be alive or dead
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                grid[i][j] = (std::rand() % 2 == 0);
            }
        }
    }
};
