
#ifndef EPLAYER_FRAMEQUEUE_H
#define EPLAYER_FRAMEQUEUE_H


#define FRAME_QUEUE_SIZE 10

typedef struct Frame {
    char *frame;
    double pts;           /* presentation timestamp for the frame */
    double duration;      /* estimated duration of the frame */
    int width;
    int height;
    int format;
    int uploaded;
} Frame;

class FrameQueue {

public:
    FrameQueue(int max_size, int keep_last);

    virtual ~FrameQueue();

    void start();

    void abort();

    Frame *currentFrame();

    Frame *nextFrame();

    Frame *lastFrame();

    Frame *peekWritable();

    void pushFrame();

    void popFrame();

    void flush();

    int getFrameSize();

    int getShowIndex() const;

private:
    void unrefFrame(Frame *vp);

private:
    int abort_request;
    Frame queue[FRAME_QUEUE_SIZE];
    int rindex;
    int windex;
    int size;
    int max_size;
    int keep_last;
    int show_index;
};

#endif //EPLAYER_FRAMEQUEUE_H
