//
// Created by liuxiaobing on 2021/5/12.
//

#ifndef NDKDEMO_LRUCACHE_H
#define NDKDEMO_LRUCACHE_H

#include<map>
#include <iostream>
#include "LogUtils.h"

using namespace std;

template<typename K, typename V>
struct CacheNode {
    K key;
    V value;
    CacheNode *prev, *next;
};


template<typename K, typename V>
class LruCache {

private:
    int size;


    map<K, CacheNode<K, V> *> hashmap_;
    vector<CacheNode<K, V> *> free_entries_; // 存储可用结点的地址
    CacheNode<K, V> *head_, *tail_;
    CacheNode<K, V> *entries_; // 双向链表中的结点


    // 分离结点
    void detach(CacheNode<K, V> *node) {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }

    // 将结点插入头部
    void attach(CacheNode<K, V> *node) {
        node->prev = head_;
        node->next = head_->next;
        head_->next = node;
        node->next->prev = node;
    }


public:
    LruCache(int size) {


        entries_ = new CacheNode<K, V>[size];
        for (int i = 0; i < size; ++i)// 存储可用结点的地址
            free_entries_.push_back(entries_ + i);
        head_ = new CacheNode<K, V>;
        tail_ = new CacheNode<K, V>;
        head_->prev = NULL;
        head_->next = tail_;
        tail_->prev = head_;
        tail_->next = NULL;

    }

    ~LruCache() {
        delete head_;
        delete tail_;
        delete[] entries_;
    }

    V getData(K key) {
        CacheNode<K, V> *node = hashmap_[key];
        if (node) {
            detach(node);
            attach(node);
            return node->value;
        } else {
            return nullptr;
        }

    }


    void updateCache(K key, V value) {
        CacheNode<K, V> *node = hashmap_[key];
        if (node) { // node exists
            detach(node);
            node->value = value;
            attach(node);
            LOGV("90-------命中缓存 将缓存提到队列头部");
        } else {
            if (free_entries_.empty()) {// 可用结点为空，即cache已满
                node = tail_->prev;
                detach(node);
                hashmap_.erase(node->key);
            } else {
                node = free_entries_.back();
                free_entries_.pop_back();
            }
            node->key = key;
            node->value = value;
            hashmap_[key] = node;
            attach(node);
            LOGV("90-------缓存中无值，新增一个结点插入缓存队列 尾部");
        }
    }


    bool remove(K key) {
        if (key != nullptr) {
            CacheNode<K, V> *node = hashmap_[key];
            if (node) { // node exists
                detach(node);
                hashmap_.erase(node->key);
            }
        }
        return 0;
    }

    void print() {
        CacheNode<K, V> *q = head_->next;
        while (q != tail_) {
            LOGV("123-------key = %s, value = %s", q->key, q->value);
            q = q->next;
        }
    }




//    int get(int key) {
//        map<int, CacheNode *>::iterator it = mp.find(key);
//        if (it != mp.end()) {
//            CacheNode *node = it->second;
//            remove(node);
//            setHead(node);
//            return node->value;
//        } else {
//            return -1;
//        }
//    }
//
//    void set(int key, int value) {
//        LOGV("49-----:写数据到缓存 key : %d, value = %d", key, value);
//        map<int, CacheNode *>::iterator it = mp.find(key);
//        if (it != mp.end()) {
//            CacheNode *node = it->second;
//            node->value = value;
//            remove(node);
//            setHead(node);
//        } else {
//            CacheNode *newNode = new CacheNode(key, value);
//            if (mp.size() >= size) {
//                map<int, CacheNode *>::iterator iter = mp.find(tail->key);
//                remove(tail);
//                mp.erase(iter);
//            }
//            setHead(newNode);
//            mp[key] = newNode;
//        }
//    }
//
//    void remove(CacheNode *node) {
//        if (node->pre != NULL) {
//            node->pre->next = node->next;
//        } else {
//            head = node->next;
//        }
//        if (node->next != NULL) {
//            node->next->pre = node->pre;
//        } else {
//            tail = node->pre;
//        }
//    }
//
//    void setHead(CacheNode *node) {
//        node->next = head;
//        node->pre = NULL;
//
//        if (head != NULL) {
//            head->pre = node;
//        }
//        head = node;
//        if (tail == NULL) {
//            tail = head;
//        }
//    }
};


#endif //NDKDEMO_LRUCACHE_H
