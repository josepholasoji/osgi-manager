#ifndef BUNDLEMETRICSHANDLER_H
#define BUNDLEMETRICSHANDLER_H

#include <ESBManager.h>
#include <request.h>
#include <handler.h>


class BundleMetricsHandler : public handler
{
    public:
        BundleMetricsHandler();
        virtual ~BundleMetricsHandler();

        std::string handle(const void*, void*);
    protected:

    private:
};

#endif // BUNDLEMETRICSHANDLER_H
