//
// Created by liuxiaobing on 2021/1/13.
//

#ifndef NDKDEMO_AIRCRAFT_H
#define NDKDEMO_AIRCRAFT_H


class Aircraft {


public:
    //C++ 多态是通过虚函数实现的，所以函数前面需要需要添加virtual 修饰
    virtual void refuel();

    /**
     * 未加virtual 修饰
     */
    void stop();

    virtual void fly() = 0;
};

#endif //NDKDEMO_AIRCRAFT_H
