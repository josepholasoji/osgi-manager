#ifndef ESBMANAGER_H
#define ESBMANAGER_H
#include <string>
#include <vector>
#include <mutex>
#include "DEFS.h"
#include "main.h"
#include "console_commands.h"
#include <pthread.h>
#include <memory>
#include <map>
#include <regex>

#include "Bundle.h"
#include "SystemInfo.h"


namespace _ESBManager
{
    class ESBManager
    {
        public:
            ESBManager();
            virtual ~ESBManager();

            static void* Start(void*);
            void InitializeAndStart();

            static _ESBManager::SystemInfo getCurrentSystemInfo(){
                return _ESBManager::ESBManager::currentSystemInfo;;
            }

            static std::map<std::string, _ESBManager::Bundle*> GetBundlesWithNameAndId(){
                return _ESBManager::ESBManager::bundlesWithNameAndId;;
            }


        protected:

        private:

            void ShutDown();
            static void* StartMonitorService(void*);
            static void* StartRESTServer(void*);
            static void* MonitorIfGogoIsReadyForCommand(void*);

            static WORD event;
            static bool continueRunning;
            static bool showDebugInfo;
            static FILE* fPipe;

            static std::string BuildBundlesToStop(std::map<std::string, _ESBManager::Bundle*> bundlesWithNameAndId, std::vector<std::string> bundleStopList);
            static std::string BuildBundlesToStart(std::map<std::string, _ESBManager::Bundle*> bundlesWithNameAndId,
                                                   std::vector<std::string> deadBundles,
                                                   std::vector<std::string> failedBundles,
                                                   std::vector<std::string> bundleKeepAliveList,
                                                   std::vector<std::string> systemBundleList);

            static std::vector<std::string> GetListOfBundlesWhoesStateShouldBeEscalated(std::map<std::string, _ESBManager::Bundle*> bundlesWithNameAndId);
            static std::vector<std::string> GetFailedBundles(std::map<std::string, _ESBManager::Bundle*> bundlesWithNameAndId);
            static std::vector<std::string> GetDeadBundles(std::map<std::string, _ESBManager::Bundle*> bundlesWithNameAndId);
            static std::map<std::string, _ESBManager::Bundle*> MapBundleNamesToBundlesId(std::string bundleList);


            static _ESBManager::SystemInfo GetSystemInfo(std::string);
            static void StartBundles();
            static void FireCommand(std::string);
            static std::string FireCommandExecVP(std::string);

            static std::vector<std::string> systemBundles;
            static std::vector<std::string> processBundles;

            static std::string bundlesToStart;
            static std::string bundlesToStop;
            static std::string gogoUserName;
            static std::string gogoPassword;
            static std::regex  regexListParser;
            static std::regex  systemInfoParser;
            static std::string workingDir;

            static std::map<std::string, _ESBManager::Bundle*> bundlesWithNameAndId;
            static std::vector<std::string> deadBundles;
            static std::vector<std::string> failedBundles;
            static std::vector<std::string> inferBundlesToEscalateByNotitifacation;
            static _ESBManager::SystemInfo currentSystemInfo;

            static std::string karafStartupCommnad;
            static std::vector<std::string>*  ProcessDeadBundlesData(std::string);
            static std::vector<std::string>*  ProcessStartedBundlesData(std::string);
            static void* ProcessSystemInfoData(std::string);
            static bool IsServiceStarted(std::string);
            static std::string trim(const std::string&);

            static bool startWholly;

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

            static std::shared_ptr<std::string> previousData;
            static void setPreviousData(std::shared_ptr<std::string> _previousData){
                ESBManager::previousData = _previousData;
            }

            static std::shared_ptr<std::string> getPreviousData(){
                return ESBManager::previousData;
            }


            static bool gogoReady;
            static std::shared_ptr<pthread_mutex_t> gogoReadyMutext;

            static void GogoReady(bool IsGogoReady);
            static bool IsGogoReady();

            static bool fireCommandOperationInProgress;
            static std::shared_ptr<pthread_mutex_t>  fireCommandOperationInProgressMutext;

            static void FireCommandOperationInProgress(bool IsFireCommandOperationInProgress);
            static bool IsFireCommandOperationInProgress();
    };

}

#endif // ESBMANAGER_H
