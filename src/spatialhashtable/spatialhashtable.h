#pragma once

#include <vector>
#include "cellpool.h"
#include "neighbours.h"
#include "../sph/fluidscene.h"

class SpatialHashTable {
    friend struct Neighbours;

public:
    SpatialHashTable(int table_size, int cell_size);

    unsigned hash(GridPoint p) const;

    void construct(const std::vector<QVector3D> &x, float d);

    Neighbours neighbours(const QVector3D &x);

    GridPoint positionToGrid(const QVector3D &p) const;

private:
    void updateHandles();

    float d;
    const int table_size;
    std::vector<Handle> handles;
    CellPool pool;
};
