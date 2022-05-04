//
// Created by liuxiaobing on 2021/3/16.
//

#include "SocketMgr.h"
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "LogUtils.h"

#include <cstdio>
#include <stdlib.h>
#include <locale.h>
#include <string>
#if defined(_WIN32) || defined(_WIN64)
#include <wtypes.h>
#endif

#define MAXSIZE 4096
struct sockaddr_in sock_add;
struct sockaddr_in server_addr;
int sockfd = 0;








using namespace std;

//gbk字符串srcStr 转换为utf8字符串utfStr ，字符串utfStr的缓存最大大小 maxUtfStrlen
//失败返回-1,成功返回大于0 ，maxUtfStrlen的大小至少是源字符串有效长度大小2倍加1
inline int gbk2utf8(char *utfStr,size_t maxUtfStrlen,const char *srcStr)
{
    if(!srcStr||!utfStr)
    {
        printf("Bad Parameter\n");
        return -1;
    }
#if defined(_WIN32) || defined(_WIN64)
    int len = MultiByteToWideChar(CP_ACP, 0, (LPCCH)srcStr, -1, NULL,0);
	unsigned short * strUnicode = new unsigned short[len+1];
	memset(strUnicode, 0, len * 2 + 2);
	MultiByteToWideChar(CP_ACP, 0, (LPCCH)srcStr, -1, (LPWSTR)strUnicode, len);
	len = WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)strUnicode, -1, NULL, 0, NULL, NULL);
	if (len > (int)maxUtfStrlen)
	{
		printf("Dst Str memory not enough\n");
		delete[] strUnicode;
		return -1;
	}
	WideCharToMultiByte (CP_UTF8, 0, (LPWSTR)strUnicode, -1, utfStr, len, NULL,NULL);
	delete[] strUnicode;
	return len;
#else//linux
    //首先先将gbk编码转换为unicode编码
    if(NULL==setlocale(LC_ALL,"zh_CN.gbk"))//设置转换为unicode前的码,当前为gbk编码
    {
        LOGV("66-----参数有错误\n");
        return -1;
    }

    int unicodeLen=mbstowcs(NULL,srcStr,0);//计算转换后的长度
    if(unicodeLen<=0)
    {
        LOGV("73-------不能转换!!!unicodeLen:(%d)\n",unicodeLen);
        return -1;
    }
    wchar_t *unicodeStr=(wchar_t *)calloc(sizeof(wchar_t),unicodeLen+1);
    mbstowcs(unicodeStr,srcStr,strlen(srcStr));//将gbk转换为unicode
    //将unicode编码转换为utf8编码
    if(NULL==setlocale(LC_ALL,"zh_CN.utf8"))//设置unicode转换后的码,当前为utf8
    {
        LOGV("81-------参数有错误\n");
        free(unicodeStr);
        return -1;
    }
    int utfLen=wcstombs(NULL,unicodeStr,0);//计算转换后的长度
    if(utfLen<=0)
    {
        LOGV("88--------不能转换!!!utfLen:(%d)\n",utfLen);
        free(unicodeStr);
        return -1;
    }
    else if(utfLen>=(int)maxUtfStrlen)//判断空间是否足够
    {
        LOGV("94-----Dst Str memory not enough\n");
        free(unicodeStr);
        return -1;
    }
    wcstombs(utfStr,unicodeStr,utfLen);
    utfStr[utfLen]=0;//添加结束符
    free(unicodeStr);
    return utfLen;
#endif
}
//gbk字符串srcStr 转换为utf8字符串target
inline int gbk2utf8(std::string& target,const char *srcStr)
{
    if (!srcStr)
    {
        assert(false&&"string is empty");
    }
    int tarLen = (int)strlen(srcStr) * 2 + 1;
    char *tarStr = new char[tarLen];
    gbk2utf8(tarStr,tarLen - 1,srcStr);
    target = tarStr;
    delete []tarStr;
    return tarLen;
}


/**
 * 发送数据的线程回调函数
 * @param args
 * @return
 */
inline static void *startSendTask(void *args) {
    auto *socketMgr = (SocketMgr *) args;
    if (socketMgr) {
        socketMgr->sendTask();
    }
    return nullptr;
}

inline static void *startRecTask(void *args) {
    auto *socketMgr = (SocketMgr *) args;
    if (socketMgr) {
        socketMgr->recTask();
    }
    return nullptr;
}

SocketMgr::~SocketMgr() {
    release();
}

void SocketMgr::release() {
    close(sockfd);
    isConnect = false;
    msgQueue.clear();
}

const char *SocketMgr::connectRemote(const char *addr, const int port) {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);    //ipv4,TCP数据连接
    if (sockfd < 0) {
        LOGV("初始化socket 错误，程序终止");
        return "socket error";
    }
    //remote server address
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, addr, &server_addr.sin_addr) < 0) {    //set ip address
        LOGV("set ip address，程序终止");
        return "address error";
    }
    socklen_t server_addr_length = sizeof(server_addr);
    int connfd = connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)); //连接到服务器
    if (connfd < 0) {
        isConnect = false;
        LOGV(" socket connect error，程序终止!!!");
        return "connect error";
    }
    isConnect = true;

    pthread_create(&sendThreadId, nullptr, startSendTask, this);
    pthread_create(&recThreadId, nullptr, startRecTask, this);

    return "SUCCESS";
}

void SocketMgr::sendTask() {
    while (isConnect) {
        int i = 0;
        while (msgQueue.size() != 0) {
            //LOGV("ffmpeg 编码好h264帧，开始发送");
            BaseMsgEntity *msgEntity = msgQueue.pop();
            ;


            //发送4字节协议号
            unsigned char cType[4] = {0};
            memcpy(cType, int2Bytes(msgEntity->type), 4);

            uint8_t *type = cType;
            int sendResult = send(sockfd, type, 4, 0);
            if(sendResult < 0) {
                LOGV("socket 断开连接，数据发送失败!!!");
                isConnect = false;
                break;
            }
            //发送帧长度，用4字节表示
            uint8_t *frameLenght = int2Bytes(msgEntity->length);
            send(sockfd, frameLenght, 4, 0);

            //发送帧数据
            send(sockfd, msgEntity->data, msgEntity->length, 0);


            char* utf8Str;
            char* gbkStr = (char*) msgEntity->data;
            gbk2utf8(utf8Str,msgEntity->length,gbkStr);
            LOGV("205--------data :%s ", utf8Str);
            LOGV("102--------data :%s  数据长度:%d", msgEntity->data,msgEntity->length);


//            发送文本的方式
//            unsigned char data[] = "this is test data 信息2029这是一条测试信息";
//            int len = strlen((char*)data);
//            uint8_t* frameLenght2 = int2Bytes(len);
//            send(sockfd, frameLenght2, 4, 0);
//            send(sockfd, data, len, 0);

        }
        usleep(1000 * 5);

    }

}

void SocketMgr::addMsg(int type, int size, uint8_t *buff) {
    if (buff != nullptr) {
        auto* videoFrame = new BaseMsgEntity(buff,24, size);
        msgQueue.push(videoFrame);
    }
}



uint8_t *SocketMgr::int2Bytes(int i) {
    unsigned char buf[4];
    buf[0] = (unsigned char) i;
    buf[1] = i >> 8;
    buf[2] = i >> 16;
    buf[3] = i >> 24;
    return buf;

}

void SocketMgr::stopSendTask() {
    isConnect = false;
    msgQueue.clear();
}

char *SocketMgr::UnicodeToUtf8(const wchar_t *unicode) {
    return nullptr;
}

void SocketMgr::recTask() {
    while (isConnect) {

    }
}






