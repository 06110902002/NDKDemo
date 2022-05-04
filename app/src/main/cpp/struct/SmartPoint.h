//
// Created by liuxiaobing on 2021/4/10.
//

#ifndef NDKDEMO_SMARTPOINT_H
#define NDKDEMO_SMARTPOINT_H

#include "LogUtils.h"


struct Computer{

public:
    int m_age;

    Computer(int age):m_age(age){

    }

    void test(){
        LOGV("21---------:this is smartPoint test obj：%d",m_age);
    }

    ~Computer(){
        LOGV("25-------智能指针指向的堆空间对象 Person已经正常析构 ~Computer()");
    }

};

/**
 * 智能指针的自定义实现
 * @tparam T
 */
template<typename T>
class SmartPoint {

private:
    /**
     * 本指针指向堆空间对象，当SmartPoint实例对象回收时，也回收它指向的对象
     */
    T *m_ptr;

public:

    SmartPoint(T *ptr) : m_ptr(ptr) {

    }

    /**
     * 当本对象 （注意是对象存放在栈空间）释放时，将成员m_ptr指针变量指向的堆空间对象也释放
     */
    ~SmartPoint() {
        if (m_ptr != nullptr) {
            delete m_ptr;
            m_ptr = nullptr;
        }
    }

    /**
     * 重载 -> 操作符  使用当前对象（也就是SmartPoint对象实例） 能够操作 m_ptr指向对象的成员函数
     * @return
     */
    T *operator->() {
        return m_ptr;
    }

};



#endif //NDKDEMO_SMARTPOINT_H
