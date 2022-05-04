//
// Created by liuxiaobing on 11/27/21.
//  主要通过调用_Unwind_Backtrace函数来获得函数的调用堆栈，但是原文的并不好用，
//  地址通过addr2line转换以后得不到函数名和行号，主要原因我们得到的地址是运行时地址，应该减去SO的基地址再来转换，下面看我改造后的例子，更好用。
//  然后使用ndk-stack 命令来定位出错方法的行号
// 打印函数调用堆栈 来定位问题

#ifndef NDKDEMO_NDKSTACKUTILS_H
#define NDKDEMO_NDKSTACKUTILS_H

#include <unwind.h>
#include <dlfcn.h>
#include <vector>
#include <string>
#include <android/log.h>


class NDKStackUtils {

public:

    explicit NDKStackUtils();
    ~NDKStackUtils();

    void callstackDump(std::string &dump);

    void callstackLogcat(int prio, const char* tag);

};


#endif //NDKDEMO_NDKSTACKUTILS_H
