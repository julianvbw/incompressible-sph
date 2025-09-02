#include <vector>
#include "common.h"

struct Cell;

class CellPool {
public:
    explicit CellPool(int cell_size);

    void z_sort();

    Cell& get(Handle& handle);

    Cell& create(Handle& handle, GridPoint p);

    void resetCells();

    void updateCells();

//    void shrink();

private:
    std::vector<Cell>::iterator findUnused();

    CellId constructCell(GridPoint p);

private:
    int cell_size;
    std::vector<unsigned> index;
    std::vector<Cell> cells;
};

struct Cell {
    Cell(int size, CellId id, GridPoint p);

    bool operator< (const Cell& other) const;

    bool unused;
    GridPoint point;
    CellId id;
    std::vector<unsigned> data;
};


