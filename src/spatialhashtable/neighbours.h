#pragma once
#include <iterator>
#include <array>
#include "common.h"

class SpatialHashTable;

struct Neighbours {
    struct Iterator;

    Neighbours(GridPoint p, SpatialHashTable& table);

    Iterator begin();

    Iterator end();

private:
    const GridPoint cp;
    SpatialHashTable& table;
};

struct Neighbours::Iterator: public std::iterator<std::input_iterator_tag, unsigned> {
    Iterator(GridPoint p, SpatialHashTable& table, int cell);

    Iterator& operator++();

    const Iterator operator++(int);

    bool operator==(const Iterator &rhs) const;

    bool operator!=(const Iterator &rhs) const;

    unsigned& operator*() const;

private:
    GridPoint currentPoint() const;

    void nextGridPoint();

    bool validGridPoint() const;

    void initCurrent();

    const GridPoint cp;
    std::array<unsigned, 27> hashes{};
    int hashes_tail;
    int cell;
    int index, size;
    SpatialHashTable& table;
};