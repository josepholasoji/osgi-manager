#ifndef APPSTATUSHANDLER_H
#define APPSTATUSHANDLER_H

#include <requestHandle.h>
#include <string>
#include <request.h>


class AppStatusHandler : public requestHandle
{
    public:
        AppStatusHandler();
        virtual ~AppStatusHandler();

        std::string handle(http::server::request request);
    protected:

    private:
};

#endif // APPSTATUSHANDLER_H
