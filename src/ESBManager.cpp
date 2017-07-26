#include "ESBManager.h"
#include "Bundle.h"
#include "SystemInfo.h"

#include <stdio.h>
#include <stdlib.h>
#include  <sys/types.h>
#include <iostream>
#include <pthread.h>
#include <memory>
#include <string>
#include <cstring>
#include <chrono>
#include <errno.h>
#include <memory>
#include <map>
#include <regex>
#include <unistd.h>  /* _exit, fork */
#include <sys/types.h> /* pid_t */
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#define CHARACTER_STREAM_BUFFER_LENGTH  1027
#define THREAD_SLEEP_TIME               1000
#define MAXIMUM_LOAD_COUNT              1800 //Approximately 30min
#define RETURNED_FAILURE_DATALENGTH     50

/*extern "C" FILE* popen(const char* command, const char* mode);
extern "C" FILE* pclose(FILE*);*/

WORD _ESBManager::ESBManager::event;
bool _ESBManager::ESBManager::continueRunning;
FILE* _ESBManager::ESBManager::fPipe;

std::string                  _ESBManager::ESBManager::lastCommandStr = "";
std::shared_ptr<std::string> _ESBManager::ESBManager::previousData;

bool _ESBManager::ESBManager::gogoReady = false;
bool _ESBManager::ESBManager::fireCommandOperationInProgress = false;
bool _ESBManager::ESBManager::showDebugInfo = true;

std::shared_ptr<pthread_mutex_t> _ESBManager::ESBManager::gogoReadyMutext = std::make_shared<pthread_mutex_t>();
std::shared_ptr<pthread_mutex_t> _ESBManager::ESBManager::fireCommandOperationInProgressMutext = std::make_shared<pthread_mutex_t>();
std::string                      _ESBManager::ESBManager::karafStartupCommnad = "";

std::vector<std::string> _ESBManager::ESBManager::systemBundles;
std::vector<std::string> _ESBManager::ESBManager::processBundles;

std::string              _ESBManager::ESBManager::bundlesToStart  = "";
std::string              _ESBManager::ESBManager::bundlesToStop   = "";
bool                     _ESBManager::ESBManager::startWholly     = false;

std::map<std::string,_ESBManager::Bundle> bundlesWithNameAndId;

_ESBManager::SystemInfo currentSystemInfo;
_ESBManager::SystemInfo _ESBManager::ESBManager::currentSystemInfo;

std::regex  _ESBManager::ESBManager::regexListParser;
std::regex  _ESBManager::ESBManager::systemInfoParser;
std::string _ESBManager::ESBManager::gogoUserName = "";
std::string _ESBManager::ESBManager::gogoPassword = "";
std::string _ESBManager::ESBManager::workingDir = "";


void* _ESBManager::ESBManager::Start(void* param)
{
    printf("\nChecking for system command line availability...");
    if(system(nullptr))
    {
        printf("\nSystem command found, starting sub-process...");

        if(_ESBManager::ESBManager::showDebugInfo)
            printf(std::string("\n[Command: " + karafStartupCommnad + " ]\n").c_str());

        //Start gogo ready state monitor service here...
        printf("\nSub-process starting...");
        _ESBManager::ESBManager::fPipe = popen(karafStartupCommnad.c_str(), "w");
        if(_ESBManager::ESBManager::fPipe != NULL)
        {
            try
            {
                _ESBManager::ESBManager::FireCommandOperationInProgress(true); //Start up command
                _ESBManager::ESBManager::GogoReady(false);   //Gogo asn't started yet
                _ESBManager::ESBManager::setPreviousData(std::make_shared<std::string>(std::string(1024000,0)));
                _ESBManager::ESBManager::continueRunning = true; //Start the systems and keep running
            }
            catch (std::runtime_error ex)
            {
                pclose(_ESBManager::ESBManager::fPipe);
                throw;
            }
        }
        else
        {
            printf(std::string("\nError starting child process with command [ " + karafStartupCommnad + " ] | %s\n").c_str(),strerror(errno));
            _ESBManager::ESBManager::continueRunning = false;
        }
    }

    return nullptr;
}

