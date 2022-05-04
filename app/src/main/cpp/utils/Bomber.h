//
// Created by liuxiaobing on 2021/1/13.
//

#ifndef NDKDEMO_BOMBER_H
#define NDKDEMO_BOMBER_H

#include "../head/LogUtils.h"
#include "Aircraft.h"

class Bomber : public Aircraft{

    void refuel(){
        LOGV("14-----轰炸机飞加油");
    }
    void fly(){
        LOGV("17------轰炸机飞翔");
    }

public:
    void attack();

    /**
     * 定义一个函数指针p，只能指向返回值为int，形参为两个int的函数
     * @return 指向的是函数的入口
     */
    int (*p)(int, int);

    int max(int a, int b);

    int min(int a, int b);






    /**
     * 指针函数
     * 这是一个形参为两个int类型，
     * @return  返回值是int型指针的函数
     */
    int *p1(int, int);

};

#endif //NDKDEMO_BOMBER_H
