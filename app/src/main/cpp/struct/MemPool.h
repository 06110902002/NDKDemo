//
// Created by Rambo.Liu on 2023/11/30.
//

#ifndef MEM_POOL_MEMPOOL_H
#define MEM_POOL_MEMPOOL_H

#include<iostream>
#include<vector>
#include<ctime>

using namespace std;

#define MAX_REQUEST_COUNT 10 //最大申请个数

template<class T>
class MemPool {
private:
    //内存块结构
    typedef struct BlockNode {
        /**
         * 内存块地址
         */
        void *_memory;

        BlockNode *_next;   //下一个blockNode
        size_t _objNum;     //内存块对象的个数

        /**
         * 构造函数---
         * @param num num表示申请对象的个数
         */
        BlockNode(size_t num)
                : _objNum(num),
                  _next(nullptr) {
            _memory = malloc(_objNum * _size);
        }

        ~BlockNode() {
            free(_memory);
            _memory = nullptr;
            _next = nullptr;
            _objNum = 0;
            LOGI("当前block 被释放");
        }
    } BlockNode;


protected:
    static size_t _size;            //单个对象的大小
    T *_releaseMemory = nullptr;    //释放的内存
    BlockNode *_requestMemory;      //申请的内存块
    size_t _maxNum;                 //内存块最大的大小
    size_t _useCount;               //当前内存块已经使用的对象个数
    int _sum_block;                 //内存池总块数
    pthread_mutex_t preview_mutex;
    pthread_cond_t cond;

protected:
    //设置单个对象的大小
    static size_t setSize() {
        return (sizeof(T) >= sizeof(T *) ? sizeof(T) : sizeof(T *));
    }

public:
    MemPool()
            : _useCount(0),
              _releaseMemory(nullptr),
              _maxNum(100000 * _size) {
        //开始先申请3个_size大小的空间
        _requestMemory = new BlockNode(3);
        _sum_block = 3;
        LOGI("初始申请内存块 = %d, 每块大小 = %ld\n",_sum_block,_size);
//        pthread_mutex_init(&preview_mutex, NULL);
//        pthread_cond_init(&cond, NULL);
    }

    ~MemPool() {
        BlockNode *cur = _requestMemory;
        while (cur) {
            BlockNode *del = cur;
            cur = cur->_next;
            delete del;            //会自动调用~BlockNode()
        }
//        pthread_cond_destroy(&cond);
//        pthread_mutex_destroy(&preview_mutex);
    }

    T *New() {
//        pthread_mutex_lock(&preview_mutex);
//        while (_useCount == MAX_REQUEST_COUNT) {
//            printf("88---------已经达到最大申请个数 10 暂不申请，等待回收");
//            pthread_cond_wait(&cond,&preview_mutex);
//        }
        //先在releaseMemory中找
        if (_releaseMemory) {
            LOGI("从回收池中复用上一份的数据-----\n");
            T *obj = _releaseMemory;
            _releaseMemory = *((T **) _releaseMemory);//releaseMemory的前几个字节存储的是下一个节点的地址
//            pthread_cond_signal(&cond);
//            pthread_mutex_unlock(&preview_mutex);
            return obj;
        } else {
            //判断requesetMemory中是否还有空闲内存
            if (_requestMemory->_objNum == _useCount) {
                //取物理内存中申请一块内存
                size_t size = 2 * _useCount >= _maxNum ? _maxNum : 2 * _useCount;
                _sum_block += size;
                LOGI("本次物理内存申请块数 size = %ld 总块数 _sum_block = %ld \n",size,_sum_block);
                BlockNode *newBlock = new BlockNode(size);

                newBlock->_next = _requestMemory;
                _requestMemory = newBlock;
                _useCount = 0;
            }
            //走到这里，一定有内存
            T *obj = (T *) ((char *) _requestMemory->_memory + _useCount * _size);
            LOGI("申请的内存地址 = 0x%02x \n",((char *) _requestMemory->_memory + _useCount * _size));
            _useCount++;
//            pthread_cond_signal(&cond);
//            pthread_mutex_unlock(&preview_mutex);
            return new(obj)T();//用定位new对这块空间初始化
        }
    }

    void Delete(T *obj) {
        //pthread_mutex_lock(&preview_mutex);
        if (obj) {
            obj->~T();
            *((T **) obj) = _releaseMemory;
            _releaseMemory = obj;
        }
        //pthread_mutex_unlock(&preview_mutex);
    }
};


#endif //MEM_POOL_MEMPOOL_H
