#define RAPIDJSON_HAS_STDSTRING 1

#include <ESBManager.h>
#include "BundleMetricsHandler.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "request.h"
#include "reply.h"
#include "mime_types.h"

BundleMetricsHandler::BundleMetricsHandler()
{
    //ctor
}

BundleMetricsHandler::~BundleMetricsHandler()
{
    //dtor
}

std::string BundleMetricsHandler::handle(const void* request, void* _rep)
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

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);

    std::string extension = "json";
    // Fill out the reply to be sent to the client.
    rep->status = http::server::reply::ok;

    rep->content = buffer.GetString();
    rep->headers.resize(2);
    rep->headers[0].name = "Content-Length";
    rep->headers[0].value = std::to_string(rep->content.size());
    rep->headers[1].name = "Content-Type";
    rep->headers[1].value = http::server::mime_types::extension_to_type(extension);

    return rep->content;
}
