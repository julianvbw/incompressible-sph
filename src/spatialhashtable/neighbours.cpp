#include "neighbours.h"
#include "spatialhashtable.h"

Neighbours::Neighbours(GridPoint p, SpatialHashTable& table):
    cp(p), table(table) {}

Neighbours::Iterator Neighbours::begin() {
    return {cp, table, 0};
}

Neighbours::Iterator Neighbours::end() {
    return {cp, table, 27};
}

Neighbours::Iterator::Iterator(GridPoint p, SpatialHashTable& table, int cell) :
    cp(p), hashes_tail(0), cell(cell), index(0), size(0), table(table) {

    if (cell < 27) {
        if (validGridPoint())
            initCurrent();
        else
            nextGridPoint();
    }
}

bool Neighbours::Iterator::operator==(const Neighbours::Iterator &rhs) const {
    return (cell == 27 && rhs.cell == 27) || // end() iterator
        (cp == rhs.cp && cell == rhs.cell && index == rhs.index);
}

bool Neighbours::Iterator::operator!=(const Neighbours::Iterator &rhs) const {
    return !(rhs == *this);
}

Neighbours::Iterator &Neighbours::Iterator::operator++() {
    if (++index >= size) nextGridPoint();
    return *this;
}

const Neighbours::Iterator Neighbours::Iterator::operator++(int) {
    Neighbours::Iterator tmp(*this);
    operator++();
    return tmp;
}

unsigned &Neighbours::Iterator::operator*() const {
    GridPoint np = currentPoint();
    Handle handle = table.handles.at(table.hash(np));
    return table.pool.get(handle).data.at(index);
}

void Neighbours::Iterator::nextGridPoint() {
    while (++cell < 27 && !validGridPoint());
    if (cell < 27) initCurrent();
}

bool Neighbours::Iterator::validGridPoint() const {
    GridPoint np = currentPoint();
    if (cell >= 27) {
        return false;
    }
    else {
        unsigned h = table.hash(np);
        Handle handle = table.handles.at(h);
        return !handle.dangling &&
               std::find(hashes.begin(), hashes.begin() + hashes_tail, h) == hashes.begin() + hashes_tail;
    }
}

GridPoint Neighbours::Iterator::currentPoint() const {
    return cp + GridPoint{
        cell / 9 - 1,
        (cell % 9) / 3 - 1,
        (cell % 9) % 3 - 1
    };
}

void Neighbours::Iterator::initCurrent() {
    GridPoint np = currentPoint();
    hashes.at(hashes_tail++) = table.hash(np);
    Handle handle = table.handles.at(table.hash(np));
    size = table.pool.get(handle).data.size();
    index = 0;
}
