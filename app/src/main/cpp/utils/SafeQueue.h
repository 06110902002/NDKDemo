//
// Created by liuxiaobing on 2021/1/13.
// 注意  C++ 使用编译器 g++ 不支持 template 类、方法，在头文件声明，源文件实现的操作




#ifndef NDKDEMO_SAFEQUEUE_H
#define NDKDEMO_SAFEQUEUE_H

#include "LogUtils.h"
#include "queue"
#include "pthread.h"

using namespace std;

/**
 * 队列
 * @tparam T  元素类型
 * @tparam num  每次扩充的步长
 */
template<typename T>
class SafeQueue {

private:
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    queue<T> q;

public:
    SafeQueue();

    ~SafeQueue();

    bool push(T e);

    T pop();

    int size();

    void clear();
};


template<typename T>
SafeQueue<T>::SafeQueue() {
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

}

template<typename T>
SafeQueue<T>::~SafeQueue() {
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    clear();
}

template<typename T>
bool SafeQueue<T>::push(T e) {
    if (e) {
        pthread_mutex_lock(&mutex);
        q.push(e);
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
        return true;
    }
    return false;

}

template<typename T>
T SafeQueue<T>::pop() {
    pthread_mutex_lock(&mutex);
    while (q.empty()) {
        pthread_cond_wait(&cond,&mutex);
    }
    T head = q.front();
    q.pop();
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);

    return head;
}

template<typename T>
int SafeQueue<T>::size() {
    return q.size();

}

template<typename T>
void SafeQueue<T>::clear() {
    while(!q.empty()) {
        q.pop();
    }

}



#endif //NDKDEMO_SAFEQUEUE_H
