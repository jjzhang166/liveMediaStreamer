/*
 *  FilterMockUp - A filter class mockup
 *  Copyright (C) 2014  Fundació i2CAT, Internet i Innovació digital a Catalunya
 *
 *  This file is part of media-streamer.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Authors:    Marc Palau <marc.palau@i2cat.net>
 *              David Cassany <david.cassany@i2cat.net>
 *              Gerard Castillo <gerard.castillo@i2cat.net>
 *
 */

#ifndef _FILTER_MOCKUP_HH
#define _FILTER_MOCKUP_HH

#include <thread>
#include <chrono>
#include <random>
#include <cstring>

#include "Filter.hh"
#include "AVFramedQueue.hh"
#include "Frame.hh"
#include "VideoFrame.hh"

#define READERS 1
#define WRITERS 1


class FrameMock : public Frame {
public:
    ~FrameMock(){};
    virtual unsigned char *getDataBuf() {
        return buff;
    };

    static FrameMock* createNew(size_t seqNum) {
        FrameMock *frame = new FrameMock();
        frame->setSequenceNumber(seqNum);
        return frame;
    }

    virtual unsigned char **getPlanarDataBuf() {return NULL;};
    virtual unsigned int getLength() {return 4;};
    virtual unsigned int getMaxLength() {return 4;};
    virtual void setLength(unsigned int length) {};
    virtual bool isPlanar() {return false;};

protected:
    unsigned char buff[4];
};

class VideoFrameMock : public InterleavedVideoFrame
{
public:
    ~VideoFrameMock(){};
    virtual unsigned char *getDataBuf() {
        return buff;
    };

    static VideoFrameMock* createNew() {
        return new VideoFrameMock();
    };

    virtual unsigned char **getPlanarDataBuf() {return NULL;};
    virtual unsigned int getLength() {return 4;};
    virtual unsigned int getMaxLength() {return 4;};
    virtual void setLength(unsigned int length) {};
    virtual bool isPlanar() {return false;};

protected:
    unsigned char buff[4] = {1,1,1,1};

private:
    VideoFrameMock(): InterleavedVideoFrame(RAW, 1920, 1080, YUV420P) {};
};

class AVFramedQueueMock : public AVFramedQueue
{
public:
    AVFramedQueueMock(unsigned max) : AVFramedQueue(max) {
        config();
    };

protected:
    virtual bool config() {
        for (unsigned i=0; i<max; i++) {
                frames[i] = FrameMock::createNew(i+1);
        }
        return true;
    }
};

class BaseFilterMockup : public BaseFilter
{
public:
    BaseFilterMockup(unsigned readers, unsigned writers) : BaseFilter() {
        maxReaders = readers;
        maxWriters = writers;
    };

    using BaseFilter::getReader;

protected:
    FrameQueue *allocQueue(int wId) {return new AVFramedQueueMock(4);};
    size_t masterProcessFrame() {return 20;};
    size_t slaveProcessFrame() {return 20;};
    void doGetState(Jzon::Object &filterNode) {};
    void stop() {};

private:
    VCodecType codec;

    bool runDoProcessFrame() {return true;};
};

class OneToOneFilterMockup : public OneToOneFilter
{
public:
    OneToOneFilterMockup(size_t processTime_, size_t queueSize_, bool gotFrame_,
                         size_t frameTime, FilterRole role, bool sharedFrames) :
        OneToOneFilter(true, role, sharedFrames, frameTime, false),
        processTime(processTime_), queueSize(queueSize_), gotFrame(gotFrame_) {};

    void setGotFrame(bool gotFrame_) {gotFrame = gotFrame_;};
    using BaseFilter::getReader;

protected:
    bool doProcessFrame(Frame *org, Frame *dst) {
        size_t realProcessTime;
        std::uniform_int_distribution<size_t> distribution(processTime/2, processTime*0.99);
        realProcessTime = distribution(generator);
        utils::debugMsg("Process time " + std::to_string(realProcessTime));
        std::this_thread::sleep_for(std::chrono::nanoseconds(realProcessTime));
        return gotFrame;
    }
    void doGetState(Jzon::Object &filterNode) {};
    void stop() {};

private:
    virtual FrameQueue *allocQueue(int wId) {return new AVFramedQueueMock(queueSize);};

    std::default_random_engine generator;
    size_t processTime; //usec
    size_t queueSize;
    bool gotFrame;
};

class OneToManyFilterMockup : public OneToManyFilter
{
public:
    OneToManyFilterMockup(unsigned maxWriters, size_t processTime_, size_t queueSize_, bool gotFrame_,
                         size_t frameTime, FilterRole role, bool sharedFrames) :
        OneToManyFilter(role, sharedFrames, maxWriters, frameTime, false),
        processTime(processTime_), queueSize(queueSize_), gotFrame(gotFrame_) {};

    void setGotFrame(bool gotFrame_) {gotFrame = gotFrame_;};
    using BaseFilter::getReader;

protected:
    bool doProcessFrame(Frame *org, std::map<int, Frame *> dstFrames) {
        size_t realProcessTime;
        std::uniform_int_distribution<size_t> distribution(processTime/2, processTime*0.99);
        realProcessTime = distribution(generator);
        utils::debugMsg("Process time " + std::to_string(realProcessTime));
        std::this_thread::sleep_for(std::chrono::nanoseconds(realProcessTime));
        return gotFrame;
    }
    void doGetState(Jzon::Object &filterNode) {};
    void stop() {};

private:
    virtual FrameQueue *allocQueue(int wId) {return new AVFramedQueueMock(queueSize);};

