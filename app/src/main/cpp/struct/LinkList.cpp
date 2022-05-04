//
// Created by liuxiaobing on 2021/3/31.
//

#include "LinkList.h"


LinkList::LinkList() {

    DateTT d = {2021,03,02};

    DateTT d2;
    d2.year = 2019;
    d2.month = 10;
    d2.day = 1;

    DateTT* p = (DateTT*)&d2.month;
    p->year = 2021;
    p->month = 4;
    p->day = 3;
    d2.display();
    p->display();



}

LinkList::~LinkList() {

}


LNode *LinkList::create(int *list, int size) {

    LNode *root = new LNode(-1);
    LNode *pnode = root;

    for (int i = 0; i < size - 1; ++i) {
        root->data = list[i];//当前节点
        root->next = nullptr;

        LNode *p = new LNode(-1);//下一个结点
        p->data = list[i + 1];
        p->next = nullptr;

        root->next = p;// 连接 当前节点 与 下一个结点
        root = root->next;
    }
    root->next = nullptr;//这一句 最重要！！
    return pnode;
}

LNode *LinkList::reverseList(LNode *head) {

    LNode *preLNode = nullptr;
    LNode *curLNode = head;
    LNode *next = nullptr;
    while (curLNode->next != nullptr) {
        next = curLNode->next;
        curLNode->next = preLNode;
        preLNode = curLNode;
        curLNode = next;
    }
    return preLNode;
}

LNode *LinkList::mergeList(LNode *head1, LNode *head2) {

    if (head1 == nullptr) {
        return head2;
    }
    if (head2 == nullptr) {
        return head1;
    }

    LNode *head = new LNode(0);
    if (head1->data < head2->data) {
        head = head1;
        head1->next = mergeList(head1->next, head2);
    }
    if (head2->data < head1->data) {
        head = head2;
        head2->next = mergeList(head1, head2->next);
    }
    return head;
}
