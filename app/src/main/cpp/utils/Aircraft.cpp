//
// Created by liuxiaobing on 2021/1/13.
//

#include "Aircraft.h"
#include "../head/LogUtils.h"


// 这是一个普通虚函数，意味着基类希望子类提供自己的个性化实现代码，但基类
// 同时也提供一个缺省的虚函数实现版本，在子类不复写该虚函数的情况下作为备选方案。
void Aircraft::refuel() {

    LOGV("这是基类 refuel");
}

void Aircraft::stop() {
    LOGV("这是基类 stop");
}
