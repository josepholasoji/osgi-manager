#include "ESBManager.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include <memory>
#include <string>
#include <cstring>
#include <chrono>
#include <errno.h>

#define CHARACTER_STREAM_BUFFER_LENGTH 1027
#define THREAD_SLEEP_TIME 1000
#define MAXIMUM_LOAD_COUNT 1800 //Approximately 30min
#define RETURNED_FAILURE_DATALENGTH 50

/*extern "C" FILE* popen(const char* command, const char* mode);
extern "C" FILE* pclose(FILE*);*/

WORD ESBManager::event;
bool ESBManager::continueRunning;
FILE* ESBManager::pipe;
std::vector<int> ESBManager::bundlesToStart;
std::string ESBManager::childProcessFilePath;
std::string ESBManager::lastCommandStr = "";
std::string ESBManager::previousData = "";
bool ESBManager::gogoReady = false;
bool ESBManager::fireCommandOperationInProgress = false;
pthread_mutex_t ESBManager::gogoReadyMutext;
pthread_mutex_t ESBManager::fireCommandOperationInProgressMutext;


void* ESBManager::Start(void* param)
{
    printf("\nChecking for system command line availability...");
    if(system(nullptr))
    {
        printf("\nSystem command found, starting sub-process...");
        printf(std::string("\n[Command: " + ESBManager::childProcessFilePath + " ]\n").c_str());

        ESBManager::pipe = popen(ESBManager::childProcessFilePath.c_str(), "w");
        if(ESBManager::pipe != NULL)
        {
            try
            {
                ESBManager::continueRunning = true;

                //Start gogo ready state monitor service here...
                printf("\nSub-process starting...");

                pthread_t startMonitor;
                pthread_create(&startMonitor, NULL, ESBManager::MonitorIfGogoIsReadyForCommand, NULL);

                printf("\nSub-process started successfully...");
                ESBManager::StartMonitorService();
            }
            catch (std::runtime_error ex)
            {
                pclose(ESBManager::pipe);
                throw;
            }
        }
        else{
            printf(std::string("Error starting child process with command [ " + ESBManager::childProcessFilePath + " ] | %s\n").c_str(),strerror(errno));
            ESBManager::continueRunning = false;
        }
    }
}

ESBManager::ESBManager()
{
    //
    childProcessFilePath = "";
    ESBManager::continueRunning = false;
    ESBManager::bundlesToStart = {};
}

ESBManager::~ESBManager()
{
    //dtor
}

void ESBManager::InitializeAndStart()
{
    printf("Manager starting...");
    ESBManager::continueRunning = true;
    pthread_t StartKaraf;
    ESBManager::childProcessFilePath = "chkdsk /?";

    pthread_create(&StartKaraf, NULL, ESBManager::Start, NULL);
    while(ESBManager::continueRunning){Sleep(10000);};
}

void ESBManager::ShutDown()
{
    //dtor
}

void ESBManager::StartMonitorService()
{
    while (ESBManager::continueRunning)
    {
        if(ESBManager::IsGogoReady() && ESBManager::IsFireCommandOperationInProgress())
        {
            if(ESBManager::getLastCommandStr().compare(GOGO_LIST_STARTED_BUNDLES) == 0)
            {
                ESBManager::GetDeadBundles();
            }
            else if(ESBManager::getLastCommandStr().compare(GOGO_LIST_DEAD_BUNDLES) == 0)
            {
                ESBManager::GetSystemInfo();
            }
            else if(ESBManager::getLastCommandStr().compare(GOGO_GET_SYSTEM_INFO) == 0)
            {
                 ESBManager::StartBundles();
            }
            else if(ESBManager::getLastCommandStr().compare(GOGO_START_BUNDLE) == 0)
            {

            }
            else if(ESBManager::getLastCommandStr().compare(GOGO_STOP_BUNDLE) == 0)
            {

            }
            else if(ESBManager::getLastCommandStr().compare(GOGO_RESTART_BUNDLE) == 0)
            {

            }
            else
            {
                ESBManager::GetStartedBundles();
            }
        }

        Sleep(THREAD_SLEEP_TIME);
    }
}

void ESBManager::GetDeadBundles()
{
    ESBManager::FireCommand(GOGO_LIST_DEAD_BUNDLES);
    //auto deadBundles = ESBManager::ProcessDeadBundlesData(commandOutputData);
}

void ESBManager::GetStartedBundles()
{
    ESBManager::FireCommand(GOGO_LIST_STARTED_BUNDLES);
    //auto startedBundles = ESBManager::ProcessStartedBundlesData(commandOutputData);
}

