#include <chrono>

#include <iostream>
#include <iomanip>
#include <stdint.h>

#include "qqwing.hpp"

const uint16_t DIMENSION = 3;

static const uint16_t ROWS = DIMENSION * DIMENSION;
static const uint16_t COLS = DIMENSION * DIMENSION;
static const uint16_t BOXES = DIMENSION * DIMENSION;
static const uint16_t NUM_CELLS = DIMENSION * DIMENSION * DIMENSION * DIMENSION;

// Each cell is a 16 bit number representing the state of the cell
// If Bit 16 is set, then the 8 lsbs represent the value of the cell
// Otherwise, the 10 lsbs represent the posibilites of the cell as a 1-indexed bitmap

uint16_t grid[NUM_CELLS];

// Bit-wise constants
const uint16_t SOLVED = 0x8000;

// 1-Index array of bit values for each number
const uint16_t NUMBERS[10] =
{
	0,
	0x0002,
	0x0004,
	0x0008,
	0x0010,
	0x0020,
	0x0040,
	0x0080,
	0x0100,
	0x0200
};

const uint16_t ROW_INDEX[NUM_CELLS] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 1, 1, 1, 1,
	2, 2, 2, 2, 2, 2, 2, 2, 2,
	3, 3, 3, 3, 3, 3, 3, 3, 3,
	4, 4, 4, 4, 4, 4, 4, 4, 4,
	5, 5, 5, 5, 5, 5, 5, 5, 5,
	6, 6, 6, 6, 6, 6, 6, 6, 6,
	7, 7, 7, 7, 7, 7, 7, 7, 7,
	8, 8, 8, 8, 8, 8, 8, 8, 8
};

const uint16_t COL_INDEX[NUM_CELLS] =
{
	0, 1, 2, 3, 4, 5, 6, 7, 8,
	0, 1, 2, 3, 4, 5, 6, 7, 8,
	0, 1, 2, 3, 4, 5, 6, 7, 8,
	0, 1, 2, 3, 4, 5, 6, 7, 8,
	0, 1, 2, 3, 4, 5, 6, 7, 8,
	0, 1, 2, 3, 4, 5, 6, 7, 8,
	0, 1, 2, 3, 4, 5, 6, 7, 8,
	0, 1, 2, 3, 4, 5, 6, 7, 8,
	0, 1, 2, 3, 4, 5, 6, 7, 8
};

// Index of top left of each Box
const uint16_t BOX_FIRST_INDEX[NUM_CELLS] =
{
	 0,  0,  0,  3,  3,  3,  6,  6,  6,
	 0,  0,  0,  3,  3,  3,  6,  6,  6,
	 0,  0,  0,  3,  3,  3,  6,  6,  6,
	27, 27, 27, 30, 30, 30, 33, 33, 33,
	27, 27, 27, 30, 30, 30, 33, 33, 33,
	27, 27, 27, 30, 30, 30, 33, 33, 33,
	54, 54, 54, 57, 57, 57, 60, 60, 60,
	54, 54, 54, 57, 57, 57, 60, 60, 60,
	54, 54, 54, 57, 57, 57, 60, 60, 60
};

// Index of box 0 - 8
const uint16_t BOX_INDEX[NUM_CELLS] =
{
	0, 0, 0, 1, 1, 1, 2, 2, 2,
	0, 0, 0, 1, 1, 1, 2, 2, 2,
	0, 0, 0, 1, 1, 1, 2, 2, 2,
	3, 3, 3, 4, 4, 4, 5, 5, 5,
	3, 3, 3, 4, 4, 4, 5, 5, 5,
	3, 3, 3, 4, 4, 4, 5, 5, 5,
	6, 6, 6, 7, 7, 7, 8, 8, 8,
	6, 6, 6, 7, 7, 7, 8, 8, 8,
	6, 6, 6, 7, 7, 7, 8, 8, 8
};

