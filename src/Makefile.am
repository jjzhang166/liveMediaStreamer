AUTOMAKE_OPTIONS = foreign

lib_LTLIBRARIES = liblivemediastreamer.la
liblivemediastreamer_la_SOURCES = modules/audioDecoder/AudioDecoderLibav.cpp \
                                  modules/audioEncoder/AudioEncoderLibav.cpp \
                                  modules/audioMixer/AudioMixer.cpp \
                                  modules/videoDecoder/VideoDecoderLibav.cpp \
                                  modules/videoEncoder/VideoEncoderX264.cpp \
                                  modules/videoEncoder/VideoEncoderX265.cpp \
                                  modules/videoEncoder/VideoEncoderX264or5.cpp \
                                  modules/videoMixer/VideoMixer.cpp \
                                  modules/videoResampler/VideoResampler.cpp \
                                  modules/dasher/Dasher.cpp \
                                  modules/dasher/DashVideoSegmenter.cpp \
                                  modules/dasher/DashAudioSegmenter.cpp \
                                  modules/dasher/MpdManager.cpp \
                                  modules/dasher/i2libdash.c \
                                  modules/dasher/i2libisoff.c \
                                  modules/liveMediaInput/ExtendedRTSPClient.cpp \
                                  modules/liveMediaInput/H264QueueSink.cpp \
                                  modules/liveMediaInput/Handlers.cpp \
                                  modules/liveMediaInput/QueueSink.cpp \
                                  modules/liveMediaInput/SourceManager.cpp \
                                  modules/liveMediaOutput/AudioQueueServerMediaSubsession.cpp \
                                  modules/liveMediaOutput/H264or5QueueServerMediaSubsession.cpp \
                                  modules/liveMediaOutput/H264QueueServerMediaSubsession.cpp \
                                  modules/liveMediaOutput/H265QueueServerMediaSubsession.cpp \
                                  modules/liveMediaOutput/H264or5QueueSource.cpp \
                                  modules/liveMediaOutput/MPEGTSQueueServerMediaSubsession.cpp \
                                  modules/liveMediaOutput/QueueServerMediaSubsession.cpp \
                                  modules/liveMediaOutput/QueueSource.cpp \
                                  modules/liveMediaOutput/SinkManager.cpp \
                                  modules/liveMediaOutput/VP8QueueServerMediaSubsession.cpp \
                                  modules/liveMediaOutput/Connection.cpp \
                                  modules/liveMediaOutput/H264VideoStreamSampler.cpp \
                                  modules/liveMediaOutput/H264or5StartCodeInjector.cpp \
                                  modules/liveMediaOutput/UltraGridAudioRTPSink.cpp \
                                  modules/liveMediaOutput/UltraGridVideoRTPSink.cpp \
                                  modules/liveMediaOutput/ADTSQueueServerMediaSubsession.cpp \
                                  modules/liveMediaOutput/ADTSStreamParser.cpp \
                                  modules/liveMediaOutput/CustomMPEG4GenericRTPSink.cpp \
                                  modules/liveMediaOutput/SPSparser/h264_stream.c \
                                  modules/sharedMemory/SharedMemory.cpp \
                                  AVFramedQueue.cpp \
                                  AudioCircularBuffer.cpp \
                                  SlicedVideoFrameQueue.cpp \
                                  AudioFrame.cpp \
                                  Controller.cpp \
                                  Event.cpp \
                                  Filter.cpp \
                                  Frame.cpp \
                                  IOInterface.cpp \
                                  Jzon.cpp \
                                  Path.cpp \
                                  PipelineManager.cpp \
                                  Utils.cpp \
                                  VideoFrame.cpp \
                                  Runnable.cpp \
                                  Worker.cpp 

liblivemediastreamer_la_CPPFLAGS = -g -D__STDC_CONSTANT_MACROS -Wall

liblivemediastreamer_la_CXXFLAGS = -std=c++11 

liblivemediastreamer_la_CFLAGS = -g -D__STDC_CONSTANT_MACROS -Wall

liblivemediastreamer_la_LDFLAGS = -shared -fPIC -pthread -lBasicUsageEnvironment -lUsageEnvironment -lliveMedia -lgroupsock -lavcodec -lavformat -lavutil -lswresample -lswscale -llog4cplus -lopencv_core -lopencv_imgproc -lx264 -lx265
