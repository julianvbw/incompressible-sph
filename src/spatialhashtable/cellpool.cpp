#include "cellpool.h"
#include <iostream>
#include <algorithm>


CellPool::CellPool(int cell_size): cell_size(cell_size) {}

Cell& CellPool::get(Handle& handle){
    if (handle.dangling){
        std::cerr << "Access using dangling handle. Undefined behaviour expected..." << std::endl;
    }
    return cells[index[handle.id]];
}

Cell& CellPool::create(Handle& handle, GridPoint p){
    if (handle.dangling) {
        auto unused = findUnused();
        if (unused == cells.end()) {
            CellId id = constructCell(p);
            handle.id = id;
            handle.dangling = false;
        } else {
            unused->point = p;
            unused->unused = false;
            handle.id = unused->id;
            handle.dangling = false;
        }
    }
    else {
        std::cerr << "Trying to create cell for valid handle. Returning old cell..." << std::endl;
    }
    return cells[index[handle.id]];
}

std::vector<Cell>::iterator CellPool::findUnused() {
    for (auto cell = cells.begin(); cell < cells.end(); ++cell){
        if (cell->unused) return cell;
    }
    return cells.end();
}

CellId CellPool::constructCell(GridPoint p) {
    CellId id = cells.size();
    index.emplace_back(id);
    cells.emplace_back(Cell(cell_size, id, p));
    return id;
}

void CellPool::z_sort() {
    std::sort(cells.begin(), cells.end());
    //update indices
    for (int i = 0; i < cells.size(); ++i)
        index[cells[i].id] = i;
}

void CellPool::resetCells() {
    for (Cell &cell: cells) cell.data.clear();
}

void CellPool::updateCells() {
    for (Cell &cell: cells) cell.unused = cell.data.empty();
}

Cell::Cell(int size, CellId id, GridPoint p) : point(p), id(id), unused(false) {
    data.reserve(size);
}

////////////////////////////////////////////////////
// Z-value compare
// src: https://en.wikipedia.org/wiki/Z-order_curve
inline
bool less_msb(unsigned x, unsigned y){
    return x < y && x < (x ^ y);
}

bool Cell::operator<(const Cell &other) const {
    if (unused) return false;

    int ind[3] = {point.i, point.j, point.k};
    int oind[3] = {other.point.i, other.point.j, other.point.k};
    int msd = 0;
    for (int dim = 0; dim < 3; ++dim)
        if (less_msb(ind[msd] ^ oind[msd], ind[dim] ^ oind[dim]))
            msd = dim;

    return ind[msd] < oind[msd];
}
////////////////////////////////////////////////////