_ESBManager::ESBManager::ESBManager()
{
    _ESBManager::ESBManager::continueRunning = false;
    _ESBManager::ESBManager::bundlesToStart = {};

    pthread_mutex_init(_ESBManager::ESBManager::fireCommandOperationInProgressMutext.get(), NULL);
    pthread_mutex_init(_ESBManager::ESBManager::gogoReadyMutext.get(), NULL);
}

_ESBManager::ESBManager::~ESBManager()
{
    //dtor
    pthread_mutex_destroy(_ESBManager::ESBManager::fireCommandOperationInProgressMutext.get());
    pthread_mutex_destroy(_ESBManager::ESBManager::gogoReadyMutext.get());
}

void _ESBManager::ESBManager::InitializeAndStart()
{
    printf("Manager starting...");
    _ESBManager::ESBManager::continueRunning = true;
    pthread_t StartKaraf;

    const int FILE_READ_CHUNK_SIZE = 1024;
    std::string configString = "";
    FILE* configFile = fopen("./com.carnegies.esb.manager.cfg", "r");
    if(configFile != NULL)
    {
        char rdData[FILE_READ_CHUNK_SIZE + 1] = {0};
        while(true)
        {
            if(fread(rdData, 1, FILE_READ_CHUNK_SIZE, configFile) > 0)
            {
                configString.append(rdData);
                if(strlen(rdData) < FILE_READ_CHUNK_SIZE) break;
                memset(rdData, 0, FILE_READ_CHUNK_SIZE);
            }
        }

        //printf(configString.c_str());
        fclose(configFile);

        //Load the config...
        rapidjson::Document d;
        d.Parse(configString.c_str());

        rapidjson::Value& set_commands     = d["set_commands"];
        rapidjson::Value& batch_commands   = d["batch_commands"];
        rapidjson::Value& system_bundles   = d["system_bundles"];
        rapidjson::Value& process_bundles  = d["process_bundles"];

        rapidjson::Value& workingDir  = d["working.dir"];
        rapidjson::Value& command     = d["command"];
        rapidjson::Value& mainclass   = d["mainclass"];
        rapidjson::Value& classpath1  = d["classpath.1"];
        rapidjson::Value& classpath2  = d["classpath.2"];
        rapidjson::Value& classpath3  = d["classpath.3"];
        rapidjson::Value& classpath4  = d["classpath.4"];
        rapidjson::Value& listParser  = d["listParser"];
        rapidjson::Value& systemInfoParser  = d["systemInfoParser"];
        rapidjson::Value& gogoUsername  = d["gogo_username"];
        rapidjson::Value& gogoPassword  = d["gogo_password"];
        rapidjson::Value& libraryPath   = d["library.path.1"];
        rapidjson::Value& configStartWolly   = d["startWholly"];

        _ESBManager::ESBManager::startWholly     = (configStartWolly.GetString() == "true");
        _ESBManager::ESBManager::regexListParser = std::regex(listParser.GetString());
        _ESBManager::ESBManager::systemInfoParser = std::regex(systemInfoParser.GetString(), std::regex::extended);
        _ESBManager::ESBManager::gogoUserName    = gogoUsername.GetString();
        _ESBManager::ESBManager::gogoPassword    = gogoPassword.GetString();
        _ESBManager::ESBManager::workingDir      = workingDir.GetString();

        //process the jvm array
        std::string batchCommands = "";
        for(unsigned int i = 0; i < batch_commands.Size(); i++)
        {
            batchCommands.append( " " + std::string(batch_commands[i].GetString()) );
        }

        //process system bundles
        for(unsigned int i = 0; i < system_bundles.Size(); i++)
        {
            _ESBManager::ESBManager::systemBundles.push_back( std::string(system_bundles[i].GetString()) );
        }

        //process the process bundles
        for(unsigned int i = 0; i < process_bundles.Size(); i++)
        {
            _ESBManager::ESBManager::processBundles.push_back( std::string(process_bundles[i].GetString()) );
        }

        //Process the set commands
        std::string commandLineSetString = "";
        for (rapidjson::Value::ConstMemberIterator itr = set_commands.MemberBegin(); itr != set_commands.MemberEnd(); ++itr)
        {
            std::string key = itr->name.GetString();
            std::string value = itr->value.GetString();


            while(batchCommands.find("%" + key + "%") != std::string::npos)
            {
                auto position = batchCommands.find("%" + key + "%");
                batchCommands.replace(position, (std::string("%" + key + "%")).size(), value);
            }

            commandLineSetString.append("set " + key + "=" + value + " & ");
        }

        karafStartupCommnad = commandLineSetString + batchCommands;

        //
        if(_ESBManager::ESBManager::startWholly)
        {
            pthread_create(&StartKaraf, NULL, _ESBManager::ESBManager::Start, NULL);
        }
        else
        {
            _ESBManager::ESBManager::GogoReady(true);
            _ESBManager::ESBManager::FireCommandOperationInProgress(false);
            pthread_create(&StartKaraf, NULL, _ESBManager::ESBManager::StartMonitorService, NULL);
        }

        Sleep((THREAD_SLEEP_TIME * 5));
        while(_ESBManager::ESBManager::continueRunning)
        {
            Sleep(THREAD_SLEEP_TIME * 5);
        };
    }
    else
        printf("\nError occurred: config. file missing. | %s\n", strerror(errno));
}

