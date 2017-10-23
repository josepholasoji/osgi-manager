#ifndef APPSTATUSHANDLER_H
#define APPSTATUSHANDLER_H

#include <request_handler.h>
#include <string>
#include <request.h>
#include <reply.h>
#include <handler.h>

struct reply;
struct request;

class AppStatusHandler : public handler
{
    public:
        AppStatusHandler();
        virtual ~AppStatusHandler();

        std::string handle(const void* request, void* reponse);
    protected:

    private:
};

#endif // APPSTATUSHANDLER_H
