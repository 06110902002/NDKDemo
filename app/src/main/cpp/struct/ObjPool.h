//
// Created by liuxiaobing on 2021/4/13.
//

#ifndef NDKDEMO_OBJPOOL_H
#define NDKDEMO_OBJPOOL_H

#include "queue"
#include "pthread.h"
#include "LogUtils.h"
#include "Memory.h"

using namespace std;

template<typename T>
class ObjPool {

public:

    ObjPool(int size);


    ~ObjPool();

    T *getObjFromPool();

    bool recycObj2Pool(T &obj);

private:

    /**
     * 数据列表
     */
    queue<T> m_dataList;

    /**
     * 正在使用的元素个数
     */
    int m_retainCount;

    /**
     * 最大使用个数
     */
    const int MAXSIZE = 10;

    /**
     * 信号量，线程安全使用
     */
    pthread_mutex_t m_mutex;
    pthread_cond_t m_cond;

};


template<typename T>
ObjPool<T>::ObjPool(int size) {
    pthread_mutex_init(&m_mutex, NULL);
    pthread_cond_init(&m_cond, NULL);
    m_retainCount = 0;

}

template<typename T>
ObjPool<T>::~ObjPool() {
    pthread_mutex_destroy(&m_mutex);
    pthread_cond_destroy(&m_cond);

    delete m_dataList;
}

template<typename T>
T *ObjPool<T>::getObjFromPool() {
    //pthread_mutex_lock(&m_mutex);
    while (m_retainCount >= MAXSIZE) {
        LOGV("71--------未取到可用对象，当前在用对象个数为:%d ", m_retainCount);
        pthread_cond_wait(&m_cond, &m_mutex);
    }

    if(m_dataList.empty()){
        LOGV("78-------池为空，需要新创建一个对象放入池中");
        Car car(1000,"bmw");
        m_dataList.push(car);
    }
//    T head = m_dataList.front();
//    m_dataList.pop();
//    m_retainCount++;
//    LOGV("76--------取到可用对象，当前在用对象个数为:%d", m_retainCount);
//    pthread_cond_signal(&m_cond);
//    pthread_mutex_unlock(&m_mutex);
//    return &head;
    return nullptr;
}

template<typename T>
bool ObjPool<T>::recycObj2Pool(T &obj) {
    pthread_mutex_lock(&m_mutex);
    m_dataList.push(obj);
    pthread_cond_signal(&m_cond);
    pthread_mutex_unlock(&m_mutex);
    return true;
}


#endif //NDKDEMO_OBJPOOL_H
