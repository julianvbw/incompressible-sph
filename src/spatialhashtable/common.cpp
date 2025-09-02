//
// Created by Julian on 7/2/2022.
//

#include "common.h"

GridPoint GridPoint::operator+(const GridPoint& rhs) const{
    return {i + rhs.i, j + rhs.j, k + rhs.k};
}

bool GridPoint::operator==(const GridPoint &rhs) const {
    return i == rhs.i && j == rhs.j && k == rhs.k;
}

Handle::Handle(): dangling(true), id(0) {}
