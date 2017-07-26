#ifndef CONSOLE_COMMANDS_H_INCLUDED
#define CONSOLE_COMMANDS_H_INCLUDED

    const std::string GOGO_LIST                   ="client -h 127.0.0.1 -u %s -p %s \"list -l |grep file:/\"";
    const std::string GOGO_GET_SYSTEM_INFO        ="client -h 127.0.0.1 -u %s -p %s info";
    const std::string GOGO_START_BUNDLE           ="client -h 127.0.0.1 -u %s -p %s \"start %s\"";
    const std::string GOGO_STOP_BUNDLE           ="client -h 127.0.0.1 -u %s -p %s \"stop %s\"";

#endif // CONSOLE_COMMANDS_H_INCLUDED
