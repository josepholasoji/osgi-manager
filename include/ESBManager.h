#ifndef ESBMANAGER_H
#define ESBMANAGER_H
#include <string>
#include <vector>
#include <mutex>
#include "DEFS.h"
#include "main.h"
#include "console_commands.h"
#include <pthread.h>


class ESBManager
{
    public:
        ESBManager();
        virtual ~ESBManager();

        static std::string childProcessFilePath;
        static void* Start(void*);
        void InitializeAndStart();
    protected:

    private:

        void ShutDown();
        static bool StartupService();
        static void StartMonitorService();
        static void* MonitorIfGogoIsReadyForCommand(void*);

        static WORD event;
        static bool continueRunning;
        static FILE* pipe;

        static void GetDeadBundles();
        static void GetStartedBundles();
        static void GetSystemInfo();
        static void StartBundles();
        static void FireCommand(std::string);


        static std::vector<std::string>*  ProcessDeadBundlesData(std::string);
        static std::vector<std::string>*  ProcessStartedBundlesData(std::string);
        static void* ProcessSystemInfoData(std::string);
        static bool IsServiceStarted(std::string);

        static std::vector<int> bundlesToStart;

        static WORD getEvent()
        {
            return ESBManager::event;
        }

        static WORD setEvent(WORD event)
        {
            return ESBManager::event = event;
        }


        static std::string lastCommandStr;
        static void setLastCommandStr(std::string command){
            ESBManager::lastCommandStr = command;
        }

        static std::string getLastCommandStr(){
            return ESBManager::lastCommandStr;
        }

        static std::string previousData;
        static void setPreviousData(std::string _previousData){
            ESBManager::previousData = _previousData;
        }

        static std::string getPreviousData(){
            return ESBManager::previousData;
        }


        static bool gogoReady;
        static pthread_mutex_t gogoReadyMutext;

        static void GogoReady(bool IsGogoReady);
        static bool IsGogoReady();

        static bool fireCommandOperationInProgress;
        static pthread_mutex_t  fireCommandOperationInProgressMutext;

        static void FireCommandOperationInProgress(bool IsFireCommandOperationInProgress);
        static bool IsFireCommandOperationInProgress();
};

#endif // ESBMANAGER_H
