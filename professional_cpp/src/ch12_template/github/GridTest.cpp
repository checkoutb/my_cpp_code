
import grid;
import spreadsheet_cell;
import <vector>;
import <memory>;
import <string>;

using namespace std;

void processIntGrid(Grid<int>& /*grid*/)
{
	// Body omitted for brevity
}

/*

// g

https://github.com/carlesmartin85/procpp5e/blob/main/code/c12_code/02_Grid/02_MethodsInInterfacePartition/Grid.cppm


$ g++ -std=c++20 -fmodules-ts GridDefinition.cc GridImplementation.cc Grid.cc SpreadsheetCell.cc SpreadsheetCell.cpp GridTest.cpp
In file included from Grid.cc:4:8,
of module grid, imported at GridTest.cpp:2:
GridImplementation.cc: In instantiation of ‘Grid@grid<T>::Grid(size_t, size_t) [with T = int; size_t = long unsigned int]’:
GridTest.cpp:860574:12:   required from here
GridImplementation.cc:1011931:13: 编译器内部错误：在 tsubst_copy 中，于 cp/pt.cc:17288


*/


int main()
{
	Grid<int> myIntGrid; // declares a grid that stores ints,
						 // using default arguments for the constructor
	Grid<double> myDoubleGrid{ 11, 11 }; // declares an 11x11 Grid of doubles

	// myIntGrid.at(0, 0) = 10;
	// int x{ myIntGrid.at(0, 0).value_or(0) };
 //
	// Grid<int> grid2{ myIntGrid };  // Copy constructor
	// Grid<int> anotherIntGrid;
	// anotherIntGrid = grid2;      // Assignment operator
 //
	// //Grid test;   // WILL NOT COMPILE
	// //Grid<> test; // WILL NOT COMPILE
 //
	// processIntGrid(myIntGrid);
 //
	// Grid<SpreadsheetCell> mySpreadsheet;
	// SpreadsheetCell myCell{ 1.234 };
	// mySpreadsheet.at(3, 4) = myCell;
 //
	// Grid<const char*> myStringGrid;
	// myStringGrid.at(2, 2) = "hello";
 //
	// Grid<vector<int>> gridOfVectors;
	// vector<int> myVector{ 1, 2, 3, 4 };
	// gridOfVectors.at(5, 6) = myVector;
 //
	// auto myGridOnFreeStore { make_unique<Grid<int>>(2, 2) }; // 2x2 Grid on the free store
	// myGridOnFreeStore->at(0, 0) = 10;
	// int x2 { myGridOnFreeStore->at(0, 0).value_or(0) };
}

// Explicit class template instantiation.
// template class Grid<string>;
