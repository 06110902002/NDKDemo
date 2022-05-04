//
// Created by liuxiaobing on 2021/3/27.
//

#include "Cell.h"

Cell::Cell() {

}

Cell::~Cell() {

}

Cell::Cell(int x, int y) {
    this->x = x;
    this->y = y;
}


bool Cell::isEqual(Cell *cell) {
    if(cell != nullptr){
        return this->x == cell->x && this->y == cell->y;
    }
    return false;
}
