//
// Created by liuxiaobing on 2021/3/31.
//

#ifndef NDKDEMO_LINKLIST_H
#define NDKDEMO_LINKLIST_H
#include "LogUtils.h"

/**
 * C++ 中可以直接用struct定义一个类，与class 区别就是默认权限不同
 */
struct LNode{
    int data;
    LNode* next;
    LNode(int x) : data(x),next(nullptr){}
};

struct DateTT{
    int year;
    int month;
    int day;

    void display(){
        LOGV("20----year :%d   month :%d  day:%d",year,month,day);
    }
};


class LinkList {

public:

    LinkList();
    ~LinkList();

    /**
     * 根据数据创建链表
     * @param list
     * @return
     */
    LNode* create(int* list,int size);

    /**
     * 反转链表使用头插法
     * @param head
     * @return
     */
    LNode* reverseList(LNode* head);

    /**
     * 合并两个无序的链表为有序链表
     * @param head1
     * @param head2
     * @return
     */
    LNode* mergeList(LNode* head1, LNode* head2);

};


#endif //NDKDEMO_LINKLIST_H
