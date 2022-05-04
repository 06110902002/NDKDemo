//
// Created by liuxiaobing on 2021/3/27.
//

#include <stack>
#include "Person.h"
class Cell;

#ifndef NDKDEMO_ARRAYTEST_H
#define NDKDEMO_ARRAYTEST_H



/**
 * C++ 二维数组的使用
 */
class ArrayTest {

public:
    ArrayTest();
    ~ArrayTest();

    void test();


    /**
     * 查找迷宫路径
     */
    void queryPath();

    /**
     * 将未访问的点存入栈中
     * @param x
     * @param y
     */
    void pushUnVisited(int x, int y);


    void printPath();


    void pointPassValue(Person* p);

    void refPassValue(Person& p);




private:

    /**
     * 存放未访问的栈
     */
    std::stack<Cell*> mazeStack;
    Cell* cell;

    char storeArray[5][6] = {
            '1',  '1',  '1', '1',  '1',  '1',
            '1',  '1',  '1', '0',  '0',  '1',
            '1',  '0',  '0', '0',  'e',  '1',
            '1',  '0',  '0', 's',  '1',  '1',
            '1',  '1',  '1', '1',  '1',  '1'
    };

    Cell* curCell;
    Cell* enterCell;
    Cell* exitCell;

};


#endif //NDKDEMO_ARRAYTEST_H
