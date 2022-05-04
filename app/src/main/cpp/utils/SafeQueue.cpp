/*
//
// Created by liuxiaobing on 2021/1/13.
//

#include "SafeQueue.h"

template<typename T, int num>
SafeQueue<T, num>::SafeQueue() {
    front = 0;
    rear = 0;
    arr = new T;
}

template<typename T, int num>
SafeQueue<T, num>::~SafeQueue() {
    delete[] arr;
}


template<typename T, int num>
bool SafeQueue<T, num>::isEmpty() {
    return front == rear;
}

template<typename T, int num>
bool SafeQueue<T, num>::isFull() {
    return front == num;
}


template<typename T, int num>
bool SafeQueue<T, num>::push(const T &a) {
    if (isFull()) {
        LOGV("队列已满");
        return false;
    } else {
        arr[front] = a;
        ++front;
    }
    return true;
}

template<typename T, int num>
bool SafeQueue<T, num>::pop(T &b) {
    if (isEmpty()) {
        LOGV("队列为空")
        return false;
    } else {
        b = arr[rear];
        ++rear;
    }
    return true;
}
*/