// Row/Col Index for Each box
const uint16_t BOX_ROW[BOXES] =
{
	0,  0,  0,
	3,  3,  3,
	6,  6,  6
};
const uint16_t BOX_COL[BOXES] =
{
	0,  3,  6,
	0,  3,  6,
	0,  3,  6
};

// Get the number from a cell with only 1 bit set
uint16_t getNumber(uint16_t cellValue);

// Get a 1D index for a 2D cell
uint16_t getIndex(uint16_t row, uint16_t col);

// Get the top left index for a given cells box
uint16_t getBox(uint16_t row, uint16_t col);

// Set a given cell to a given value, and update all possibilities
void updateCell(uint16_t row, uint16_t col, uint16_t value, bool init = false);
void updateCell(uint16_t idx, uint16_t value, bool init = false);

void updateRow(uint16_t row, uint16_t col, uint16_t value, bool init = false);
void updateCol(uint16_t row, uint16_t col, uint16_t value, bool init = false);
void updateBox(uint16_t row, uint16_t col, uint16_t value, bool init = false);

// Check all Rows/Cols/Boxes for any last possible values
bool checkCell(uint16_t idx);
bool checkCells();
bool checkRows();
bool checkCols();
bool checkBoxes();

// Get the index of the last reamining posibility in a row/col for a given value
uint16_t getLastInRow(uint16_t row, uint16_t value);
uint16_t getLastInCol(uint16_t col, uint16_t value);
uint16_t getLastInBox(uint16_t box, uint16_t value);

// Check for Naked Pairs
bool checkNakedBoxes();
bool checkNakedRows();
bool checkNakedCols();

// Print the grid
void printGrid(bool debug = false);

void printCell(uint16_t cellValuee, bool debug);
void printSolvedCell(uint16_t cellValue, bool debug);

// 16-bit Hamming Weight
// https://stackoverflow.com/questions/9946115/hamming-weight-written-only-in-binary-operations
uint16_t popCount(uint16_t n);

// Statistics
uint16_t numReads = 0;
uint16_t numWrites = 0;

