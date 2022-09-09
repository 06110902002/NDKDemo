//
// Created by Rambo.Liu on 2022/7/15.
//

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <LogUtils.h>
#include <unistd.h>
#include "Handler.h"

Handler::Handler() {

}

Handler::~Handler() {
    LOGE("16------Handler 被析构");
}

void Handler::msg_free_res(AVMessage *msg) {
    if (!msg || !msg->obj)
        return;
    assert(msg->free_l);
    msg->free_l(msg->obj);
    msg->obj = NULL;
}

int Handler::msg_queue_put_private(MessageQueue *q, AVMessage *msg) {
    AVMessage *msg1;

    if (q->abort_request)
        return -1;
    msg1 = q->recycle_msg;
    if (msg1) {
        q->recycle_msg = msg1->next;
        q->recycle_count++;
    } else {
        q->alloc_count++;
        msg1 = (AVMessage*)malloc(sizeof(AVMessage));
        msg_init_msg(msg1);
    }
    if (!msg1)
        return -1;

    *msg1 = *msg;
    msg1->next = NULL;

    if (!q->last_msg)
        q->first_msg = msg1;
    else
        q->last_msg->next = msg1;
    q->last_msg = msg1;
    q->nb_messages++;
    pthread_cond_signal(&q->cond);
    return 0;
}

int Handler::msg_queue_put(MessageQueue *q, AVMessage *msg) {
    int ret;
    pthread_mutex_lock(&q->mutex);
    ret = msg_queue_put_private(q, msg);
    pthread_mutex_unlock(&q->mutex);
    return ret;
}

void Handler::msg_init_msg(AVMessage *msg) {
    memset(msg, 0, sizeof(AVMessage));
}

void Handler::msg_queue_put_simple1(MessageQueue *q, int what) {
    AVMessage msg;
    msg_init_msg(&msg);
    msg.what = what;
    msg_queue_put(q, &msg);
}

void Handler::msg_queue_put_simple2(MessageQueue *q, int what, int arg1) {
    AVMessage msg;
    msg_init_msg(&msg);
    msg.what = what;
    msg.arg1 = arg1;
    msg_queue_put(q, &msg);
}

void Handler::msg_queue_put_simple3(MessageQueue *q, int what, int arg1, int arg2) {
    AVMessage msg;
    msg_init_msg(&msg);
    msg.what = what;
    msg.arg1 = arg1;
    msg.arg2 = arg2;
    msg_queue_put(q, &msg);
}

void Handler::msg_obj_free_l(void *obj) {
    free(obj);
}

void Handler::msg_queue_put_simple4(MessageQueue *q, int what, int arg1, int arg2, void *obj,
                                    int obj_len) {
    AVMessage msg;
    msg_init_msg(&msg);
    msg.what = what;
    msg.arg1 = arg1;
    msg.arg2 = arg2;
    msg.obj = malloc(sizeof(AVMessage));
    memcpy(msg.obj, obj, obj_len);
    msg.free_l = msg_obj_free_l;
    msg_queue_put(q, &msg);
}

void Handler::msg_queue_init(MessageQueue *q) {
    memset(q, 0, sizeof(MessageQueue));
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->cond, NULL);
    q->abort_request = 1;
}

void Handler::msg_queue_flush(MessageQueue *q) {

    AVMessage *msg, *msg1;
    pthread_mutex_lock(&q->mutex);
    for (msg = q->first_msg; msg != NULL; msg = msg1) {
        msg1 = msg->next;
        msg->next = q->recycle_msg;
        q->recycle_msg = msg;
    }
    q->last_msg = NULL;
    q->first_msg = NULL;
    q->nb_messages = 0;
    pthread_mutex_unlock(&q->mutex);

}

void Handler::msg_queue_destroy(MessageQueue *q) {
    msg_queue_flush(q);
    pthread_mutex_lock(&q->mutex);
    while (q->recycle_msg) {
        AVMessage *msg = q->recycle_msg;
        if (msg)
            q->recycle_msg = msg->next;
        msg_free_res(msg); //释放msg 中引用obj 的内存
        free(msg);  //此处因为是malloc 分配的内存 不可用delete 进行释放
    }
    pthread_mutex_unlock(&q->mutex);
    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->cond);
}

void Handler::msg_queue_abort(MessageQueue *q) {
    pthread_mutex_lock(&q->mutex);
    q->abort_request = 1;
    pthread_cond_signal(&q->cond);
    pthread_mutex_unlock(&q->mutex);
}

void Handler::msg_queue_start(MessageQueue *q) {
    pthread_mutex_lock(&q->mutex);
    q->abort_request = 0;

    AVMessage msg;
    msg_init_msg(&msg);
    msg.what = FFP_MSG_FLUSH;
    msg_queue_put_private(q, &msg);
    pthread_mutex_unlock(&q->mutex);
}

int Handler::msg_queue_get(MessageQueue *q, AVMessage *msg, int block) {
    AVMessage *msg1;
    int ret;
    pthread_mutex_lock(&q->mutex);
    for (;;) {
        if (q->abort_request) {
            ret = -1;
            break;
        }

        msg1 = q->first_msg;
        if (msg1) {
            q->first_msg = msg1->next;
            if (!q->first_msg)
                q->last_msg = NULL;
            q->nb_messages--;
            *msg = *msg1;
            msg1->obj = NULL;
            msg1->next = q->recycle_msg;
            q->recycle_msg = msg1;
            ret = 1;
            break;
        } else if (!block) {
            ret = 0;
            break;
        } else {
            pthread_cond_wait(&q->cond, &q->mutex);
        }
    }
    pthread_mutex_unlock(&q->mutex);
    return ret;
}

void Handler::msg_queue_remove(MessageQueue *q, int what) {

    AVMessage **p_msg, *msg, *last_msg;
    pthread_mutex_lock(&q->mutex);
    last_msg = q->first_msg;
    if (!q->abort_request && q->first_msg) {
        p_msg = &q->first_msg;
        while (*p_msg) {
            msg = *p_msg;

            if (msg->what == what) {
                *p_msg = msg->next;
                msg_free_res(msg);
                msg->next = q->recycle_msg;
                q->recycle_msg = msg;
                q->nb_messages--;
            } else {
                last_msg = msg;
                p_msg = &msg->next;
            }
        }

        if (q->first_msg) {
            q->last_msg = last_msg;
        } else {
            q->last_msg = NULL;
        }
    }
    pthread_mutex_unlock(&q->mutex);

}


AVMessage::AVMessage() {

}

AVMessage::~AVMessage() {
    LOGE("237--------AVMessage 被析构");
}

MessageQueue::MessageQueue() {
    
}

MessageQueue::~MessageQueue() {
    LOGE("245--------MessageQueue 被析构");
}