void _ESBManager::ESBManager::ShutDown()
{
    //dtor
}

std::string _ESBManager::ESBManager::trim(const std::string& str)
{
    size_t first = str.find_first_not_of(' ');
    if (std::string::npos == first)
    {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

std::map<std::string, _ESBManager::Bundle*> _ESBManager::ESBManager::MapBundleNamesToBundlesId(std::string bundleList)
{
    std::string tempString;
    std::vector<std::string> _bundleLines;
    std::map<std::string, _ESBManager::Bundle*> bundles;

    boost::split(_bundleLines, bundleList, boost::is_any_of("\n"));
    for(unsigned int i = 2; i < _bundleLines.size(); i++)
    {
        std::smatch _match;
         std::string eachBundleLine = _bundleLines[i];

        if(std::regex_search( eachBundleLine, _match, _ESBManager::ESBManager::regexListParser ))
        {
            _ESBManager::Bundle* bundle = new _ESBManager::Bundle();
            bundle->id          = _ESBManager::ESBManager::trim(_match[1]);
            bundle->state       = _ESBManager::ESBManager::trim(_match[2]);
            bundle->blueprint   = _ESBManager::ESBManager::trim(_match[3]);
            bundle->spring      = _ESBManager::ESBManager::trim(_match[4]);
            bundle->level       = _ESBManager::ESBManager::trim(_match[5]);
            bundle->name        = _ESBManager::ESBManager::trim(_match[6]);

            bundles[bundle->name] = bundle;
        }
    }

    return bundles;
}

std::vector<std::string> _ESBManager::ESBManager::GetDeadBundles(std::map<std::string, _ESBManager::Bundle*> bundlesWithNameAndId)
{
    std::vector<std::string> deadBundles = {};
    for(auto bundle : bundlesWithNameAndId)
    {
        auto _bundleObject = bundle.second;
        if(_bundleObject->state.compare("Dead") == 0)
            deadBundles.push_back(_bundleObject->id);
    }

    return deadBundles;
}

std::vector<std::string> _ESBManager::ESBManager::GetFailedBundles(std::map<std::string, _ESBManager::Bundle*> bundlesWithNameAndId)
{
    std::vector<std::string> failedBundles = {};
    for(auto bundle : bundlesWithNameAndId)
    {
        auto _bundleObject = bundle.second;
        if(_bundleObject->state.compare("Failed") == 0)
            failedBundles.push_back(_bundleObject->id);
    }

    return failedBundles;
}

std::vector<std::string> _ESBManager::ESBManager::GetListOfBundlesWhoesStateShouldBeEscalated(std::map<std::string, _ESBManager::Bundle*> bundlesWithNameAndId)
{
    //To do
    return std::vector<std::string>();
}

std::string _ESBManager::ESBManager::BuildBundlesToStart(std::map<std::string, _ESBManager::Bundle*> bundlesWithNameAndId,
        std::vector<std::string> deadBundles,
        std::vector<std::string> failedBundles,
        std::vector<std::string> bundleKeepAliveList,
        std::vector<std::string> systemBundleList)
{
    std::string bundleStartIdString = "";

    for(std::string _bundleName : systemBundleList)
    {
        if(bundlesWithNameAndId.find(_bundleName) != bundlesWithNameAndId.end())
        {
            _ESBManager::Bundle* bundle = bundlesWithNameAndId[_bundleName];

            if((bundle->spring != "Started") && (bundle->spring != "Waiting"))
                bundleStartIdString += bundle->id + " ";
        }
    }

    for(std::string _bundleName : bundleKeepAliveList)
    {
        if(bundlesWithNameAndId.find(_bundleName) != bundlesWithNameAndId.end())
        {
            _ESBManager::Bundle* bundle = bundlesWithNameAndId[_bundleName];

            if((bundle->spring != "Started") && (bundle->spring != "Waiting"))
                bundleStartIdString += bundle->id + " ";
        }
    }

    return bundleStartIdString;
}

std::string _ESBManager::ESBManager::BuildBundlesToStop(std::map<std::string,  _ESBManager::Bundle*> bundlesWithNameAndId, std::vector<std::string> bundleStopList)
{
    //To do
    return "";
}

_ESBManager::SystemInfo _ESBManager::ESBManager::GetSystemInfo(std::string systemInfoText)
{
    //To do
    std::string tempString;
    std::vector<std::string> _infoText;
    _ESBManager::SystemInfo systemInfo;

    boost::split(_infoText, systemInfoText, boost::is_any_of("\n"));
    for(unsigned int i = 0; i < _infoText.size(); i++)
    {
        std::smatch _match;
        std::string eachTextLine = _infoText[i];

        if(std::regex_search( eachTextLine, _match, _ESBManager::ESBManager::systemInfoParser ))
        {
            std::string name  = _ESBManager::ESBManager::trim(_match[1]);
            std::string value = _ESBManager::ESBManager::trim(_match[3]);
            systemInfo.setValue(name, value);
        }
    }

    return systemInfo;
}

void* _ESBManager::ESBManager::StartMonitorService(void* threadPparam)
{
    std::map<std::string, _ESBManager::Bundle*> bundlesWithNameAndId;
    std::vector<std::string> deadBundles;
    std::vector<std::string> failedBundles;
    std::vector<std::string> inferBundlesToEscalateByNotitifacation;

    while (_ESBManager::ESBManager::continueRunning)
    {
        if(_ESBManager::ESBManager::IsGogoReady() && !_ESBManager::ESBManager::IsFireCommandOperationInProgress())
        {
            if(_ESBManager::ESBManager::getLastCommandStr().compare(GOGO_LIST) == 0)
            {
                char strBuff[1024] = {0};
                sprintf(strBuff, GOGO_LIST.c_str(), _ESBManager::ESBManager::gogoUserName.c_str(), _ESBManager::ESBManager::gogoPassword.c_str());
                std::string command = _ESBManager::ESBManager::workingDir + "/bin\\" + strBuff;

                std::string bundleList  = _ESBManager::ESBManager::FireCommandExecVP(command);
                bundlesWithNameAndId    = _ESBManager::ESBManager::MapBundleNamesToBundlesId(bundleList);
                deadBundles             = _ESBManager::ESBManager::GetDeadBundles(bundlesWithNameAndId);
                failedBundles           = _ESBManager::ESBManager::GetFailedBundles(bundlesWithNameAndId);
                inferBundlesToEscalateByNotitifacation = _ESBManager::ESBManager::GetListOfBundlesWhoesStateShouldBeEscalated(bundlesWithNameAndId);

                _ESBManager::ESBManager::bundlesToStart = _ESBManager::ESBManager::BuildBundlesToStart(bundlesWithNameAndId, deadBundles, failedBundles, _ESBManager::ESBManager::processBundles, _ESBManager::ESBManager::systemBundles);
                //_ESBManager::ESBManager::bundlesToStop  = _ESBManager::ESBManager::BuildBundlesToStop(bundlesWithNameAndId, bundleStopList);

                _ESBManager::ESBManager::setLastCommandStr(GOGO_START_BUNDLE);
            }
            else if(_ESBManager::ESBManager::getLastCommandStr().compare(GOGO_GET_SYSTEM_INFO) == 0)
            {
                char strBuff[1024] = {0};
                sprintf(strBuff, GOGO_GET_SYSTEM_INFO.c_str(), (const*)_ESBManager::ESBManager::gogoUserName.c_str(), _ESBManager::ESBManager::gogoPassword.c_str(), _ESBManager::ESBManager::bundlesToStart.c_str());
                std::string command = _ESBManager::ESBManager::workingDir + "/bin\\" + strBuff;

                std::string systemInfo = _ESBManager::ESBManager::FireCommandExecVP(command);
                _ESBManager::ESBManager::currentSystemInfo = _ESBManager::ESBManager::GetSystemInfo(systemInfo);
                _ESBManager::ESBManager::setLastCommandStr(GOGO_LIST);
            }
            else if(_ESBManager::ESBManager::getLastCommandStr().compare(GOGO_START_BUNDLE) == 0)
            {
                char strBuff[1024] = {0};
                sprintf(strBuff, GOGO_START_BUNDLE.c_str(), (const*)_ESBManager::ESBManager::gogoUserName.c_str(), _ESBManager::ESBManager::gogoPassword.c_str(), _ESBManager::ESBManager::bundlesToStart.c_str());
                std::string command = _ESBManager::ESBManager::workingDir + "/bin\\" + strBuff;

                _ESBManager::ESBManager::FireCommandExecVP(command);
                _ESBManager::ESBManager::setLastCommandStr(GOGO_STOP_BUNDLE);
            }
            else if(_ESBManager::ESBManager::getLastCommandStr().compare(GOGO_STOP_BUNDLE) == 0)
            {
                char strBuff[1024] = {0};
                sprintf(strBuff, GOGO_STOP_BUNDLE.c_str(), _ESBManager::ESBManager::gogoUserName.c_str(), _ESBManager::ESBManager::gogoPassword.c_str(), _ESBManager::ESBManager::bundlesToStop.c_str());
                std::string command = _ESBManager::ESBManager::workingDir + "/bin\\" + strBuff;

                _ESBManager::ESBManager::FireCommandExecVP(command);
                _ESBManager::ESBManager::setLastCommandStr(GOGO_GET_SYSTEM_INFO);
            }
            else
            {
                _ESBManager::ESBManager::setLastCommandStr(GOGO_LIST);
            }
        }

        Sleep(THREAD_SLEEP_TIME);
    }

    return NULL;
}

std::string _ESBManager::ESBManager::FireCommandExecVP(std::string command)
{
    std::string streamContent = "";
    FILE* fPipe = popen(command.c_str(), "r");
    char charToRead[CHARACTER_STREAM_BUFFER_LENGTH] = {0};

    if(fPipe != NULL)
    {
        while(true)
        {
            memset(charToRead, 0, CHARACTER_STREAM_BUFFER_LENGTH);
            if (fgets(charToRead, (6), fPipe))
            {
                streamContent.append(charToRead);
//                printf(charToRead);
            }

            else break;
        }

    }
    else
    {
        printf(std::string("\nError starting child process with command [ " + command + " ] | %s\n").c_str(),strerror(errno));
    }

    pclose(fPipe);
    return streamContent;
}

std::vector<std::string>*  _ESBManager::ESBManager::ProcessDeadBundlesData(std::string deadBundledata)
{
    return nullptr;
}

std::vector<std::string>*  _ESBManager::ESBManager::ProcessStartedBundlesData(std::string startedBundleData)
{
    return nullptr;
}

void* _ESBManager::ESBManager::ProcessSystemInfoData(std::string infoData)
{
    return nullptr;
}

bool _ESBManager::ESBManager::IsServiceStarted(std::string commandData)
{
    return commandData.size() > RETURNED_FAILURE_DATALENGTH ? false : true;
}

void _ESBManager::ESBManager::GogoReady(bool IsGogoReady)
{
    pthread_mutex_lock(gogoReadyMutext.get());
    gogoReady = IsGogoReady;
    pthread_mutex_unlock(gogoReadyMutext.get());
}

bool _ESBManager::ESBManager::IsGogoReady()
{
    return gogoReady;
}

void _ESBManager::ESBManager::FireCommandOperationInProgress(bool IsFireCommandOperationInProgress)
{
    pthread_mutex_lock(fireCommandOperationInProgressMutext.get());
    fireCommandOperationInProgress = IsFireCommandOperationInProgress;
    pthread_mutex_unlock(fireCommandOperationInProgressMutext.get());
}

bool _ESBManager::ESBManager::IsFireCommandOperationInProgress()
{
    return fireCommandOperationInProgress;
}