int main(uint16_t argc, char* argv[])
{
	// Initialze each cell to 0b0000 0011 1111 1110 to represent all cells have all posibilities
	for (uint16_t i = 0; i < NUM_CELLS; ++i)
	{
		grid[i] = 0x03FE;
	}

	// Taken from an instance of https://qqwing.com/generate.html
	/*
	const char* sudokuString = "5..8.7..6.87.....9....5...2.9..82.14.2.1....541.6.......6.18......9.6...9..57....";
	for (int i = 0; i < qqwing::BOARD_SIZE; ++i)
	{
		if (sudokuString[i] != '.')
		{
			updateCell(i, sudokuString[i] - '0');
		}
	}
	*/

	// Use QQWING to generate random puzzle
	srand(unsigned(time(0)));
	qqwing::SudokuBoard ss;
	ss.generatePuzzle();
	const int* board = ss.getPuzzle();
	for (int i = 0; i < qqwing::BOARD_SIZE; ++i)
	{
		if (board[i] != 0)
		{
			updateCell(i, board[i], true);
		}
	}

	printGrid();

	// Reset Statistics after setup
	numReads = 0;
	numWrites = 0;
	auto start = std::chrono::high_resolution_clock::now();

	// Check for any cells/Rows/Cols/Boxes with 1 remaining option
	bool updatedCells = true;
	while (updatedCells)
	{
		updatedCells = false;

		updatedCells = updatedCells || checkCells();
		updatedCells = updatedCells || checkBoxes();
		updatedCells = updatedCells || checkRows();
		updatedCells = updatedCells || checkCols();

		// Only do Complex Checks when nescessary
		if (!updatedCells)
		{
			updatedCells = updatedCells || checkNakedRows();
			updatedCells = updatedCells || checkNakedCols();
			updatedCells = updatedCells || checkNakedBoxes();
		}



	}

	auto stop = std::chrono::high_resolution_clock::now();

	printGrid();

	// Check if grid was completed or nah
	uint16_t numFailed = 0;
	for (int i = 0; i < NUM_CELLS; ++i)
	{
		if ((grid[i] & SOLVED) == 0)
		{
			++numFailed;
		}
	}

	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
	if (numFailed > 0)
	{
		printGrid(true);
		std::cout << "FAILURE!!" << std::endl;
		std::cout << "Num Missed:" << numFailed << std::endl;
		for (int i = 0; i < NUM_CELLS; ++i)
		{
			if ((grid[i] & SOLVED) == 0)
			{
				std::cout << '.';
			}
			else
			{
				std::cout << (grid[i] & ~(SOLVED));
			}
		}
		std::cout << std::endl;
		int givenCount = ss.getGivenCount();
		int singleCount = ss.getSingleCount();
		int hiddenSingleCount = ss.getHiddenSingleCount();
		int nakedPairCount = ss.getNakedPairCount();
		int hiddenPairCount = ss.getHiddenPairCount();
		int pointingPairTripleCount = ss.getPointingPairTripleCount();
		int boxReductionCount = ss.getBoxLineReductionCount();
		int guessCount = ss.getGuessCount();
		int backtrackCount = ss.getBacktrackCount();

		std::cout << "Number of Givens: " << givenCount << std::endl;
		std::cout << "Number of Singles: " << singleCount << std::endl;
		std::cout << "Number of Hidden Singles: " << hiddenSingleCount << std::endl;
		std::cout << "Number of Naked Pairs: " << nakedPairCount << std::endl;
		std::cout << "Number of Hidden Pairs: " << hiddenPairCount << std::endl;
		std::cout << "Number of Pointing Pairs/Triples: " << pointingPairTripleCount << std::endl;
		std::cout << "Number of Box/Line Intersections: " << boxReductionCount << std::endl;
		std::cout << "Number of Guesses: " << guessCount << std::endl;
		std::cout << "Number of Backtracks: " << backtrackCount << std::endl;
	}
	else
	{
		std::cout << "SUCCESS!!" << std::endl;

	}

	std::cout << "Difficulty: " << ss.getDifficultyAsString() << std::endl;
	std::cout << "Time Taken: " << duration.count() << "us" << std::endl;
	std::cout << "Num Writes: " << numWrites << std::endl;
	std::cout << "Num Reads : " << numReads << std::endl;

	return 0;
}

// Get the number from a cell with only 1 bit set
uint16_t getNumber(uint16_t cellValue)
{
	// Loop through all 16 bits
	for (int i = 0; i < 16; ++i)
	{
		if (((cellValue >> i) & 0x1) == 1)
		{
			return i;
		}
	}

	// ERROR CASE
	return 0;
}

uint16_t getIndex(uint16_t i, uint16_t j)
{
	return (i * ROWS) + j;
}

// Get the top left index for a given cells box
uint16_t getBox(uint16_t row, uint16_t col)
{
	uint16_t idx = getIndex(row, col);

	return BOX_FIRST_INDEX[idx];
}

// Set a given cell to a given value, and update all possibilities
void updateCell(uint16_t row, uint16_t col, uint16_t value, bool init)
{
	grid[getIndex(row, col)] = SOLVED | value;
	++numWrites;

	updateRow(row, col, value, init);
	updateCol(row, col, value, init);
	updateBox(row, col, value, init);
}

void updateCell(uint16_t idx, uint16_t value, bool init)
{
	uint16_t row = ROW_INDEX[idx];
	uint16_t col = COL_INDEX[idx];

	updateCell(row, col, value, init);
}

