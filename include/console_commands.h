#ifndef CONSOLE_COMMANDS_H_INCLUDED
#define CONSOLE_COMMANDS_H_INCLUDED

    const std::string CHECK_FOR_GOGO_READY        ="\r\n";
    const std::string GOGO_READY                  ="JBossFuse:karaf@root>";
    const std::string GOGO_LIST_STARTED_BUNDLES   ="\r\nlist | grep Started";
    const std::string GOGO_LIST_DEAD_BUNDLES      ="\r\nlist | grep Failed";
    const std::string GOGO_GET_SYSTEM_INFO        ="\r\ninfo";
    const std::string GOGO_START_BUNDLE           ="\r\nstart %i";
    const std::string GOGO_STOP_BUNDLE            ="\r\nstop %i";
    const std::string GOGO_RESTART_BUNDLE         ="\r\nrestart %i";


#endif // CONSOLE_COMMANDS_H_INCLUDED
