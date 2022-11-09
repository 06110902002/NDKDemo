//
// Created by liuxiaobing on 7/26/21.
//

#include <pthread.h>
#include <LogUtils.h>
#include "H264Demux.h"

H264Demux::H264Demux(const char *filePath) {
    this->m_h264FilePath = static_cast<char *>(malloc(512));
    strcpy(this->m_h264FilePath, filePath);
    m_h264_file = nullptr;
}

H264Demux::~H264Demux() {
    isStartRead = false;
    //因为启动了子线程，此处等子任务结束之后  再释放与其相关的资源
    pthread_join(demux_thread,nullptr);
    if (this->m_h264FilePath) {
        free(this->m_h264FilePath);
        this->m_h264FilePath = nullptr;
    }
    if (onParse) {
        onParse = nullptr;
    }
    LOGI("H264Demux  析构");
}

void *h264DemuxTask(void *data) {
    auto *h264Demux = static_cast<H264Demux *>(data);
    h264Demux->isStartRead = true;
    h264Demux->demux();
    return 0;
}

void H264Demux::startDemux() {
    pthread_create(&demux_thread, NULL, h264DemuxTask, this);
}

int H264Demux::demux() {

    if (!this->m_h264FilePath) {
        LOGI("%s,路径为空，线程停止", this->m_h264FilePath);
        return 0;
    }

    NALU *n;
    int buffersize = 100000;

    m_h264_file = fopen(this->m_h264FilePath, "rb+");
    if (m_h264_file == NULL) {
        LOGE("Open file error\n");
        return 0;
    }

    n = (NALU *) calloc(1, sizeof(NALU));
    if (n == NULL) {
        LOGI("Alloc NALU Error\n");
        return 0;
    }

    n->max_size = buffersize;
    n->buf = (char *) calloc(buffersize, sizeof(char));
    if (n->buf == NULL) {
        free(n);
        LOGI("AllocNALU: n->buf");
        return 0;
    }

    int data_offset = 0;
    int nal_num = 0;
    LOGI("-----+-------- NALU Table ------+---------+\n");
    LOGI(" NUM |    POS  |    IDC |  TYPE |   LEN   |\n");
    LOGI("-----+---------+--------+-------+---------+\n");
    uint32_t timeStamp = 0;
    while (isStartRead && !feof(m_h264_file)) {
        int data_lenth = getAnnexbNALU(n);
        char type_str[20] = {0};
        switch (n->nal_unit_type) {
            case SLICE:
                sprintf(type_str, "SLICE");
                break;
            case DPA:
                sprintf(type_str, "DPA");
                break;
            case DPB:
                sprintf(type_str, "DPB");
                break;
            case DPC:
                sprintf(type_str, "DPC");
                break;
            case IDR:
                sprintf(type_str, "IDR");
                break;
            case SEI:
                sprintf(type_str, "SEI");
                break;
            case SPS:
                sprintf(type_str, "SPS");
                break;
            case PPS:
                sprintf(type_str, "PPS");
                break;
            case AUD:
                sprintf(type_str, "AUD");
                break;
            case EOSEQ:
                sprintf(type_str, "EOSEQ");
                break;
            case EOSTREAM:
                sprintf(type_str, "EOSTREAM");
                break;
            case FILL:
                sprintf(type_str, "FILL");
                break;
        }
        char idc_str[20] = {0};
        switch (n->nal_reference_idc >> 5) {
            case PRIORITY_DISPOSABLE:
                sprintf(idc_str, "DISPOS");
                break;
            case PRIRITY_LOW:
                sprintf(idc_str, "LOW");
                break;
            case PRIORITY_HIGH:
                sprintf(idc_str, "HIGH");
                break;
            case PRIORITY_HIGHEST:
                sprintf(idc_str, "HIGHEST");
                break;
        }
        if (onParse) {
            onParse(PARSING, n);
        }
        //usleep(1000 * 1000 / 25);
        //LOGI("%5d| %8d| %7s| %6s| %8d|\n", nal_num, data_offset, idc_str, type_str, n->len);
        data_offset = data_offset + data_lenth;
        nal_num++;
    }
    if (m_h264_file) {
        fflush(m_h264_file);
        fclose(m_h264_file);
    }
    if (onParse) {
        onParse(PARSED, nullptr);
    }
    if (n) {
        if (n->buf) {
            free(n->buf);
            n->buf = NULL;
        }
        free(n);
    }
    return 0;
}

