#include <chrono>

#include <iostream>
#include <iomanip>
#include <stdint.h>

const uint16_t DIMENSION = 3;

static const uint16_t ROWS = DIMENSION * DIMENSION;
static const uint16_t COLS = DIMENSION * DIMENSION;
static const uint16_t NUM_CELLS = DIMENSION * DIMENSION * DIMENSION * DIMENSION;

static const uint16_t MAX_WIDTH = 2;

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

const uint16_t BOX_INDEX[NUM_CELLS] =
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

// Get the number from a cell with only 1 bit set
uint16_t getNumber(uint16_t cellValue);

// Get a 1D index for a 2D cell
uint16_t getIndex(uint16_t row, uint16_t col);

// Get the top left index for a given cells box
uint16_t getBox(uint16_t row, uint16_t col);

// Set a given cell to a given value, and update all possibilities
void updateCell(uint16_t row, uint16_t col, uint16_t value);
void updateCell(uint16_t idx, uint16_t value);

void updateRow(uint16_t row, uint16_t col, uint16_t value);
void updateCol(uint16_t row, uint16_t col, uint16_t value);
void updateBox(uint16_t row, uint16_t col, uint16_t value);

// Check if a given value is valid for the given cell
bool checkCell(uint16_t row, uint16_t col, uint16_t value);

bool checkRow(uint16_t row, uint16_t col, uint16_t value);
bool checkCol(uint16_t row, uint16_t col, uint16_t value);
bool checkBox(uint16_t row, uint16_t col, uint16_t value);

// Print the grid
void printGrid();

void printCell(uint16_t cellValuee);
void printSolvedCell(uint16_t cellValue);

// 16-bit Hamming Weight
// https://stackoverflow.com/questions/9946115/hamming-weight-written-only-in-binary-operations
uint16_t popCount(uint16_t n);

int main(uint16_t argc, char* argv[])
{
	// Initialze each cell to 0b0000 0011 1111 1110 to represent all cells have all posibilities
	for (uint16_t i = 0; i < NUM_CELLS; ++i)
	{
		grid[i] = 0x03FE;
	}

	// Taken from an instance of https://www.sudoku.com/easy
	updateCell(0, 3);
	updateCell(1, 4);
	//updateCell(5, 7);
	//updateCell(7, 2);
	//updateCell(8, 6);

	updateCell(10, 2);
	updateCell(14, 4);
	//updateCell(15, 8);
	updateCell(17, 1);

	updateCell(20, 1);
	updateCell(21, 2);
	updateCell(23, 5);

	updateCell(27, 2);
	updateCell(28, 5);
	updateCell(30, 6);

	updateCell(36, 6);
	//updateCell(37, 8);
	//updateCell(39, 9);
	updateCell(42, 3);
	updateCell(43, 4);

	updateCell(46, 1);
	updateCell(48, 4);
	updateCell(50, 2);
	updateCell(53, 8);

	updateCell(54, 1);
	updateCell(55, 6);
	updateCell(58, 8);
	updateCell(59, 9);
	updateCell(61, 3);

	updateCell(63, 4);
	updateCell(65, 9);
	//updateCell(67, 2);
	updateCell(69, 6);

	updateCell(74, 8);
	updateCell(75, 7);
	//updateCell(76, 4);
	updateCell(77, 6);
	updateCell(78, 2);

	printGrid();

	auto start = std::chrono::high_resolution_clock::now();


	// Check for any cells with 1 remaining option
	bool updatedCell = true;
	while (updatedCell)
	{
		updatedCell = false;

		for (uint16_t i = 0; i < NUM_CELLS; ++i)
		{
			uint16_t value = grid[i];

			// If only 1 bit is set
			if (popCount(value) == 1)
			{
				updateCell(i, getNumber(value));
				updatedCell = true;

				//printGrid();
			}
		}
	}

	auto stop = std::chrono::high_resolution_clock::now();


	printGrid();

	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
	std::cout << "Time Taken: " << duration.count() << "us" << std::endl;



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

	return BOX_INDEX[idx];
}

// Set a given cell to a given value, and update all possibilities
void updateCell(uint16_t row, uint16_t col, uint16_t value)
{
	grid[getIndex(row, col)] = SOLVED | value;

	updateRow(row, col, value);
	updateCol(row, col, value);
	updateBox(row, col, value);
}

void updateCell(uint16_t idx, uint16_t value)
{
	uint16_t row = ROW_INDEX[idx];
	uint16_t col = COL_INDEX[idx];

	updateCell(row, col, value);
}

void updateRow(uint16_t row, uint16_t col, uint16_t value)
{
	for (uint16_t c = 0; c < COLS; ++c)
	{
		uint16_t idx = getIndex(row, c);
		// Only update un-solved cells
		if ((grid[idx] & SOLVED) == 0)
		{
			// Bitwise AND with the ones compiment of the value to switch off only that bit
			grid[idx] &= ~(NUMBERS[value]);
		}
	}
}

void updateCol(uint16_t row, uint16_t col, uint16_t value)
{
	for (uint16_t r = 0; r < ROWS; ++r)
	{
		uint16_t idx = getIndex(r, col);
		// Only update un-solved cells
		if ((grid[idx] & SOLVED) == 0)
		{
			// Bitwise AND with the ones compiment of the value to switch off only that bit
			grid[idx] &= ~(NUMBERS[value]);
		}
	}
}

void updateBox(uint16_t row, uint16_t col, uint16_t value)
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
			if ((grid[idx] & SOLVED) == 0)
			{
				// Bitwise AND with the ones compiment of the value to switch off only that bit
				grid[idx] &= ~(NUMBERS[value]);
			}
		}
	}
}


// Check if a given value is valid for the given cell
bool checkCell(uint16_t row, uint16_t col, uint16_t value)
{
	uint16_t idx = getIndex(row, col);

	// Solved Cells can not be changed
	if ((grid[idx] & SOLVED) == SOLVED)
	{
		// Make sure the cell is already the given value
		if ((grid[idx] & ~(SOLVED)) == value)
		{
			return true;
		}
		return false;
	}

	// Check if an unsolved cell is able to be the given value
	if ((grid[idx] & NUMBERS[value]) == NUMBERS[value])
	{
		return true;
	}

	return false;
}


bool checkRow(uint16_t row, uint16_t col, uint16_t value)
{
	return true;
}

bool checkCol(uint16_t row, uint16_t col, uint16_t value)
{
	return true;
}

bool checkBox(uint16_t row, uint16_t col, uint16_t value)
{
	return true;
}

// Print the grid
void printGrid()
{
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
				printCell(grid[idx]);
			}
			else
			{
				printSolvedCell(grid[idx]);
			}
		}


		std::cout << std::endl;
	}
	std::cout << std::endl;
}

void printCell(uint16_t cellValue)
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
	//std::cout << std::setw(MAX_WIDTH) << cellValue << " ";
	std::cout << std::setw(MAX_WIDTH) << "  " << " ";
}

void printSolvedCell(uint16_t cellValue)
{
	std::cout << std::setw(MAX_WIDTH) << (cellValue & ~(SOLVED)) << " ";
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