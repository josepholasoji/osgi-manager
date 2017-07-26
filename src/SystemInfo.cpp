#include "SystemInfo.h"

_ESBManager::SystemInfo::SystemInfo()
{
    //ctor
}

_ESBManager::SystemInfo::~SystemInfo()
{
    //dtor
}

_ESBManager::SystemInfo::SystemInfo(const _ESBManager::SystemInfo& other)
{
    //copy ctor
}

_ESBManager::SystemInfo& _ESBManager::SystemInfo::operator=(const _ESBManager::SystemInfo& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    //assignment operator
    return *this;
}

void _ESBManager::SystemInfo::setValue(std::string name, std::string value)
{
    if(name ==  "Architecture")
    {
        architecture = value;
    }

    else if(name ==  "Committed heap size")
    {
        committedHeap = value;
    }

    else if(name ==  "Current classes loaded")
    {
        currentClassesLoaded = value;
    }

    else if(name ==  "Current heap size")
    {
        currentHeap = value;
    }

    else if(name ==  "Daemon threads")
    {
        daemonThreads = value;
    }

    else if(name ==  "Garbage collector")
    {
       garbageCollector.size() > 0 ? garbageCollector += "\n" + value : garbageCollector = value;
    }

    else if(name ==  "Java Virtual Machine")
    {
        javaVirtualMachine = value;
    }

    else if(name ==  "Live threads")
    {
        liveThreads = value;
    }

    else if(name ==  "Live threads")
    {
        liveThreads = value;
    }

    else if(name ==  "Maximum heap size")
    {
        maximumHeap = value;
    }

    else if(name ==  "Name")
    {
        Name = value;
    }

    else if(name ==  "Version")
    {
        version = value;
    }

    else if(name ==  "Vendor")
    {
        vendor = value;
    }

    else if(name ==  "Uptime")
    {
        upTime = value;
    }

    else if(name ==  "Total started")
    {
        totalStarted = value;
    }

    else if(name ==  "Total compile time")
    {
        totalCompileTime = value;
    }

    else if(name ==  "Total classes unloaded")
    {
        totalClassesUnloaded = value;
    }

    else if(name ==  "Total classes loaded")
    {
        totalClassesLoaded = value;
    }

    else if(name ==  "Processors")
    {
        processors = value;
    }

    else if(name ==  "Pid")
    {
        pid = value;
    }

    else if(name ==  "Pending objects")
    {
        pendingObjects = value;
    }

    else if(name ==  "Peak")
    {
        peak = value;
    }
}
