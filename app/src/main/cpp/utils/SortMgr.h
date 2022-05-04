//
// Created by liuxiaobing on 2021/3/30.
//

//此种方法可以防止在一个文件中重复引入
#ifndef NDKDEMO_SORTMGR_H
#define NDKDEMO_SORTMGR_H


class SortMgr {

public:
    SortMgr();
    SortMgr(int* array);
    ~SortMgr();

    void quickSort(int a[],int low,int hight);


    //堆排序
    int* list;
    void heapSort(int* array,int size);

    void buildHeap(int size);
    void adjustHeap(int i,int size);


};


#endif //NDKDEMO_SORTMGR_H
