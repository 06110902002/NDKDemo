//
// Created by Rambo.Liu on 2022/7/15.
// C++ 实现一个Handler 机制  源码来源于ijkplayer 消息队列
// 本实例中对象的分配都是用的malloc 不可用delete 进行释放切记
//

#ifndef NDKDEMO_HANDLER_H
#define NDKDEMO_HANDLER_H

#include "pthread.h"


#define FFP_MSG_FLUSH                       0

/**
 * 消息队列中的消息实体
 * 定义一个结构体，顺便起别名
 */
class AVMessage {
public:
    AVMessage();

    ~AVMessage();

    int what;
    int arg1;
    int arg2;
    void *obj;
    //函数指针  用来释放 obj 对象
    void (*free_l)(void *obj);

    AVMessage *next;
};

/**
 * 消息队列
 */
class MessageQueue {
public:
    MessageQueue();

    ~MessageQueue();

    AVMessage *first_msg, *last_msg;
    int nb_messages;
    int abort_request;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    AVMessage *recycle_msg;
    int recycle_count;
    int alloc_count;
};


class Handler {

public:

    Handler();

    ~Handler();

    /**
     * 释放msg 资源
     * @param msg
     */
    void msg_free_res(AVMessage *msg);

    /**
     * 往队列存消息
     *
     * @param q    待存放在队列
     * @param msg  消息实体
     * @return     存放的结果，成功返回 1  失败 返回 0
     */
    int msg_queue_put(MessageQueue *q, AVMessage *msg);

    /**
     * 初始化 消息
     *
     * @param msg
     */
    void msg_init_msg(AVMessage *msg);

    /**
     * 往队列存消息，只带一个消息标志的，类似Handler.sendEmptyMessage
     *
     * @param q
     * @param what
     */
    void msg_queue_put_simple1(MessageQueue *q, int what);

    void msg_queue_put_simple2(MessageQueue *q, int what, int arg1);

    void msg_queue_put_simple3(MessageQueue *q, int what, int arg1, int arg2);

    static void msg_obj_free_l(void *obj);

    void
    msg_queue_put_simple4(MessageQueue *q, int what, int arg1, int arg2, void *obj, int obj_len);

    void msg_queue_init(MessageQueue *q);

    void msg_queue_flush(MessageQueue *q);

    void msg_queue_destroy(MessageQueue *q);

    void msg_queue_abort(MessageQueue *q);

    void msg_queue_start(MessageQueue *q);

    /**
     *  获取队头一条消息，block：无消息时是否阻塞 1 : 阻塞 0 ：否
     * @param q
     * @param msg
     * @param block
     * @return
     */
    int msg_queue_get(MessageQueue *q, AVMessage *msg, int block);

    void msg_queue_remove(MessageQueue *q, int what);


private:
    /**
     * 往消息队列中添加消息
     *
     * @param q    待存放在队列
     * @param msg  消息实体
     * @return     存放的结果，成功返回 1  失败 返回 0
     */
    int msg_queue_put_private(MessageQueue *q, AVMessage *msg);


};


#endif //NDKDEMO_HANDLER_H