int H264Demux::findStartCode3(unsigned char *buf) {
    if (buf[0] != 0 || buf[1] != 0 || buf[2] != 1) return 0; //0x000001?
    else return 1;
}

int H264Demux::findStartCode4(unsigned char *Buf) {
    if (Buf[0] != 0 || Buf[1] != 0 || Buf[2] != 0 || Buf[3] != 1) return 0;//0x00000001?
    else return 1;
}

int info22 = 0, info33 = 0;

int H264Demux::getAnnexbNALU(NALU *nalu) {
    int pos = 0;
    int StartCodeFound, rewind;
    unsigned char *Buf;

    if ((Buf = (unsigned char *) calloc(nalu->max_size, sizeof(char))) == NULL)
        printf("GetAnnexbNALU: Could not allocate Buf memory\n");

    nalu->startcodeprefix_len = 3;

    if (3 != fread(Buf, 1, 3, m_h264_file)) {
        free(Buf);
        return 0;
    }
    info22 = findStartCode3(Buf);
    if (info22 != 1) {
        if (1 != fread(Buf + 3, 1, 1, m_h264_file)) {
            free(Buf);
            return 0;
        }
        info33 = findStartCode4(Buf);
        if (info33 != 1) {
            free(Buf);
            return -1;
        } else {
            pos = 4;
            nalu->startcodeprefix_len = 4;
        }
    } else {
        nalu->startcodeprefix_len = 3;
        pos = 3;
    }
    StartCodeFound = 0;
    info22 = 0;
    info33 = 0;

    while (!StartCodeFound) {
        if (feof(m_h264_file)) {
            nalu->len = (pos - 1) - nalu->startcodeprefix_len;
            memcpy(nalu->buf, &Buf[nalu->startcodeprefix_len], nalu->len);
            nalu->forbidden_bit = nalu->buf[0] & 0x80; //1 bit
            nalu->nal_reference_idc = nalu->buf[0] & 0x60; // 2 bit
            nalu->nal_unit_type = (nalu->buf[0]) & 0x1f;// 5 bit
            free(Buf);
            return pos - 1;
        }
        Buf[pos++] = fgetc(m_h264_file);
        info33 = findStartCode4(&Buf[pos - 4]);
        if (info33 != 1)
            info22 = findStartCode3(&Buf[pos - 3]);
        StartCodeFound = (info22 == 1 || info33 == 1);
    }

    // Here, we have found another start code (and read length of startcode bytes more than we should
    // have.  Hence, go back in the file
    rewind = (info33 == 1) ? -4 : -3;

    if (0 != fseek(m_h264_file, rewind, SEEK_CUR)) {
        free(Buf);
        printf("GetAnnexbNALU: Cannot fseek in the bit stream file");
    }

    // Here the Start code, the complete NALU, and the next start code is in the Buf.
    // The size of Buf is pos, pos+rewind are the number of bytes excluding the next
    // start code, and (pos+rewind)-startcodeprefix_len is the size of the NALU excluding the start code

    nalu->len = (pos + rewind) - nalu->startcodeprefix_len;
    memcpy(nalu->buf, &Buf[nalu->startcodeprefix_len], nalu->len);//
    nalu->forbidden_bit = nalu->buf[0] & 0x80; //1 bit
    nalu->nal_reference_idc = nalu->buf[0] & 0x60; // 2 bit
    nalu->nal_unit_type = (nalu->buf[0]) & 0x1f;// 5 bit
    free(Buf);

    return (pos + rewind);
}

int H264Demux::isContainStart(unsigned char *buffer) {
    return findStartCode3(buffer) || findStartCode4(buffer);
}

