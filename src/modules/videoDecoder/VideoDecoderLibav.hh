#define __STDC_CONSTANT_MACROS 1 

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
}

#include "../../Frame.hh"

enum CodecType {H264, VP8, MJPEG, RAW};

enum PixType {RGB24, RGB32, YUYV422};

class VideoDecoderLibav {

    public:
        VideoDecoderLibav();
        bool decodeFrame(Frame *codedFrame, Frame *decodedFrame);
        bool configDecoder(CodecType cType, PixType pTyp);
        bool toBuffer(Frame *decodedFrame);
        //int getFormat();
        
    private:
        AVCodec             *codec;
        AVCodecContext      *codecCtx;
        //struct SwsContext   *imgConvertCtx;
        AVFrame             *frame;
        AVPacket            pkt;
        int                 gotFrame;
        //unsigned int        fWidth, fHeight;
        
        CodecType           fCodec;
        PixType             fPixType;
};
