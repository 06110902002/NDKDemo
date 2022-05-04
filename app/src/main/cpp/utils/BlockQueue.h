//
// Created by liuxiaobing on 2021/1/19.
//

#ifndef NDKDEMO_BLOCKQUEUE_H
#define NDKDEMO_BLOCKQUEUE_H

#include <string>
#include <iostream>

using namespace std;


template<typename T>
class Node {

public:
    T data;
    Node<T> *next;
};


template<typename T>
class BlockQueue {

public:

    BlockQueue();

    ~BlockQueue();

    Node<T> *head;

    int size;

    bool add(Node<T> *e);

    Node<T> *pop();

    Node<T> *peek();


};

template<typename T>
BlockQueue<T>::BlockQueue() {

    head = new Node<T>;
    head->data = -1;
    head->next = NULL;
}

template<typename T>
BlockQueue<T>::~BlockQueue() {
    delete head;
}

template<typename T>
bool BlockQueue<T>::add(Node<T> *e) {
    if (!e) {
        return false;
    }
    Node<T> *ptemp = this->head;
    while (ptemp->next != NULL) {
        ptemp = ptemp->next;
    }
    Node<T> *pnew = new Node<T>;
    pnew->data = e->data;
    pnew->next = NULL;
    ptemp->next = pnew;
    this->size++;

    return true;
}

template<typename T>
Node<T> *BlockQueue<T>::pop() {
    if (!head || !head->next) {
        return nullptr;
    }
    Node<T> *ptemp = this->head->next;
    this->head->next = ptemp->next;
    this->size--;

    Node<T> *result = ptemp;
    free(ptemp);
    return result;
}

template<typename T>
Node<T> *BlockQueue<T>::peek() {
    if (!head || !head->next) {
        return nullptr;
    }
    Node<T> *ptemp = this->head->next;
    this->head->next = ptemp->next;
    this->size--;
    return ptemp;
}

#endif //NDKDEMO_BLOCKQUEUE_H