void updateRow(uint16_t row, uint16_t col, uint16_t value, bool init)
{
	for (uint16_t c = 0; c < COLS; ++c)
	{
		uint16_t idx = getIndex(row, c);
		// Only update un-solved cells
		++numReads;
		if ((grid[idx] & SOLVED) == 0)
		{
			// Bitwise AND with the ones compiment of the value to switch off only that bit
			grid[idx] &= ~(NUMBERS[value]);
			++numWrites;

			// Check if there is now only one possible value for this cell
			if (!init)
			{
				checkCell(idx);
			}
		}
	}
}

void updateCol(uint16_t row, uint16_t col, uint16_t value, bool init)
{
	for (uint16_t r = 0; r < ROWS; ++r)
	{
		uint16_t idx = getIndex(r, col);
		// Only update un-solved cells
		++numReads;
		if ((grid[idx] & SOLVED) == 0)
		{
			// Bitwise AND with the ones compiment of the value to switch off only that bit
			grid[idx] &= ~(NUMBERS[value]);
			++numWrites;

			// Check if there is now only one possible value for this cell
			if (!init)
			{
				checkCell(idx);
			}
		}
	}
}

void updateBox(uint16_t row, uint16_t col, uint16_t value, bool init)
{
	// Get the index of the top left of this box
	uint16_t box = getBox(row, col);

	// Get the Row of the first box cell
	uint16_t startRow = ROW_INDEX[box];

	// Get the Col of the first box cell
	uint16_t startCol = COL_INDEX[box];

	for (uint16_t r = startRow; r < startRow + DIMENSION; ++r)
	{
		for (uint16_t c = startCol; c < startCol + DIMENSION; ++c)
		{
			uint16_t idx = getIndex(r, c);
			// Only update un-solved cells
			++numReads;
			if ((grid[idx] & SOLVED) == 0)
			{
				// Bitwise AND with the ones compiment of the value to switch off only that bit
				grid[idx] &= ~(NUMBERS[value]);
				++numWrites;

				// Check if there is now only one possible value for this cell
				if (!init)
				{
					checkCell(idx);
				}
			}
		}
	}
}

// Check all Rows/Cols/Boxes for any last possible values
bool checkCell(uint16_t idx)
{
	++numReads;
	uint16_t value = grid[idx];

	// Only calculate on un-solved cells
	if ((value & SOLVED) != SOLVED)
	{
		// If only 1 bit is set
		if (popCount(value) == 1)
		{
			updateCell(idx, getNumber(value));
			return true;
		}
	}

	return false;
}

bool checkCells()
{
	bool anyCellsUpdated = false;

	bool updatedCell = true;
	while (updatedCell)
	{
		updatedCell = false;

		for (uint16_t i = 0; i < NUM_CELLS; ++i)
		{
			updatedCell = checkCell(i);
			if (updatedCell)
			{
				anyCellsUpdated = true;
			}
		}
	}

	return anyCellsUpdated;
}

bool checkRows()
{
	bool anyCellsUpdated = false;
	// Check for any cells that are the only remaining option for the Row
	uint16_t last = NUM_CELLS;
	bool updatedCell = true;
	while (updatedCell)
	{
		updatedCell = false;

		// For Each Row
		for (uint16_t i = 0; i < COLS; ++i)
		{
			// For each value
			for (uint16_t val = 1; val < 10; ++val)
			{
				// If there is a single cell that is the last possibility for this value
				last = getLastInCol(i, val);
				if (last != NUM_CELLS)
				{
					updateCell(last, val);
					updatedCell = true;
					anyCellsUpdated = true;
				}
			}

		}
	}

	return anyCellsUpdated;
}

bool checkCols()
{
	bool anyCellsUpdated = false;
	// Check for any cells that are the only remaining option for the Col
	uint16_t last = NUM_CELLS;
	bool updatedCell = true;
	while (updatedCell)
	{
		updatedCell = false;

		// For Each Col
		for (uint16_t i = 0; i < COLS; ++i)
		{
			// For each value
			for (uint16_t val = 1; val < 10; ++val)
			{
				// If there is a single cell that is the last possibility for this value
				last = getLastInCol(i, val);
				if (last != NUM_CELLS)
				{
					updateCell(last, val);
					updatedCell = true;
					anyCellsUpdated = true;
				}
			}
		}
	}

	return anyCellsUpdated;
}

