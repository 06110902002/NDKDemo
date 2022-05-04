//
// Created by liuxiaobing on 2021/4/3.
//

#ifndef NDKDEMO_MEMORY_H
#define NDKDEMO_MEMORY_H

#include "LogUtils.h"
#include "stdio.h"


struct FlvInfo {
    FlvInfo();
    int tagType;
    int timestamp;
    int dataSize;
    int codec;
    char *data;
};


struct Car{
    char* m_color;
    int m_price;

    Car(int price){
        m_price = price;
        LOGV("13------Car 构造函数调用了  说明car对象创建成功");
    }
    ~Car(){
        LOGV("13------Car 析构函数调用了，说明car对象资源释放了");

    }
    /**
     * 类似这种构造器中存在堆空间的引用时，需要防止指针变成野指针
     * 为了防止这种情况出现，可以在当前堆空间在申请一块空间由当前对象管理
     * 例如：外面在栈空间传进来一个字符串 "color",比如在一个函数中使用了 char* blue = {'b','l','u','e','\0'};
     * 由于blue 是在函数 中申请的，所以它分布在栈空间上，当这个函数结束时，它的空间就被回收了，
     * 这个时候有可能Car 的实例对象还在使用，而当前的成员变量 m_color 还指向着 color在栈空间的那块地址
     * 再使用时，可能就造成了野指针了，所以不管外面是如何将color的值传进来的，这里是保险的做法都是自己重新
     * 在堆上开辟一块空间，然后将color值拷贝到这块空间上，生命周期由当前对象管理
     * 这种拷贝实质为深拷贝
     * @param price
     * @param color
     */
    Car(int price = 0, const char* color = NULL){
        LOGV("36----------Car(int price = 0, const char* color = NULL):");
        if(color != nullptr){
            m_color = new char[strlen(color) + 1]{};
            strcpy(m_color,color);
        }
        m_price = price;
    }

    /**
     * 拷贝构造器
     * 1.如果没有实现拷贝构造器，相当于将 car 的对象相应值赋值给新对象
     * 2.如果实现了拷贝构造器，需要手动将car 对象的成员变量赋值给新对象
     * @param car
     */
    Car(const Car &car){
        LOGV("27----自定义Car拷贝构造函数 需要手动将car 对象的成员变量赋值给当前创建的对象：");
        m_price = car.m_price;
        m_color = car.m_color;
    }

    /**
     * 该方法没有用  virtual 修饰，当使用多态时，调用的为父类方法，而不会向下转型找子类 的run方法
     * C++ 多态的实现方法是通过virtual 实现的，请注意分析虚函数表及其实现原理
     * https://blog.csdn.net/qq_40840459/article/details/80195158
     */
    void run(){
        LOGV("45---------父类 run 方法");
    }

    virtual void color(){
        LOGV("26------父类车的颜色");
    }

    /**
     * const 修饰的形参，在调用时实参可以是const 与非const的
     * @param constParams
     */
    void testConst(const char* constParams){
        LOGV("69------testconstParams:%s",constParams);
    }

    /**
     * 有 const 修饰的成员函数（指 const 放在函数参数表的后面，而不是在函数前面或者参数表内），
     * 只能读取数据成员，不能改变数据成员；没有 const 修饰的成员函数，对数据成员则是可读可写的。
     *
     * 注意：若将成员函数声明为const，则不允许通过其修改类的数据成员。 值得注意的是，
     * 如果类中存在指针类型的数据成员即便是const函数只能保证不修改该指针的值，并不能保证不修改指针指向的对象
     *
     * @param params
     */
    void testConstFun(char* params) const {
        //m_color = params; //// 错误！不能修改m_color;
        LOGV("77------testConstFun:%s",params);
    }
};

/**
 * 如果继承的父类中没有  元参构造器时，子类的构造器必须显示的调用父类的有参构造器
 */
struct House : Car{

    House():Car(23, nullptr){

    }

    ~House(){
        LOGV("27------House 析构函数调用了，说明House对象资源释放了");

    }

    void run(){
        LOGV("45---------子类 run 方法没有用 virtual");
    }

    void color() override{
        LOGV("49---------子类车颜色");
    }
};


class Memory {

public:
    Memory();
    ~Memory();

    /**
     * 该对象会存放在堆上
     */
    Car* car;


    /**
     *   如果 Memory 使用 Memory mempry 初始化，则该对象会存放在栈上，会随着Memory的释放一起释放
     *   如果 Memory 使用 Memory* mempry = new Memory() 初始化 则house 也是存放在堆上的，会随着Memory的释放一起释放
     */
    //House house;

    /**
     * 统一回收当前对象堆空间的内存资源，本接口通常用于当前对象释放内存时调用，可放于析构函数中
     */
    void recycHeapRes();

    /**
     * 测试C++ lambda表达式
     * lambda 实质就是一个函数，形式与oc block 很像
     */
    void testLambda();


    /**
     * 此处的参数为对象类型，存放在栈上，栈上空间为临时对象
     * @param car 接受一个栈上对象
     */
    void testPassObj(Car car);

    FlvInfo* test();

};



#endif //NDKDEMO_MEMORY_H
