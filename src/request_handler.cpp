//
// request_handler.cpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "request_handler.h"
#include <fstream>
#include <sstream>
#include <memory>
#include <string>
#include "mime_types.h"
#include "reply.h"
#include <map>
#include "request.h"
#include "AppStatusHandler.h"
#include "BundleMetricsHandler.h"

std::map<std::string, void*> http::server::request_handler::api_handler =
            {
                std::pair<std::string, void*>("/status", new AppStatusHandler()),
                std::pair<std::string, void*>("/metric", new BundleMetricsHandler())
            };

namespace http {
    namespace server {

        request_handler::request_handler(const std::string& doc_root)
        : doc_root_(doc_root)
        {
        }

        void request_handler::handle_request(const request& req, reply& rep) {
            // Decode url to path.
            std::string request_path;
            if (!url_decode(req.uri, request_path)) {
                rep = reply::stock_reply(reply::bad_request);
                return;
            }

            // Request path must be absolute and not contain "..".
            if (request_path.empty() || request_path[0] != '/'
                    || request_path.find("..") != std::string::npos) {
                rep = reply::stock_reply(reply::bad_request);
                return;
            }

            try
            {
                auto handlerObject = (handler*)http::server::request_handler::api_handler[request_path];
                const void* _request = &req;
                void* _response = &rep;
                handlerObject->handle(_request, _response);
            }
            catch(std::exception ex)
            {
                // Open the file to send back.
                std::string full_path = doc_root_ + request_path;
                std::ifstream is(full_path.c_str(), std::ios::in | std::ios::binary);
                if (!is) {
                    rep = reply::stock_reply(reply::not_found);
                    return;
                }
            }
        }

        bool request_handler::url_decode(const std::string& in, std::string& out) {
            out.clear();
            out.reserve(in.size());
            for (std::size_t i = 0; i < in.size(); ++i) {
                if (in[i] == '%') {
                    if (i + 3 <= in.size()) {
                        int value = 0;
                        std::istringstream is(in.substr(i + 1, 2));
                        if (is >> std::hex >> value) {
                            out += static_cast<char> (value);
                            i += 2;
                        } else {
                            return false;
                        }
                    } else {
                        return false;
                    }
                } else if (in[i] == '+') {
                    out += ' ';
                } else {
                    out += in[i];
                }
            }
            return true;
        }

    } // namespace server
} // namespace http
