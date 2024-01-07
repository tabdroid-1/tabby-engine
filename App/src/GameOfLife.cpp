#include "GameOfLife.h"

GameOfLife::GameOfLife(int rows, int cols)
    : rows(rows)
    , cols(cols)
    , generation(0)
{
    // Initialize the grid with random initial state
    grid.resize(rows, std::vector<bool>(cols, false));
    initializeRandomState();

    stable = false;
    stableSinceGen = 0;
}

void GameOfLife::evolve()
{
    std::vector<std::vector<bool>> newGrid = grid;

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            int liveNeighbors = countLiveNeighbors(i, j);

            if (grid[i][j]) { // Cell is alive
                if (liveNeighbors != 2 && liveNeighbors != 3) {
                    // Dies of loneliness or overpopulation
                    newGrid[i][j] = false;
                }
            } else { // Cell is dead
                if (liveNeighbors == 3) {
                    // Becomes alive due to exactly 3 live neighbors
                    newGrid[i][j] = true;
                }
            }
        }
    }

    // Update the grid with the new generation

    stable = (generation > 1) && (newGrid == grid);
    grid = newGrid;
    ++generation;
}

int GameOfLife::countLivingCells() const
{
    int count = 0;
    for (const auto& row : grid) {
        for (bool cell : row) {
            if (cell) {
                ++count;
            }
        }
    }
    return count;
}
