
#include "FrameQueue.h"
#include <malloc.h>
#include <cstring>

FrameQueue::FrameQueue(int max_size, int keep_last) {
    memset(queue, 0, sizeof(Frame) * FRAME_QUEUE_SIZE);
    this->max_size = max_size;
    this->keep_last = (keep_last != 0);
//    //为数组元素frame分配空间
//    for (int i = 0; i < this->max_size; ++i) {
//        queue[i].frame = ;
//    }
    abort_request = 1;
    rindex = 0;
    windex = 0;
    size = 0;
    show_index = 0;
}

FrameQueue::~FrameQueue() {
    for (int i = 0; i < max_size; ++i) {
        Frame *vp = &queue[i];
        unrefFrame(vp);
        delete vp;
        vp = nullptr;
    }
}

void FrameQueue::start() {
    abort_request = 0;

}

void FrameQueue::abort() {

    abort_request = 1;

}

Frame *FrameQueue::currentFrame() {
    return &queue[(rindex + show_index) % max_size];
}

Frame *FrameQueue::nextFrame() {
    return &queue[(rindex + show_index + 1) % max_size];
}

Frame *FrameQueue::lastFrame() {
    return &queue[rindex];
}

/**
 * 取出frame数组中的可写入元素的指针，取出来以后给元素赋值
 * @return
 */
Frame *FrameQueue::peekWritable() {
    return &queue[windex];
}

/**
 * 在调用了peekWritable以后，会写入元素到数组中，写入成功以后调用这个方法，代表插入成功，然后改变一些数据值
 */
void FrameQueue::pushFrame() {
    if (++windex == max_size) {
        windex = 0;
    }
    size++;
}

//代表放弃一个frame
void FrameQueue::popFrame() {
    //keep_last表示是否保持最后一个元素
    if (keep_last && !show_index) {
        show_index = 1;
        return;
    }
    unrefFrame(&queue[rindex]);
    if (++rindex == max_size) {
        rindex = 0;
    }
    size--;
}

void FrameQueue::flush() {
    while (getFrameSize() > 0) {
        popFrame();
    }
}

int FrameQueue::getFrameSize() {
    return size - show_index;
}

void FrameQueue::unrefFrame(Frame *vp) {
//    av_frame_unref(vp->frame);
//    avsubtitle_free(&vp->sub);
}

int FrameQueue::getShowIndex() const {
    return show_index;
}