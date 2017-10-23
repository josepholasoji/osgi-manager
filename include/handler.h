#ifndef HANDLER_H
#define HANDLER_H

#include <string>

class handler
{
    public:
        handler();
        virtual ~handler();

        virtual std::string handle(const void* request, void* response)=0;
    protected:

    private:
};

#endif // HANDLER_H
