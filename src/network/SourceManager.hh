/*
 *  SourceManager.hh - Class that handles multiple sessions dynamically.
 *  Copyright (C) 2014  Fundació i2CAT, Internet i Innovació digital a Catalunya
 *
 *  This file is part of liveMediaStreamer.
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
 *  Authors:  David Cassany <david.cassany@i2cat.net>,
 *            
 */
#ifndef _SOURCE_MANAGER_HH
#define _SOURCE_MANAGER_HH

#include <liveMedia.hh>
#include <BasicUsageEnvironment.hh>
#include <thread>
#include <map>
#include <list>
#include "../FrameQueue.hh"

#define ID_LENGTH 4

class Session;

class SourceManager {
private:
    SourceManager();
    
public:
    static SourceManager* getInstance();
    static void destroyInstance();
      
    bool runManager();
    bool stopManager();
    bool isRunning();
    
    void closeManager();

    bool addSession(std::string id, Session* session);
    Session* getSession(std::string id);
    
    void addFrameQueue(FrameQueue* queue);
    //TODO: determine who has to call it, should it be public?
    void removeFrameQueue(FrameQueue* queue);
    
    bool initiateAll();
        
    bool removeSession(std::string id);
    
    UsageEnvironment* envir() { return env; }
    
private:
    static void* startServer(void *args);
      
    std::thread mngrTh;
    
    static SourceManager* mngrInstance;
    std::map<std::string, Session*> sessionList;
    std::list<FrameQueue*> inputs;
    UsageEnvironment* env;
    uint8_t watch;
    
};

class Session {
public:
    static Session* createNewByURL(UsageEnvironment& env, std::string progName, std::string rtspURL);
    static Session* createNew(UsageEnvironment& env, std::string sdp);
    
    virtual ~Session();
    
    bool initiateSession();
    
protected:
    Session();
    
public:
    RTSPClient* client;
    MediaSession* session;
    MediaSubsessionIterator* iter;
};

#endif