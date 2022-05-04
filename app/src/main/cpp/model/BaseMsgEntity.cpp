//
// Created by liuxiaobing on 2021/3/22.
//

#include "BaseMsgEntity.h"

void BaseMsgEntity::setData(uint8_t *buff) {
    this->data = buff;
}

void BaseMsgEntity::setType(int frameType) {
    this->type = frameType;
}

void BaseMsgEntity::setLength(int frameLength) {
    this->length = frameLength;
}

BaseMsgEntity::BaseMsgEntity(uint8_t *buff, int frameType, int frameLength) {
    this->data = buff;
    this->type = frameType;
    this->length = frameLength;
}
