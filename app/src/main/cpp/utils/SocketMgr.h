//
// Created by liuxiaobing on 2021/3/16.
//

#ifndef SMALLVIDEORECORD2_SOCKETMGR_H
#define SMALLVIDEORECORD2_SOCKETMGR_H

#include <cstdint>
#include "SafeQueue.h"
#include <jni.h>
#include <string>
#include "BaseMsgEntity.h"

using namespace std;


class SocketMgr {

public:

    ~SocketMgr();

    /**
     * 释放socket 资源
     */
    void release();

    /**
     * 连接socket
     * @param addr    ip地址
     * @param port    端口号
     * @return 连接结果
     */
    const char* connectRemote(const char* addr,const int port);

    SafeQueue<BaseMsgEntity*> msgQueue;

    pthread_t sendThreadId;

    /**
     * 接收消息的队列与线程
     */
    SafeQueue<BaseMsgEntity*> recQueue;
    pthread_t recThreadId;
    void recTask();

    /**
     * 添加发送实体入发送队列中
     *
     * @param type  在当前发送数据的报文号
     * @param size  当前报文长度
     * @param buff  当前报文大小
     */
    void addMsg(int type,int size,uint8_t* buff);

    /**
     * 发送h264帧，本函数在线程体执行
     */
    void sendTask();

    bool isConnect = false;

    /**
     * 停止发送任务
     */
    void stopSendTask();


    /**
     * int 转换 4字节 的byte，因为在c++/c中 byte 的类型即为 usigned char
     * @param i
     * @return
     */
    uint8_t* int2Bytes(int i);

    /**
     * 转码 C++ 与 java 默认中文字符集不一样，发送文本前需转换
     * @param unicode
     * @return
     */
    char* UnicodeToUtf8(const wchar_t* unicode);


};

#endif //SMALLVIDEORECORD2_SOCKETMGR_H
