//
// Created by Rambo.Liu on 2022/11/8.
//

#include <LogUtils.h>
#include <android/native_window_jni.h>
#include <__threading_support>
#include <unistd.h>
#include "MediaCodec.h"


#define MIN(a,b) ((a)<(b)?(a):(b))


MediaCodec* MediaCodec::pThis = nullptr;

MediaCodec::MediaCodec() {
    pThis = this;
    mFormat = nullptr;
}

MediaCodec::~MediaCodec() {
    LOGI("MediaCodec 正在析构......");
    stop();
}

uint64_t MediaCodec::getNowUs() {
    timeval tv;
    gettimeofday(&tv, 0);
    return (uint64_t)tv.tv_sec * 1000000 + (uint64_t)tv.tv_usec;
}

static uint8_t startCode[4] = {0x00,0x00,0x00,0x01};

void parseH264(uint32_t status,NALU *data) {
    if (data && MediaCodec::pThis) {
        if (status == PARSING) {
            MediaCodec::pThis->dequeueInputBuffer(data);
        } else {
            MediaCodec::pThis->isStartQueueOutBuffer = 0;
            MediaCodec::pThis->isStartQueueInBuffer = 0;
        }

    }
}

void *getMediaCodecInput(void *arg) {
    MediaCodec *mediaCodec = (MediaCodec *) arg;
    while (mediaCodec && mediaCodec->isStartQueueInBuffer) {
        ssize_t bufidx = AMediaCodec_dequeueInputBuffer(mediaCodec->mMediaCodec, -1);
        if (bufidx >= 0) {
            size_t bufsize;
            uint8_t *buf = AMediaCodec_getInputBuffer(mediaCodec->mMediaCodec, bufidx, &bufsize);
            NALU *nalu = mediaCodec->mDataQueue.pop();
            if (!nalu) {
                delete buf;
                buf = nullptr;
                LOGI("获取NALU 数据失败");
                continue;
            }
            //拷贝起始码
            memcpy(buf,startCode,4);
            memcpy(buf + 4,nalu->buf,nalu->len);
            uint64_t presentationTimeUs = mediaCodec->getNowUs();
            AMediaCodec_queueInputBuffer(mediaCodec->mMediaCodec, bufidx, 0, nalu->len + 4,
                                         presentationTimeUs, 0);
        }
    }
    LOGI("获取MediaCodec 输入队列 任务 结束");
    return 0;
}

void* getMediaCodecOutput(void *arg) {
    AMediaCodecBufferInfo info;
    size_t bufsize;
    MediaCodec *mediaCodec = (MediaCodec *) arg;
    while(mediaCodec && mediaCodec->isStartQueueOutBuffer) {
        ssize_t outindex = AMediaCodec_dequeueOutputBuffer(mediaCodec->mMediaCodec, &info, -1);
        if (outindex >= 0) {
            //获取解码好的数据
            //uint8_t* buf = AMediaCodec_getOutputBuffer(mediaCodec->mMediaCodec, outindex, &bufsize);
            AMediaCodec_releaseOutputBuffer(mediaCodec->mMediaCodec, outindex, true);
        } else if(outindex == AMEDIACODEC_INFO_OUTPUT_BUFFERS_CHANGED) {

        } else if (outindex == AMEDIACODEC_INFO_OUTPUT_FORMAT_CHANGED) {

        } else if (outindex == AMEDIACODEC_INFO_TRY_AGAIN_LATER) {
            usleep(5 * 1000);
        }
    }
    LOGI("获取MediaCodec 输出队列 任务 结束");
    return 0;
}

void MediaCodec::start(ANativeWindow* nativeWindow,const char *type, AMediaFormat *format, uint32_t flag) {
    if (!type || !format || !nativeWindow) {
        LOGE("MediaCodec 参数配置错误");
        return;
    }
    LOGI("codec type = %s",type);
    mFormat = format;
    mMediaCodec = AMediaCodec_createDecoderByType(type);
    media_status_t status = AMediaCodec_configure(mMediaCodec,format,nativeWindow,NULL, flag);
    if (status != 0) {
        LOGE("error config %d\n", status);
    }
    AMediaCodec_start(mMediaCodec);

    pthread_t aTid;
    pthread_t bTid;
    //pthread_create(&aTid, NULL, getMediaCodecInput, this);
    pthread_create(&bTid, NULL, getMediaCodecOutput, this);
    isStartQueueInBuffer = 1;
    isStartQueueOutBuffer = 1;

    //测试代码
    mH264Decodec = new H264Demux("/sdcard/ffmpeg.h264");
    mH264Decodec->onParse = parseH264;
    mH264Decodec->startDemux();
}

void MediaCodec::stop() {
    isStartQueueInBuffer = 0;
    isStartQueueOutBuffer = 0;
    if (mH264Decodec) {
        delete mH264Decodec;
        mH264Decodec = nullptr;
    }
    if (mMediaCodec) {
        AMediaCodec_stop(mMediaCodec);
        delete mMediaCodec;
        mMediaCodec = nullptr;
    }
    if (mFormat) {
        AMediaFormat_delete(mFormat);
        mFormat = nullptr;
    }
    LOGI("MediaCodec 资源释放完成!");
}

void MediaCodec::dequeueInputBuffer(NALU *data) {
    if (data) {
        ssize_t bufidx = AMediaCodec_dequeueInputBuffer(mMediaCodec, -1);
        if (bufidx >= 0) {
            size_t bufsize;
            uint8_t *buf = AMediaCodec_getInputBuffer(mMediaCodec, bufidx, &bufsize);
            int frameLen = data->len;
            if (mH264Decodec && !mH264Decodec->isContainStart(reinterpret_cast<unsigned char *>(data->buf))) {
                memcpy(buf,startCode,4);
                memcpy(buf + 4,data->buf,data->len);
                frameLen = data->len + 4;
            } else {
                memcpy(buf,data->buf,data->len);
            }
            uint64_t presentationTimeUs = getNowUs();
            AMediaCodec_queueInputBuffer(mMediaCodec, bufidx, 0, frameLen,
                                         presentationTimeUs, 0);
        }
    }
}
