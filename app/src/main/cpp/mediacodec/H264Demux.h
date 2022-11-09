//
// Created by liuxiaobing on 7/26/21.
// h264 解析获取 NALU 数据

#ifndef RTMPLIVEPUSHDEMO_MASTER_H264DEMUX_H
#define RTMPLIVEPUSHDEMO_MASTER_H264DEMUX_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#define PARSING  1
#define PARSED   0

typedef enum {
    SLICE = 1,
    DPA = 2,
    DPB = 3,
    DPC = 4,
    IDR = 5,
    SEI = 6,
    SPS = 7,
    PPS = 8,
    AUD = 9,
    EOSEQ = 10,
    EOSTREAM = 11,
    FILL = 12,
} NALU_TYPE;

typedef enum {
    PRIORITY_DISPOSABLE = 0,
    PRIRITY_LOW = 1,
    PRIORITY_HIGH = 2,
    PRIORITY_HIGHEST = 3
} Nalu_Priority;

typedef struct {
    int startcodeprefix_len;      //! 4 for parameter sets and first slice in picture, 3 for everything else (suggested)
    unsigned len;                 //! Length of the NAL unit (Excluding the start code, which does not belong to the NALU)
    unsigned max_size;            //! Nal Unit Buffer size
    int forbidden_bit;            //! should be always FALSE
    int nal_reference_idc;        //! NALU_PRIORITY_xxxx
    int nal_unit_type;            //! NALU_TYPE_xxxx
    char *buf;                    //! contains the first byte followed by the EBSP
} NALU;


class H264Demux {

public:

    H264Demux(const char *filePath);

    ~H264Demux();

    void startDemux();

    bool isStartRead;

    int demux();

    int isContainStart(unsigned char* buffer);

    void (*onParse)(uint32_t status,NALU* data);

private:


    char *m_h264FilePath = nullptr;
    pthread_t demux_thread = NULL;
    /**
     * 找3位起始码 0x00 00 01
     * @param buf
     * @return
     */
    int findStartCode3(unsigned char *buf);

    /**
     * 找4位的起始码 0x00 00 00 01
     * @param buf
     * @return
     */
    int findStartCode4(unsigned char *buf);

    int getAnnexbNALU(NALU *nalu);

    FILE *m_h264_file;
};


#endif //RTMPLIVEPUSHDEMO_MASTER_H264DEMUX_H
