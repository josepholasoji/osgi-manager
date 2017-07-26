#ifndef SYSTEMINFO_H
#define SYSTEMINFO_H

#include<string>

namespace _ESBManager
{
    class SystemInfo
    {
        public:
            SystemInfo();
            virtual ~SystemInfo();
            SystemInfo(const SystemInfo& other);
            SystemInfo& operator=(const SystemInfo& other);

        void setValue(std::string name, std::string value);

        //JVM
        std::string javaVirtualMachine;
        std::string version;
        std::string vendor;
        std::string pid;
        std::string upTime;
        std::string totalCompileTime;

        //Threads
        std::string liveThreads;
        std::string daemonThreads;
        std::string peak;
        std::string totalStarted;

        //Memory
        std::string currentHeap;
        std::string maximumHeap;
        std::string committedHeap;
        std::string pendingObjects;
        std::string garbageCollector;

        //Classes
        std::string currentClassesLoaded;
        std::string totalClassesLoaded;
        std::string totalClassesUnloaded;

        //Operating system
        std::string Name;
        std::string architecture;
        std::string processors ;

        protected:

        private:
    };
}


#endif // SYSTEMINFO_H
