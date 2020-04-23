#ifndef NERVE_HTTPHANDLER_H
#define NERVE_HTTPHANDLER_H

#include "cpprest/http_listener.h"
#include "cpprest/asyncrt_utils.h"
#include "cpprest/http_client.h"
#include "cpprest/base_uri.h"
#include "cpprest/uri.h"
#include "BrainChain.h"
#include <ifaddrs.h>
#include <iostream>

using namespace web;
using namespace http;
using namespace utility;
using namespace client;
using namespace http::experimental::listener;

class HttpHandler
{
public:
    HttpHandler(const utility::string_t& url, BrainChain& bc);
    ~HttpHandler();

    void post_start();
    void post_register(std::string ip);//Register to an existing system
    static void post_entry(std::string timestamp, std::string author, std::string thought);//New entry to queue to be added to block later
    static void get_think();//Add block to the chain
    static std::string getLocalIPAddress();

private:
    pplx::task<void> open();
    pplx::task<void> close();

    void handle_get(const http_request& message);
    void handle_post(const http_request& message);

    bool post_consensus();
    void post_announceNew(const web::json::value& last);

    http_listener m_listener;
    BrainChain&   bc;
    std::string   uri;
};

#endif //NERVE_HTTPHANDLER_H
