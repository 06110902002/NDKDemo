//
// Created by liuxiaobing on 2021/3/27.
//

#include "ArrayTest.h"
#include "LogUtils.h"
#include <cmath>
#include <stdio.h>
#include <ctype.h>

#include <stdio.h>
#include <stdlib.h>

#include<iostream>
#include<string>
#include<sstream>
#include "Cell.h"

ArrayTest::ArrayTest() {

    curCell = new Cell(3,3);
    exitCell = new Cell(2,4);
    enterCell = new Cell(3,3);

//    int num = 97;
//    float score = 10.00F;
//    int arr[3] = {1,2,3};
//    char v= 'c';
//    char v2 = 'c3';
//    char* cff = &v2;
//
//    int* p_num = &num;
//
//    LOGV("33-------%c  %d",*cff,*p_num);

}

ArrayTest::~ArrayTest() {

}

void ArrayTest::test() {


    /**
     * 不用指明二维数组的行数，但要指明列数,如下所示
     * 如果不指明行数，则先按列分配，数组不满列为用0填充
     *
     */
    int a1[][4] = {1,2,3,4,5,6,7,8,9};
    LOGV("35--------:a[2][3]:%d",a1[2][3]);


    /*本质上是定义了一个数组指针
     * 数组指针 https://blog.csdn.net/qq_25652139/article/details/80821857
     */
    int a21[4] = {1,2,3,4};
    int a22[4] = {5,6,7,8};
    int a23[4] = {9,2,3,4};
    int (*a2)[4] = &a21; //此处为一个指针，指向一个大小为4个int 数据的 数组
    LOGV("45------使用数组指针指向 数组首址,*a2[0]代表取第0个元素的值 :%d  a2 + 1 = :%d",*a2[0],*a2[0]+ 1);


    int (*arr)[4] = new int[3][4]{
                                    1,2, 3, 4,
                                    5,6, 7, 8,
                                    9,10,11,12
    };

    LOGV("54----数组实现二维数组：[0][3]:%d",arr[0][3]);


    /**
     * 指针数组，为一个数组，下面演示的是一个 int* 指针，指向3个元素为int*  的数组
     */
    int  var[3] = {10, 100, 200};
    int *ptr[3];
    for (int i = 0; i < 3; i++){
        ptr[i] = &var[i]; // 赋值为整数的地址
    }
    LOGV("56------指针数组 访问值的方式 :%d  ptr + 1 = :%d",*ptr[0],*ptr[1]);

}





void ArrayTest::queryPath() {

    int row = 0;
    int col = 0;
    while(!curCell->isEqual(exitCell)){
        row = curCell->x;
        col = curCell->y;
        LOGV("53--------经过的路径结点坐标 x:%d y:%d",row,col);
        if(!curCell->isEqual(enterCell)){
            //代表访问过，置成访问标识
            storeArray[row][col] = '.';

        }

        pushUnVisited(row - 1,col);
        pushUnVisited(row + 1,col);
        pushUnVisited(row,col - 1);
        pushUnVisited(row,col + 1);


        if(mazeStack.empty()){
            LOGV("96--------  无出路 寻找失败：node x:%d y:%d",row,col);
            break;
        }else{
            curCell = mazeStack.top();
            mazeStack.pop();
            LOGV("96--------  出栈 x:%d y:%d",curCell->x,curCell->y);
        }
    }

    //将最后一个结点置为走过，真实的路径可以不需要此步
    LOGV("96--------  寻找成功 x:%d y:%d",curCell->x,curCell->y);
    storeArray[curCell->x][curCell->y] = '.';
    printPath();


}

void ArrayTest::pushUnVisited(int x, int y) {

    Cell* cell = new Cell();
    cell->x = x;
    cell->y = y;

    //判断当前点是否已经访问过，只将未访问且连通的点存入栈中
    char tmp = storeArray[x][y];
    if(tmp == '0'){
        mazeStack.push(cell);
    }

}

void ArrayTest::printPath() {

    for(int i = 0; i < 5; i ++){
        for(int j = 0; j < 6; j ++){
            LOGV(" %c ",storeArray[i][j]);
        }

    }
}

void ArrayTest::pointPassValue(Person *p) {
    if(p != nullptr){
        p->age = 25;
        LOGV("154-----指针传值  修改年龄为：%d",p->age);
    }
}

void ArrayTest::refPassValue(Person &p) {
    p.age = 25;
    LOGV("154-----指针传值  修改年龄为：%d",p.age);
}
