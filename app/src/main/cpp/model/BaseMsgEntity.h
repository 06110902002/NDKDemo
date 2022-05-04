//
// Created by liuxiaobing on 2021/3/22.
//

#ifndef FFMPEG_BASEMSGENTITY_H
#define FFMPEG_BASEMSGENTITY_H

#include <cstdint>

/**
 * 网络通信信息实体对象
 */
class BaseMsgEntity{

public:

    BaseMsgEntity(uint8_t* buff, int frameType, int frameLength);

    uint8_t *data;   //报文实体对象
    int type;        //协议类型
    int length;      //当前报文长度

    void setData(uint8_t *buff);

    void setType(int frameType);

    void setLength(int frameLength);

};

#endif //FFMPEG_BASEMSGENTITY_H