bool checkBoxes()
{
	bool anyCellsUpdated = false;
	// Check for any cells that are the only remaining option for the Box
	uint16_t last = NUM_CELLS;
	bool updatedCell = true;
	while (updatedCell)
	{
		updatedCell = false;

		// For Each Box
		for (uint16_t i = 0; i < BOXES; ++i)
		{
			// For each value
			for (uint16_t val = 1; val < 10; ++val)
			{
				// If there is a single cell that is the last possibility for this value
				last = getLastInBox(i, val);
				if (last != NUM_CELLS)
				{
					updateCell(last, val);
					updatedCell = true;
					anyCellsUpdated = true;
				}
			}
		}
	}

	return anyCellsUpdated;
}

uint16_t getLastInRow(uint16_t row, uint16_t value)
{
	uint16_t count = 0;
	uint16_t index = NUM_CELLS;

	for (uint16_t c = 0; c < COLS; ++c)
	{
		uint16_t idx = getIndex(row, c);

		// Check if a solved cell matches the value
		++numReads;
		uint16_t gridValue = grid[idx];

		if ((gridValue & SOLVED) == SOLVED)
		{
			if ((gridValue & ~(SOLVED)) == value)
			{
				return NUM_CELLS;
			}
		}
		// Check if an un-solved cell is able to be that value
		else
		{
			if ((gridValue & NUMBERS[value]) == NUMBERS[value])
			{
				index = idx;
				++count;
			}
		}
	}

	// If there was only one possibility found
	if (count == 1)
	{
		return index;
	}

	return NUM_CELLS;
}

uint16_t getLastInCol(uint16_t col, uint16_t value)
{
	uint16_t count = 0;
	uint16_t index = NUM_CELLS;

	for (uint16_t r = 0; r < ROWS; ++r)
	{
		uint16_t idx = getIndex(r, col);

		// Check if a solved cell matches the value
		++numReads;
		uint16_t gridValue = grid[idx];

		if ((gridValue & SOLVED) == SOLVED)
		{
			if ((gridValue & ~(SOLVED)) == value)
			{
				return NUM_CELLS;
			}
		}
		// Check if an un-solved cell is able to be that value
		else
		{
			if ((gridValue & NUMBERS[value]) == NUMBERS[value])
			{
				index = idx;
				++count;
			}
		}
	}

	// If there was only one possibility found
	if (count == 1)
	{
		return index;
	}

	return NUM_CELLS;
}

uint16_t getLastInBox(uint16_t box, uint16_t value)
{
	uint16_t count = 0;
	uint16_t index = NUM_CELLS;

	// Get the Row of the first box cell
	uint16_t startRow = BOX_ROW[box];

	// Get the Col of the first box cell
	uint16_t startCol = BOX_COL[box];

	for (uint16_t r = startRow; r < startRow + DIMENSION; ++r)
	{
		for (uint16_t c = startCol; c < startCol + DIMENSION; ++c)
		{
			uint16_t idx = getIndex(r, c);

			// Check if a solved cell matches the value
			++numReads;
			uint16_t gridValue = grid[idx];

			if ((gridValue & SOLVED) == SOLVED)
			{
				if ((gridValue & ~(SOLVED)) == value)
				{
					return NUM_CELLS;
				}
			}
			// Check if an un-solved cell is able to be that value
			else
			{
				if ((gridValue & NUMBERS[value]) == NUMBERS[value])
				{
					index = idx;
					++count;
				}
			}
		}
	}

	// If there was only one possibility found
	if (count == 1)
	{
		return index;
	}

	return NUM_CELLS;
}

