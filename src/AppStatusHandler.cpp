#include "AppStatusHandler.h"
#include <ESBManager.h>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <request.h>



AppStatusHandler::AppStatusHandler()
{
    //ctor
}

AppStatusHandler::~AppStatusHandler()
{
    //dtor
}

std::string AppStatusHandler::handle(http::server::request request)
{
    _ESBManager::SystemInfo systemInfo = _ESBManager::ESBManager::getCurrentSystemInfo();
    rapidjson::Document document;
    document.SetObject();

//    rapidjson::Value array(rapidjson::kArrayType);
//    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
//    document.AddMember("architecture", systemInfo.architecture, allocator);
//    document.AddMember("committedHeap", systemInfo.committedHeap, allocator);
//    document.AddMember("currentClassesLoaded", systemInfo.currentClassesLoaded, allocator);
//    document.AddMember("currentHeap", systemInfo.currentHeap, allocator);
//    document.AddMember("daemonThreads", systemInfo.daemonThreads, allocator);
//    document.AddMember("daemonThreads", systemInfo.garbageCollector, allocator);
//    document.AddMember("javaVirtualMachine", systemInfo.javaVirtualMachine, allocator);
//    document.AddMember("liveThreads", systemInfo.liveThreads, allocator);
//    document.AddMember("maximumHeap", systemInfo.maximumHeap, allocator);
//    document.AddMember("name", systemInfo.Name, allocator);
//    document.AddMember("peak", systemInfo.peak, allocator);
//    document.AddMember("pendingObjects", systemInfo.pendingObjects, allocator);
//    document.AddMember("pid", systemInfo.pid, allocator);
//    document.AddMember("processors", systemInfo.processors, allocator);
//    document.AddMember("totalClassesLoaded", systemInfo.totalClassesLoaded, allocator);
//    document.AddMember("totalClassesUnloaded", systemInfo.totalClassesUnloaded, allocator);
//    document.AddMember("totalCompileTime", systemInfo.totalCompileTime, allocator);
//    document.AddMember("totalStarted", systemInfo.totalStarted, allocator);
//    document.AddMember("upTime", systemInfo.upTime, allocator);
//    document.AddMember("vendor", systemInfo.vendor, allocator);
//    document.AddMember("version", systemInfo.version, allocator);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);

    return buffer.GetString();
}
