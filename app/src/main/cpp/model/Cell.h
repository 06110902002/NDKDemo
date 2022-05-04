//
// Created by liuxiaobing on 2021/3/27.
//

#ifndef NDKDEMO_CELL_H
#define NDKDEMO_CELL_H


class Cell {

public:
    Cell();
    Cell(int x,int y);
    ~Cell();

    int x;
    int y;

    bool isEqual(Cell* cell);

};


#endif //NDKDEMO_CELL_H
