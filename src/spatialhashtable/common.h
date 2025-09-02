#pragma once

typedef unsigned CellId;

struct GridPoint {
    int i, j, k;

    GridPoint operator+(const GridPoint& rhs) const;
    bool operator==(const GridPoint& rhs) const;
};

struct Handle{
    Handle();
    bool dangling;
    CellId id;
};