    std::default_random_engine generator;
    size_t processTime; //usec
    size_t queueSize;
    bool gotFrame;
};

class LiveMediaFilterMockup : public LiveMediaFilter
{
public:
    LiveMediaFilterMockup(unsigned maxReaders, unsigned maxWriters, unsigned queueSize_, bool watch_) :
        LiveMediaFilter(maxReaders, maxWriters),
        queueSize(queueSize_), watch(watch_) {};

    using BaseFilter::getReader;

private:
    virtual FrameQueue *allocQueue(int wId) {return new AVFramedQueueMock(queueSize);};
    void stop() {
        watch = false;
    }
    void doGetState(Jzon::Object &filterNode) {};
    bool runDoProcessFrame(){
        if(watch){
            utils::debugMsg("LiveMedia filter dummy runDoProcessFrame\n");
            while(watch){
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        }
        return true;
    }

private:
    size_t queueSize;
    bool watch;

};

class VideoFilterMockup : public OneToOneFilterMockup
{
public:
    VideoFilterMockup(VCodecType c) : OneToOneFilterMockup(20000, 4, true,
                      40000, MASTER, false)  {
        codec = c;
    };

protected:
    FrameQueue *allocQueue(int wId) {return VideoFrameQueue::createNew(codec, DEFAULT_VIDEO_FRAMES);};

private:
    VCodecType codec;
};

class AudioFilterMockup : public OneToOneFilterMockup
{
public:
    AudioFilterMockup(ACodecType c) : OneToOneFilterMockup(20000, 4, true,
                      40000, MASTER, false)  {
        codec = c;
    };

protected:
    FrameQueue *allocQueue(int wId) {return AudioFrameQueue::createNew(codec, DEFAULT_AUDIO_FRAMES);};

private:
    ACodecType codec;
};

class VideoHeadFilterMockup : public HeadFilter
{
public:
    VideoHeadFilterMockup(VCodecType c, PixType pix = P_NONE) :
        HeadFilter(), srcFrame(NULL), codec(c), pixFormat(pix){};

    bool inject(InterleavedVideoFrame* frame){
        if (! frame || frame->getCodec() != codec || 
            frame->getPixelFormat() != pixFormat){
            return false;
        }
        
        srcFrame = frame;
        
        return true;
    }
    
    void doGetState(Jzon::Object &filterNode){};
    
protected:
    bool doProcessFrame(Frame *dst) {
        InterleavedVideoFrame *dstFrame;
        
        if ((dstFrame = dynamic_cast<InterleavedVideoFrame*>(dst)) != NULL){
            memmove(dstFrame->getDataBuf(), srcFrame->getDataBuf(), sizeof(unsigned char)*srcFrame->getLength());
            
            dstFrame->setLength(srcFrame->getLength());
            dstFrame->setSize(srcFrame->getWidth(), srcFrame->getHeight());
            dstFrame->setPresentationTime(std::chrono::system_clock::now());
            dstFrame->setOriginTime(srcFrame->getOriginTime());
            dstFrame->setPixelFormat(srcFrame->getPixelFormat());
            
            return true;
        }
        
        return false;
    }
    

private:
    FrameQueue *allocQueue(int wId) {
        return VideoFrameQueue::createNew(codec, 10, pixFormat);
    };

    InterleavedVideoFrame* srcFrame;
    VCodecType codec;
    PixType pixFormat;
};

class VideoTailFilterMockup : public TailFilter
{
public:
    VideoTailFilterMockup(): TailFilter(), oFrame(NULL), newFrame(false){};

    InterleavedVideoFrame* extract(){   
        if (newFrame){
            newFrame = false;
            return oFrame;
        } else {
            return NULL;
        }
    }
    
    void doGetState(Jzon::Object &filterNode){};
    
protected:
    bool doProcessFrame(std::map<int, Frame*> orgFrames) {
        InterleavedVideoFrame *orgFrame;
        
        if ((orgFrame = dynamic_cast<InterleavedVideoFrame*>(orgFrames.begin()->second)) != NULL){
            if (!oFrame){
                oFrame = InterleavedVideoFrame::createNew(orgFrame->getCodec(), 
                                                          DEFAULT_WIDTH, DEFAULT_HEIGHT, orgFrame->getPixelFormat());
            }
            
            memmove(oFrame->getDataBuf(), orgFrame->getDataBuf(), sizeof(unsigned char)*orgFrame->getLength());
            
            oFrame->setLength(orgFrame->getLength());
            oFrame->setSize(orgFrame->getWidth(), orgFrame->getHeight());
            oFrame->setPresentationTime(orgFrame->getPresentationTime());
            oFrame->setOriginTime(orgFrame->getOriginTime());
            oFrame->setPixelFormat(orgFrame->getPixelFormat());
            oFrame->setSequenceNumber(orgFrame->getSequenceNumber());
            
            newFrame = true;
            
            return true;
        }
        
        return false;
    }
    

private:
    InterleavedVideoFrame* oFrame;
    bool newFrame;
};

#endif
