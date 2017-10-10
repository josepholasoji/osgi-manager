#ifndef HANDLER_H
#define HANDLER_H

#include <string>

class requestHandle
{
    public:
        requestHandle();
        virtual ~requestHandle();

        virtual std::string handle(std::string schema, std::string method, std::string path);
    protected:

    private:
};

#endif // HANDLER_H
