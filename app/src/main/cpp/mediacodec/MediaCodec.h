//
// Created by Rambo.Liu on 2022/11/8
// NdkMediaCodec
//

#ifndef NDKDEMO_MEDIACODEC_H
#define NDKDEMO_MEDIACODEC_H

#include <SafeQueue.h>
#include "media/NdkMediaCodec.h"
#include "media/NdkMediaFormat.h"
#include "H264Demux.h"


class MediaCodec {

public:

    MediaCodec();
    ~MediaCodec();
    /**
     * 启动编/解码器
     * @param nativeWindow 渲染窗口
     * @param type     编/解码器 类型
     * @param format   配置参数
     * @param flag     标志位 flags =0，编码=MEDIACODEC_CONFIGURE_FLAG_ENCODE
     */
    void start(ANativeWindow* nativeWindow,const char* type,AMediaFormat* format,uint32_t flag);

    /**
     * 追加数据
     * @param buffer NALU 数据
     */
    void dequeueInputBuffer(NALU* buffer);


    void stop();

    int isStartQueueInBuffer = -1;
    int isStartQueueOutBuffer = -1;
    AMediaCodec*  mMediaCodec;
    SafeQueue<NALU *> mDataQueue;

    uint64_t getNowUs();

    static MediaCodec* pThis;

    H264Demux* mH264Decodec;

private:


    AMediaFormat* mFormat ;


};


#endif //NDKDEMO_MEDIACODEC_H