// Check for Naked Pairs
bool checkNakedBoxes()
{
	bool anyCellsUpdated = false;

	// For Each Box
	for (uint16_t b = 0; b < BOXES; ++b)
	{
		// Get the Row of the first box cell
		uint16_t startRow = BOX_ROW[b];

		// Get the Col of the first box cell
		uint16_t startCol = BOX_COL[b];

		// For each value
		for (uint16_t val = 1; val < 10; ++val)
		{
			uint16_t lastPossibleRow = ROWS;
			bool sameRow = false;
			bool diffRows = false;

			uint16_t lastPossibleCol = COLS;
			bool sameCol = false;
			bool diffCols = false;


			//  For Each cell in Box
			for (uint16_t r = startRow; r < startRow + DIMENSION; ++r)
			{
				for (uint16_t c = startCol; c < startCol + DIMENSION; ++c)
				{
					uint16_t idx = getIndex(r, c);

					// Check if a solved cell matches the value
					++numReads;
					uint16_t gridValue = grid[idx];

					// If the Cell if Solved
					if ((gridValue & SOLVED) == SOLVED)
					{
						// If the Cell is solved AND the value to check
						if ((gridValue & ~(SOLVED)) == val)
						{
							// Break out of the "For Each Cell" Loop
							r = NUM_CELLS;
							c = NUM_CELLS;
							sameRow = false;
						}
					}
					// Check if an un-solved cell is able to be that value
					else
					{
						if ((gridValue & NUMBERS[val]) == NUMBERS[val])
						{
							// Check Rows
							if (lastPossibleRow == ROWS)
							{
								lastPossibleRow = r;
							}
							// If the remaining possibilities for this val are on different rows
							else if (r != lastPossibleRow)
							{
								diffRows = true;
							}
							else
							{
								sameRow = true;
							}

							// Check Cols
							if (lastPossibleCol == COLS)
							{
								lastPossibleCol = c;
							}
							// If the remaining possibilities for this val are in different Cols
							else if (c != lastPossibleCol)
							{
								diffCols = true;
							}
							else
							{
								sameCol = true;
							}
						}

					} // End If Solved
				} // End For Col
			} // End For Row

			// Check if we found a value on the same row
			if (sameRow && !diffRows)
			{
				// Update all other cells in this row (in other boxes)
				for (uint16_t c = 0; c < COLS; ++c)
				{
					uint16_t idx = getIndex(lastPossibleRow, c);

					// SKip over this box
					if (BOX_INDEX[idx] != b)
					{
						// Only update un-solved cells
						++numReads;
						uint16_t gridValue = grid[idx];
						if ((gridValue & SOLVED) == 0 &&
							(gridValue & NUMBERS[val]) == NUMBERS[val])
						{
							// Bitwise AND with the ones compiment of the value to switch off only that bit
							grid[idx] &= ~(NUMBERS[val]);
							++numWrites;
							anyCellsUpdated = true;

							// Check if there is now only one possible value for this cell
							checkCell(idx);
						}
					}
				}
			} // End If Same Row
						// Check if we found a value on the same col
			if (sameCol && !diffCols)
			{
				// Update all other cells in this Col (in other boxes)
				for (uint16_t r = 0; r < ROWS; ++r)
				{
					uint16_t idx = getIndex(r, lastPossibleCol);

					// SKip over this box
					if (BOX_INDEX[idx] != b)
					{
						// Only update un-solved cells
						++numReads;
						uint16_t gridValue = grid[idx];
						if ((gridValue & SOLVED) == 0 &&
							(gridValue & NUMBERS[val]) == NUMBERS[val])
						{
							// Bitwise AND with the ones compiment of the value to switch off only that bit
							grid[idx] &= ~(NUMBERS[val]);
							++numWrites;
							anyCellsUpdated = true;

							// Check if there is now only one possible value for this cell
							checkCell(idx);
						}
					}
				}
			} // End If Same Col
		} // End for Val
	} // End For Box

	return anyCellsUpdated;
}

