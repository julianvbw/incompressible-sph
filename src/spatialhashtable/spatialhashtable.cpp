//
// Created by Julian on 5/31/2022.
//

#include "spatialhashtable.h"
#include <cmath>

SpatialHashTable::SpatialHashTable(int table_size, int cell_size):
        handles(table_size), pool(cell_size), table_size(table_size) {}

unsigned SpatialHashTable::hash(GridPoint p) const {
    unsigned hash = ((73856093U * p.i) ^ (19349663U * p.j) ^ (83492791U * p.k)) % table_size;
    return hash;
}

void SpatialHashTable::construct(const std::vector<QVector3D> &xs, float d) {
    pool.resetCells();
    this->d = d;

    for (int i = 0; i < xs.size(); ++i) {
        GridPoint p = positionToGrid(xs[i]);
        Handle& handle = handles[hash(p)];
        Cell& cell = (handle.dangling) ?
                     pool.create(handle, p) :
                     pool.get(handle);
        cell.data.emplace_back(i);
    }
    pool.updateCells();
    updateHandles();
    pool.z_sort();
}

GridPoint SpatialHashTable::positionToGrid(const QVector3D &x) const {
    int i, j, k;
    i = std::floor(x[0] / d);
    j = std::floor(x[1] / d);
    k = std::floor(x[2] / d);
    return {i, j, k};
}

Neighbours SpatialHashTable::neighbours(const QVector3D &x) {
    return {positionToGrid(x), *this};
}

void SpatialHashTable::updateHandles() {
    for (Handle& handle: handles){
        if (!handle.dangling){
            if (pool.get(handle).unused){
                handle.dangling = true;
            }
        }
    }
}
