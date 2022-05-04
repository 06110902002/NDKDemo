//
// Created by liuxiaobing on 2021/3/17.
//

#include "VideoFrame.h"

void VideoFrame::setData(uint8_t *buff) {
    this->data = buff;
}

void VideoFrame::setType(int frameType) {
    this->type = frameType;
}

void VideoFrame::setLength(int frameLength) {
    this->length = frameLength;
}

VideoFrame::VideoFrame(uint8_t *buff, int frameType, int frameLength) {
    this->data = buff;
    this->type = frameType;
    this->length = frameLength;
}
