/*
 *  PipelineManager.hh - Pipeline manager class for livemediastreamer framework
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
 *  Authors:  Marc Palau <marc.palau@i2cat.net>,
 *            David Cassany <david.cassany@i2cat.net>
 */

#ifndef _PIPELINE_MANAGER_HH
#define _PIPELINE_MANAGER_HH

#include "Filter.hh"
#include "Path.hh"
#include "Worker.hh"

#include <map>

class PipelineManager {

public:
    static PipelineManager* getInstance();
    static void destroyInstance();

    bool stop();

    Path* createPath(int orgFilter, int dstFilter, int orgWriter,
                     int dstReader, std::vector<int> midFilters);
    int searchFilterIDByType(FilterType type);
    bool addWorker(int id, Worker* worker);
    bool addPath(int id, Path* path);
    bool addFilter(int id, BaseFilter* filter);
    bool addFilterToWorker(int workerId, int filterId);

    BaseFilter* getFilter(int id);
    Worker* getWorker(int id);

    Path* getPath(int id);
    std::map<int, Path*> getPaths() {return paths;};
    bool connectPath(Path* path);
    bool addWorkerToPath(Path *path, Worker* worker = NULL);
    void startWorkers();
    void stopWorkers();

    void getStateEvent(Jzon::Node* params, Jzon::Object &outputNode);
    void createFilterEvent(Jzon::Node* params, Jzon::Object &outputNode);
    void createPathEvent(Jzon::Node* params, Jzon::Object &outputNode);
    void removePathEvent(Jzon::Node* params, Jzon::Object &outputNode);
    void addWorkerEvent(Jzon::Node* params, Jzon::Object &outputNode);
    void removeWorkerEvent(Jzon::Node* params, Jzon::Object &outputNode);
    void addSlavesToFilterEvent(Jzon::Node* params, Jzon::Object &outputNode);
    void addFiltersToWorkerEvent(Jzon::Node* params, Jzon::Object &outputNode);
    void stopEvent(Jzon::Node* params, Jzon::Object &outputNode);

private:
    PipelineManager();
    bool removePath(int id);
    bool deletePath(Path* path);
    bool removeWorker(int id);
    BaseFilter* createFilter(FilterType type, Jzon::Node* params);

    static PipelineManager* pipeMngrInstance;

    std::map<int, Path*> paths;
    std::map<int, BaseFilter*> filters;
    std::map<int, Worker*> workers;
};

#endif