bool checkNakedRows()
{
	bool anyCellsUpdated = false;

	for (uint16_t r = 0; r < ROWS; ++r)
	{
		// For each value
		for (uint16_t val = 1; val < 10; ++val)
		{

			uint16_t lastPossibleBox = BOXES;
			bool sameBox = false;
			bool diffBoxes = false;

			for (uint16_t c = 0; c < COLS; ++c)
			{
				uint16_t idx = getIndex(r, c);

				// Check if a solved cell matches the value
				++numReads;
				uint16_t gridValue = grid[idx];

				// If the Cell if Solved
				if ((gridValue & SOLVED) == SOLVED)
				{
					// If the Cell is solved AND the value to check
					if ((gridValue & ~(SOLVED)) == val)
					{
						// Break out of the "For Each Cell" Loop
						c = NUM_CELLS;
						sameBox = false;
					}
				}
				// Check if an un-solved cell is able to be that value
				else
				{
					if ((gridValue & NUMBERS[val]) == NUMBERS[val])
					{
						// Check Rows
						if (lastPossibleBox == BOXES)
						{
							lastPossibleBox = BOX_INDEX[idx];
						}
						// If the remaining possibilities for this val are on different rows
						else if (BOX_INDEX[idx] != lastPossibleBox)
						{
							diffBoxes = true;
						}
						else
						{
							sameBox = true;
						}
					}
				}
			}

			if (sameBox && !diffBoxes)
			{
				// Get the Row of the first box cell
				uint16_t startRow = BOX_ROW[lastPossibleBox];

				// Get the Col of the first box cell
				uint16_t startCol = BOX_COL[lastPossibleBox];

				for (uint16_t rB = 0; rB < DIMENSION; ++rB)
				{
					for (uint16_t cB = 0; cB < DIMENSION; ++cB)
					{
						// Skip over this row
						if (rB != r)
						{
							uint16_t idxB = getIndex(rB, cB);

							// Only update un-solved cells
							++numReads;
							uint16_t gridValue = grid[idxB];
							if ((gridValue & SOLVED) == 0 &&
								(gridValue & NUMBERS[val]) == NUMBERS[val])
							{
								// Bitwise AND with the ones compiment of the value to switch off only that bit
								grid[idxB] &= ~(NUMBERS[val]);
								++numWrites;
								anyCellsUpdated = true;

								// Check if there is now only one possible value for this cell
								checkCell(idxB);
							}
						}
					}
				}

			}
		}
	}

	return anyCellsUpdated;			
}

bool checkNakedCols()
{
	bool anyCellsUpdated = false;

	for (uint16_t c = 0; c < COLS; ++c)
	{
		// For each value
		for (uint16_t val = 1; val < 10; ++val)
		{

			uint16_t lastPossibleBox = BOXES;
			bool sameBox = false;
			bool diffBoxes = false;

			for (uint16_t r = 0; r < ROWS; ++r)
			{
				uint16_t idx = getIndex(r, c);

				// Check if a solved cell matches the value
				++numReads;
				uint16_t gridValue = grid[idx];

				// If the Cell if Solved
				if ((gridValue & SOLVED) == SOLVED)
				{
					// If the Cell is solved AND the value to check
					if ((gridValue & ~(SOLVED)) == val)
					{
						// Break out of the "For Each Cell" Loop
						c = NUM_CELLS;
						sameBox = false;
					}
				}
				// Check if an un-solved cell is able to be that value
				else
				{
					if ((gridValue & NUMBERS[val]) == NUMBERS[val])
					{
						// Check Rows
						if (lastPossibleBox == BOXES)
						{
							lastPossibleBox = BOX_INDEX[idx];
						}
						// If the remaining possibilities for this val are on different rows
						else if (BOX_INDEX[idx] != lastPossibleBox)
						{
							diffBoxes = true;
						}
						else
						{
							sameBox = true;
						}
					}
				}
			}

			if (sameBox && !diffBoxes)
			{
				// Get the Row of the first box cell
				uint16_t startRow = BOX_ROW[lastPossibleBox];

				// Get the Col of the first box cell
				uint16_t startCol = BOX_COL[lastPossibleBox];

				for (uint16_t rB = 0; rB < DIMENSION; ++rB)
				{
					for (uint16_t cB = 0; cB < DIMENSION; ++cB)
					{
						// Skip over this col
						if (cB != c)
						{
							uint16_t idxB = getIndex(rB, cB);

							// Only update un-solved cells
							++numReads;
							uint16_t gridValue = grid[idxB];
							if ((gridValue & SOLVED) == 0 &&
								(gridValue & NUMBERS[val]) == NUMBERS[val])
							{
								// Bitwise AND with the ones compiment of the value to switch off only that bit
								grid[idxB] &= ~(NUMBERS[val]);
								++numWrites;
								anyCellsUpdated = true;

								// Check if there is now only one possible value for this cell
								checkCell(idxB);
							}
						}
					}
				}

			}
		}
	}

	return anyCellsUpdated;
}

