/*
 *  VideoMixer - Video mixer structure
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
 *  Authors:  Marc Palau <marc.palau@i2cat.net>
 */

#include "VideoMixer.hh"
#include "../../AVFramedQueue.hh"
#include <iostream>
#include <chrono>

///////////////////////////////////////////////////
//                ChannelConfig Class            //
///////////////////////////////////////////////////

ChannelConfig::ChannelConfig(float width, float height, float x, float y, int layer)
{
    this->width = width;
    this->height = height;
    this->x = x;
    this->y = y;
    this->layer = layer;
    enabled = false;
    opacity = 1.0;
}

void ChannelConfig::config(float width, float height, float x, float y, int layer, bool enabled, float opacity)
{
    this->width = width;
    this->height = height;
    this->x = x;
    this->y = y;
    this->layer = layer;
    this->enabled = enabled;
    this->opacity = opacity;
}

///////////////////////////////////////////////////
//                VideoMixer Class               //
///////////////////////////////////////////////////

VideoMixer::VideoMixer(FilterRole fRole_, bool sharedFrames, int framerate, int inputChannels, 
                       int outputWidth, int outputHeight, size_t fTime) :
ManyToOneFilter(fRole_, sharedFrames, inputChannels, fTime, true)
{
    this->outputWidth = outputWidth;
    this->outputHeight = outputHeight;
    fType = VIDEO_MIXER;
    maxChannels = inputChannels;
    setFrameTime(std::chrono::nanoseconds(std::nano::den/framerate));

    layoutImg = cv::Mat(outputHeight, outputWidth, CV_8UC3);
    initializeEventMap();
}

VideoMixer::~VideoMixer()
{
    for (auto it : channelsConfig) {
        delete it.second;
    }

    channelsConfig.clear();
}

FrameQueue* VideoMixer::allocQueue(int wId)
{
    return VideoFrameQueue::createNew(RAW, DEFAULT_RAW_VIDEO_FRAMES, RGB24);
}

bool VideoMixer::doProcessFrame(std::map<int, Frame*> orgFrames, Frame *dst)
{
    int frameNumber = orgFrames.size();
    VideoFrame *vFrame;

    layoutImg.data = dst->getDataBuf();
    dst->setLength(layoutImg.step * outputHeight);
    dynamic_cast<VideoFrame*>(dst)->setSize(outputWidth, outputHeight);

    layoutImg = cv::Scalar(0, 0, 0);

    for (int lay=0; lay < MAX_LAYERS; lay++) {
        for (auto it : orgFrames) {
            if (channelsConfig[it.first]->getLayer() != lay) {
                continue;
            }

            if (!it.second || !channelsConfig[it.first]->isEnabled()) {
                frameNumber--;
                continue;
            }

            vFrame = dynamic_cast<VideoFrame*>(it.second);
            pasteToLayout(it.first, vFrame);
            frameNumber--;
        }

        if (frameNumber <= 0) {
            break;
        }
    }

    return true;
}

bool VideoMixer::configChannel(int id, float width, float height, float x, float y, int layer, bool enabled, float opacity)
{
    //NOTE: w, h, x and y are set as layout size percentages

    if (channelsConfig.count(id) <= 0) {
        return false;
    }

    if (x + width > 1 || y + height > 1) {
        return false;
    }

    channelsConfig[id]->config(width, height, x, y, layer, enabled, opacity);

    return true;
}

void VideoMixer::pasteToLayout(int frameID, VideoFrame* vFrame)
{
    ChannelConfig* chConfig = channelsConfig[frameID];
    cv::Mat img(vFrame->getHeight(), vFrame->getWidth(), CV_8UC3, vFrame->getDataBuf());

    cv::Size sz(chConfig->getWidth()*outputWidth, chConfig->getHeight()*outputHeight);

    int x = chConfig->getX()*outputWidth;
    int y = chConfig->getY()*outputHeight;

    int cropX = (vFrame->getWidth() - chConfig->getWidth()*outputWidth)/2;
    int cropY = (vFrame->getHeight() - chConfig->getHeight()*outputHeight)/2;

    if (cropX < 0 || cropX + sz.width > vFrame->getWidth() || cropY < 0 || cropY + sz.height > vFrame->getHeight()) {
        return;
    }

    if (chConfig->getOpacity() == 1) {
        img(cv::Rect(cropX, cropY, sz.width, sz.height)).copyTo(layoutImg(cv::Rect(x, y, sz.width, sz.height)));
    } else {
        addWeighted(
            img(cv::Rect(cropX, cropY, sz.width, sz.height)),
            chConfig->getOpacity(),
            layoutImg(cv::Rect(x, y, sz.width, sz.height)),
            1 - chConfig->getOpacity(),
            0.0,
            layoutImg(cv::Rect(x, y, sz.width, sz.height))
        );
    }
}

Reader* VideoMixer::setReader(int readerID, FrameQueue* queue)
{
    if (readers.count(readerID) < 0) {
        return NULL;
    }

    Reader* r = new Reader();
    readers[readerID] = r;

    channelsConfig[readerID] = new ChannelConfig(1, 1, 0, 0, 0);

    return r;
}

void VideoMixer::initializeEventMap()
{
    eventMap["configChannel"] = std::bind(&VideoMixer::configChannelEvent, this, std::placeholders::_1);
}

bool VideoMixer::configChannelEvent(Jzon::Node* params)
{
    if (!params) {
        return false;
    }

    if (!params->Has("id") || !params->Has("width") || !params->Has("height") ||
            !params->Has("x") || !params->Has("y") || !params->Has("layer") ||
                !params->Has("enabled") || !params->Has("opacity")) {

        return false;
    }

    int id = params->Get("id").ToInt();
    float width = params->Get("width").ToFloat();
    float height = params->Get("height").ToFloat();
    float x = params->Get("x").ToFloat();
    float y = params->Get("y").ToFloat();
    int layer = params->Get("layer").ToInt();
    bool enabled = params->Get("enabled").ToBool();
    float opacity = params->Get("opacity").ToFloat();

    return configChannel(id, width, height, x, y, layer, enabled, opacity);
}

void VideoMixer::doGetState(Jzon::Object &filterNode)
{
    Jzon::Array jsonChannelConfigs;

    filterNode.Add("width", outputWidth);
    filterNode.Add("height", outputHeight);
    filterNode.Add("maxChannels", maxChannels);

    for (auto it : channelsConfig) {
        Jzon::Object chConfig;
        chConfig.Add("id", it.first);
        chConfig.Add("width", it.second->getWidth());
        chConfig.Add("height", it.second->getHeight());
        chConfig.Add("x", it.second->getX());
        chConfig.Add("y", it.second->getY());
        chConfig.Add("layer", it.second->getLayer());
        chConfig.Add("enabled", it.second->isEnabled());
        chConfig.Add("opacity", it.second->getOpacity());
        jsonChannelConfigs.Add(chConfig);
    }

    filterNode.Add("channels", jsonChannelConfigs);
}
