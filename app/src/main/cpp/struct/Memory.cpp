//
// Created by liuxiaobing on 2021/4/3.
//

#include <cstdlib>
#include "Memory.h"

Memory::Memory() {

    //此时因为car 对象是存储在堆空间中，所以当memory释放时并不会连同car 一起释放
    car = new Car(1234, nullptr);
    const char *constParams = "abcd";
    car->testConst(constParams);

    char *s = "78testConstFun90";
    car->testConstFun(s);


    Car *car1 = new House();
    car1->run();
    car1->color();

    //测试拷贝构造函数
    Car car2(*car);
    LOGV("21-------实现了拷贝构造函数 car2 ptr：%d  car1 price：%d", car2.m_price, car->m_price);

    //修改car2的m_price
    car2.m_price = 100;
    LOGV("22-------修改之后的价格 car2 price：%d  car1 price：%d", car2.m_price, car->m_price);


    Car car3(1000, "123");
    testPassObj(car3);

    FlvInfo* flvInfo = test();
    LOGV("36-----测试返回对象结果 :%c",flvInfo->data[0]);

}

Memory::~Memory() {

    /**
     * 析构函数需要注意的点是，当前对象的成员变量（注意为栈空间的成员变量）在对象释放时会一起释放，但如果
     * 成员变量有其他堆空间对象的引用时，
     * 释放当前对象的内存时，并不会释放当前对象中堆空间成员变量比如，memroy释放时，如果不手动释放car，
     * 是不会自动释放car 内存资源的。
     *
     * 注意：对象内部申请的堆空间由对象内部回收
     *
     * 因为car 对象是由memory 申请的内部堆空间，所以当memory释放时并不会连同car 一起释放
     * 需要在memory内部自己手动释放
     * 析构用于当前对象申请的堆空间
     *
     * 小结：delete 释放内存时  会释放当前对象所有分布在栈空间的内存
     * 只会释放当前对象在堆内存的空间，内部申请的堆空间不会释放
     *
     * 当前对象 如果是分布在堆上，那么它的成员变量都在堆上，哪怕是基本数据型
     * 通过new malloc 都可以认为是分布在堆上的
     *
     */


    //手动释放car
    if (car != nullptr) {
        delete car;
        car = nullptr; //此处不可少，如果是仅仅delete car 只是达到了删除car 指向的地址空间，但是car指针本身还是指向之前的地址空间，如果不置空就出现野指针了
    }
    LOGV("12------Memory 析构函数调用了，说明Memory对象资源释放了");
}

void Memory::recycHeapRes() {
    if (car != nullptr) {
        delete car;
        car = nullptr;
    }
}

void Memory::testLambda() {

    [] {
        LOGV("73------这是一个简易lambda表达式");
    }();
    //因为lambda实质为一个指针，所以可以使用函数指针指向它
    //方式一
    auto result = [](int v1, int v2) -> int {
        return v1 + v2;
    };
    //方式二
    int (*p2)(int, int) = [](int v1, int v2) -> int {
        return v1 + v2;
    };
    LOGV("79-------方式一:%d", result(10, 20));
    LOGV("85-------函数指针方式:%d", p2(10, 20));


    //lambda知识点二，值捕获，即在lambda表达式函数体中访问外部变量
    //方式一，直接将要访问的值在[outVar,outVar2]声明
    //方式二 [=] 这种方式会将函数体用到的外面的变量能够全部访问，如下例子
    // 默认情况下捕获均为值捕获
    int outVar = 13;
    int outVar2 = 14;
    auto test = [outVar, outVar2] {
        LOGV("91---------lambda知识点二，值捕获[outVar,outVar2]:%d  outVar2:%d", outVar, outVar2);
    };
    auto test2 = [=] {
        LOGV("91---------lambda知识点二，值捕获[=]:%d  outVar2:%d", outVar, outVar2);
    };
    //outVar 地址（引用）捕获
    //outVar2 值捕获，访问不到outVar2最新值
    auto test3 = [&outVar, outVar2] {
        LOGV("101---------lambda知识点三，地址值捕获[&修饰]能够访问最新outVar的值:%d outVar2:%d ", outVar, outVar2);
    };
    /**
     * 在lambda函数体内部修改外部变量的值，选择使用地址捕获
     */
    auto test4 = [&outVar, outVar2] {
        outVar++;
        LOGV("101---------在lambda函数体内部修改外部变量的值，选择使用地址捕获最新outVar的值:%d outVar2:%d ", outVar, outVar2);
    };
    /**
     * 在lambda函数体内部修改外部变量的值，也可以使用mutable
     * 注意mutable与地址捕获的区别
     * mutable捕获相当于，在函数 体内部重新定义一个局部变量将值赋值
     */
    auto test5 = [outVar, outVar2]() mutable {
        //int tmp = outVar;  mutable相当于这样
        //tmp ++;
        outVar++;
        outVar2++;
        LOGV("118---------在lambda函数体内部修改外部变量的值，也可以使用mutable最新outVar的值:%d outVar2:%d ", outVar, outVar2);
    };

    outVar = 100; //因为test  test2均为值捕获，所以这里修改不影响lambda内部的值
    outVar2 = 100;
    test();
    test2();
    test3();
    test4();
    test5();


    //小结注意值捕获，与地址（引用捕获）的区别

    char *m_body = "0123456789";
    char *first = m_body;
    char *fi_2 = m_body + 2;
    int len = 5;
    char *tmp = new char[len];
    memset(tmp,0,len);
    memcpy(tmp,m_body + 2,len);
    delete[] tmp;

    LOGV("141----------m_body = :%c   m_body + 2 = %c",m_body,m_body + 2);


}

void Memory::testPassObj(Car car) {
    LOGV("142----这是一个对象类型：当出作用域之后就销毁：%d", car.m_price);
}

FlvInfo *Memory::test() {

    char tmp[3] = {'9','2','c'};

    char* dataBuf = new char[0];
    memset(dataBuf,0,3);
    memcpy(dataBuf,tmp,3);

    auto tag = new FlvInfo();
    tag->tagType = 12;
    tag->timestamp = 1234567;
    tag->dataSize = 23;
    tag->codec = 7;
    tag->data = dataBuf;
    LOGV("42-------------测试返回对象");
    return tag;

}

FlvInfo::FlvInfo() {

}
