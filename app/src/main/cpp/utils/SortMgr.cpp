//
// Created by liuxiaobing on 2021/3/30.
//

#include "SortMgr.h"
#include "LogUtils.h"


SortMgr::SortMgr() {

}

SortMgr::SortMgr(int *array) {
    this->list = array;
}

SortMgr::~SortMgr() {

}

void SortMgr::quickSort(int *a, int low, int hight) {
    if (low > hight)
        return;

    int left = low;
    int right = hight;
    int key = a[low];
    while (left < right) {

        while (left < right && key < a[right]) {
            right--;
        }
        while (left < right && key >= a[left]) {
            left++;
        }
        if (left < right) {
            int p = a[left];
            a[left] = a[right];
            a[right] = p;
        }
    }

    int p = a[left];
    a[left] = a[low];
    a[low] = p;

    quickSort(a, low, left - 1);
    quickSort(a, left + 1, hight);


}


/**
 * 堆排序
 * @param array
 * @param size
 */
void SortMgr::heapSort(int *array, int size) {
    this->list = array;
    buildHeap(size);
    for (int i = size - 1; i > 0; i--) {
        int tmp = list[0];
        list[0] = list[i];
        list[i] = tmp;
        adjustHeap(0, i);
    }

}

void SortMgr::buildHeap(int size) {
    for (int i = size / 2; i >= 0; i--) {
        adjustHeap(i, size);
    }
}

void SortMgr::adjustHeap(int i, int size) {
    int leftChild = 2 * i;
    int rightChild = 2 * i + 1;
    int tmp = i;
    if (i < size / 2) {
        if (leftChild < size && list[leftChild] >= list[tmp]) {
            tmp = leftChild;
        }
        if (rightChild < size && list[rightChild] >= list[tmp]) {
            tmp = rightChild;
        }
        if (tmp != i) {
            int t = list[tmp];
            list[tmp] = list[i];
            list[i] = t;
            adjustHeap(tmp, size);
        }
    }
}
