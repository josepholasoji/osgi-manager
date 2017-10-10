#ifndef BUNDLEMETRICSHANDLER_H
#define BUNDLEMETRICSHANDLER_H

#include <ESBManager.h>
#include <request.h>
#include <requestHandle.h>


class BundleMetricsHandler : public requestHandle
{
    public:
        BundleMetricsHandler();
        virtual ~BundleMetricsHandler();

        std::string handle(http::server::request request);
    protected:

    private:
};

#endif // BUNDLEMETRICSHANDLER_H