void ESBManager::GetSystemInfo()
{
    ESBManager::FireCommand(GOGO_GET_SYSTEM_INFO);
    //auto systemInfo = ESBManager::ProcessSystemInfoData(commandOutputData);
}

void ESBManager::StartBundles()
{
    for(unsigned int i = 0; i < bundlesToStart.size(); i++)
    {
        char smallBuffer[1024] = {0};
        sprintf(smallBuffer, GOGO_START_BUNDLE.c_str(), bundlesToStart[i]);
        ESBManager::FireCommand(std::string(smallBuffer));
    }
}

bool ESBManager::StartupService()
{
    int loadCount = 0;
    bool loadingTimedOut = false;
    char charToRead[CHARACTER_STREAM_BUFFER_LENGTH] = {0};

    while (ESBManager::continueRunning)
    {
        if(loadCount > MAXIMUM_LOAD_COUNT)
        {
            loadingTimedOut = true;
            break;
        }

        fputs(CHECK_FOR_GOGO_READY.c_str(), pipe);
        if (fgets(charToRead, CHARACTER_STREAM_BUFFER_LENGTH, pipe) != NULL)
        {
            printf(charToRead);
            if(strstr(charToRead, GOGO_READY.c_str()) != NULL) break;

            memset(charToRead, 0, CHARACTER_STREAM_BUFFER_LENGTH);
            Sleep(THREAD_SLEEP_TIME);
            loadingTimedOut++;
        }
    }

    return loadingTimedOut ? false : true;
}

void ESBManager::FireCommand(std::string command)
{
    char charToRead[CHARACTER_STREAM_BUFFER_LENGTH] = {0};

    while(true)
    {
        if(!ESBManager::IsFireCommandOperationInProgress() && ESBManager::IsGogoReady())
        {
            ESBManager::FireCommandOperationInProgress(true);
            ESBManager::GogoReady(false);
            ESBManager::setLastCommandStr(command);
            ESBManager::setPreviousData("");

            if (fputs(command.c_str(), pipe) == NULL)
            {
                printf(std::string("Error occurred: Can not send command:> " + command).c_str());
            }

            ESBManager::FireCommandOperationInProgress(false);
            break;
        }

        Sleep(THREAD_SLEEP_TIME);
    }

    printf(std::string("Command [ " + command + " ] fired successfully!").c_str());
}

void* ESBManager::MonitorIfGogoIsReadyForCommand(void *param)
{
    char charToRead[CHARACTER_STREAM_BUFFER_LENGTH] = {0};

    while(true)
    {
        if(ESBManager::IsFireCommandOperationInProgress() && !ESBManager::IsGogoReady())
        {
            if (fputs(CHECK_FOR_GOGO_READY.c_str(), pipe) > 0)
            {
                memset(charToRead, 0, CHARACTER_STREAM_BUFFER_LENGTH);
                if (fgets(charToRead, CHARACTER_STREAM_BUFFER_LENGTH, pipe) != NULL)
                {
                    ESBManager::getPreviousData().append(charToRead);
                    printf(charToRead);

                    if(strstr(charToRead, GOGO_READY.c_str()) != NULL)
                    {
                        memset(charToRead, 0, CHARACTER_STREAM_BUFFER_LENGTH);
                        ESBManager::GogoReady(true);
                        break;
                    }
                }
            }
        }

        Sleep(THREAD_SLEEP_TIME);
    }
}

std::vector<std::string>*  ESBManager::ProcessDeadBundlesData(std::string deadBundledata)
{
    return nullptr;
}

std::vector<std::string>*  ESBManager::ProcessStartedBundlesData(std::string startedBundleData)
{
    return nullptr;
}

void* ESBManager::ProcessSystemInfoData(std::string infoData)
{
    return nullptr;
}

bool ESBManager::IsServiceStarted(std::string commandData)
{
    return commandData.size() > RETURNED_FAILURE_DATALENGTH ? false : true;
}

void ESBManager::GogoReady(bool IsGogoReady)
{
    pthread_mutex_lock(&gogoReadyMutext);
        gogoReady = IsGogoReady;
    pthread_mutex_unlock(&gogoReadyMutext);
}

bool ESBManager::IsGogoReady()
{
    return gogoReady;
}

void ESBManager::FireCommandOperationInProgress(bool IsFireCommandOperationInProgress)
{
    pthread_mutex_lock(&fireCommandOperationInProgressMutext);
        fireCommandOperationInProgress = IsFireCommandOperationInProgress;
    pthread_mutex_unlock(&fireCommandOperationInProgressMutext);
}

bool ESBManager::IsFireCommandOperationInProgress()
{
    return fireCommandOperationInProgressMutext;
}
