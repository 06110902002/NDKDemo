//
// Created by liuxiaobing on 2021/1/13.
//

#ifndef NDKDEMO_AIRCRAFT_H
#define NDKDEMO_AIRCRAFT_H


class Aircraft {


public:
    virtual void refuel();

    virtual void fly() = 0;
};

#endif //NDKDEMO_AIRCRAFT_H
