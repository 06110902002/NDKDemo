//
// Created by liuxiaobing on 2021/3/17.
//

#ifndef SMALLVIDEORECORD2_VIDEOFRAME_H
#define SMALLVIDEORECORD2_VIDEOFRAME_H

#include <cstdint>

/**
 * 视频帧实体对象
 */
class VideoFrame{

public:

    VideoFrame(uint8_t* buff, int frameType, int frameLength);

    uint8_t *data;   //帧数据
    int type;        //帧类型
    int length;      //帧长度

    void setData(uint8_t *buff);

    void setType(int frameType);

    void setLength(int frameLength);
};

#endif //SMALLVIDEORECORD2_VIDEOFRAME_H
