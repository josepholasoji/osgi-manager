#include "BundleMetricsHandler.h"
#include <request.h>
#include <requestHandle.h>

BundleMetricsHandler::BundleMetricsHandler()
{
    //ctor
}

BundleMetricsHandler::~BundleMetricsHandler()
{
    //dtor
}

std::string BundleMetricsHandler::handle(http::server::request request)
{
    std::string returnedString = "";

    int bundleIndex = 0;
    _ESBManager::Bundle* bundleArray[_ESBManager::ESBManager::GetBundlesWithNameAndId().size()];
    auto bundles = _ESBManager::ESBManager::GetBundlesWithNameAndId();
    for(const auto& bundle : bundles)
    {
        _ESBManager::Bundle *_bundle = bundle.second;
        bundleArray[bundleIndex] = _bundle;
    }

    //bundleArray.
    return returnedString;
}