// Print the grid
void printGrid(bool debug)
{
	uint16_t MAX_WIDTH = 2;

	if (debug) MAX_WIDTH = 4;

	std::cout << std::endl;

	for (uint16_t r = 0; r < ROWS; ++r)
	{
		if (r % DIMENSION == 0 && r != 0)
		{

			for (uint16_t i = 0; i < (ROWS * MAX_WIDTH) + ROWS + DIMENSION; ++i)
			{
				std::cout << "-";
			}

			std::cout << std::endl;
		}

		for (uint16_t c = 0; c < COLS; ++c)
		{
			if (c % DIMENSION == 0 && c != 0)
			{
				std::cout << "|";
			}

			uint16_t idx = getIndex(r, c);
			// Only update un-solved cells
			if ((grid[idx] & SOLVED) == 0)
			{
				printCell(grid[idx], debug);
			}
			else
			{
				printSolvedCell(grid[idx], debug);
			}
		}


		std::cout << std::endl;
	}
	std::cout << std::endl;
}

void printCell(uint16_t cellValue, bool debug)
{
	/*
	uint16_t num = 1;
	for (uint16_t r = 0; r < DIMENSION; ++r)
	{
		for (uint16_t c = 0; c < DIMENSION; ++c)
		{
			if ((cellValue & NUMBERS[num]) == NUMBERS[num])
			{
				std::cout << num << ' ';
			}
		}

	}
	*/
	if (debug) std::cout << std::setw(4) << cellValue << " ";
	else std::cout << std::setw(2) << "  " << " ";
}

void printSolvedCell(uint16_t cellValue, bool debug)
{
	if (debug) std::cout << std::setw(4) << (cellValue & ~(SOLVED)) << " ";
	else std::cout << std::setw(2) << (cellValue & ~(SOLVED)) << " ";
}

// 16-bit Hamming Weight
// https://stackoverflow.com/questions/9946115/hamming-weight-written-only-in-binary-operations
uint16_t popCount(uint16_t n)
{
	// each bit in n is a one-bit integer that indicates how many bits are set
	// in that bit.

	n = ((n & 0xAAAAAAAA) >> 1) + (n & 0x55555555);

	// Now every two bits are a two bit integer that indicate how many bits were
	// set in those two bits in the original number

	n = ((n & 0xCCCCCCCC) >> 2) + (n & 0x33333333);
	// Now we're at 4 bits

	n = ((n & 0xF0F0F0F0) >> 4) + (n & 0x0F0F0F0F);
	// 8 bits

	n = ((n & 0xFF00FF00) >> 8) + (n & 0x00FF00FF);
	// 16 bits

	return n;
}