#include "HttpHandler.h"

HttpHandler::HttpHandler(const utility::string_t& url, BrainChain& bc) : m_listener(url), bc(bc), uri(url)
{
    std::cout << "OPENING LISTENER FOR: " + uri << std::endl;

    open().wait();
    m_listener.support(methods::GET, std::bind(&HttpHandler::handle_get, this, std::placeholders::_1));
    m_listener.support(methods::POST, std::bind(&HttpHandler::handle_post, this, std::placeholders::_1));
}

pplx::task<void> HttpHandler::open()
{
    return m_listener.open();
}

pplx::task<void> HttpHandler::close()
{
    return m_listener.close();
}

void HttpHandler::handle_get(const http_request& message)
{
    if(uri == "http://" + getLocalIPAddress() + ":8081/BrainChain/Think/")
    {
        if(!bc.isEmpty())
        {
            bool result = bc.think();
            if(!result)
            {
                message.reply(status_codes::BadRequest, "No thoughts");
            }
            else
            {
                //Make sure we got the longest chain
                int length = bc.getLength();
                post_consensus();
                if(length == bc.getLength())
                {
                    auto brain = json::value::parse(bc.getChain());
                    int lastIndex = brain.as_array().size();
                    auto last = brain.as_array().at(lastIndex-1);

                    post_announceNew(last);
                }
                message.reply(status_codes::OK, "Brain Mined Successfully");
            }

            return;
        }
        else
        {
            message.reply(status_codes::BadRequest, "Brain Chain Non-Existant");
        }
    }

    if(uri == "http://" + getLocalIPAddress() + ":8081/BrainChain/Show/")
    {
        web::json::value jsonObject;
        jsonObject["chain"] = web::json::value::parse(bc.getChain());
        message.reply(status_codes::OK, jsonObject);
        return;
    }

    message.reply(status_codes::NotFound, "NOT FOUND");
}

void HttpHandler::handle_post(const http_request& message)
{
    if(uri == "http://" + getLocalIPAddress() + ":8081/BrainChain/Start/")
    {
        bc.createGenesisBrain();
        message.reply(status_codes::OK, "Starting New BrainChain");
        return;
    }

    if(uri == "http://" + getLocalIPAddress() + ":8081/BrainChain/Register/")
    {
        web::json::value jsonObject;

        try
        {
            const std::string& hostAddress = message.remote_address();
            bc.addToPeers(hostAddress);
            jsonObject["chain"] = web::json::value::parse(bc.getChain());
            jsonObject["addresses"] = web::json::value::parse(bc.getPeers());
            message.reply(status_codes::OK, jsonObject);
        }
        catch (const std::exception& e)
        {
            std::cout << e.what() << std::endl;
            std::cout << "ERROR: ADDRESS or CHAIN NOT COLLECTED" << std::endl;
            message.reply(status_codes::BadRequest);
        }

        return;
    }

    if(uri == "http://" + getLocalIPAddress() + ":8081/BrainChain/Pend/")
    {
        if(!bc.isEmpty())
        {
            web::json::value json = message.extract_json().get();

            try
            {
                std::string ts = json["timestamp"].as_string();
                std::string auth = json["author"].as_string();
                std::string thought = json["thought"].as_string();

                bc.addToPending(ts,auth,thought);
                message.reply(status_codes::OK, "PENDING");
                return;
            }
            catch(const std::exception& e)
            {
                std::cout << e.what() << std::endl;
                std::cout << "ERROR: MISSING REQUIRED FIELDS PEND" << std::endl;
                message.reply(status_codes::BadRequest);
            }
        }
        else
        {
            message.reply(status_codes::BadRequest, "Brain Chain Non-Existant");
        }
    }

    if(uri == "http://" + getLocalIPAddress() + ":8081/BrainChain/Add/")
    {
        web::json::value json = message.extract_json().get();

        try
        {
            auto brain = json["brain"];
            std::string hash = json["hash"].as_string();
            bool result = bc.addNoTrain(brain, hash);
            if(result)
            {
                message.reply(status_codes::Created, "Brain Added to the Chain Successfully");
            }
            else
            {
                message.reply(status_codes::BadRequest, "Brain wasn't added to the chain");
            }

            return;
        }
        catch(const std::exception& e)
        {
            std::cout << e.what() << std::endl;
            std::cout << "ERROR: MISSING REQUIRED FIELDS TO ADD" << std::endl;
            message.reply(status_codes::BadRequest);
        }
    }

    message.reply(status_codes::NotFound);
}

void HttpHandler::post_start()
{
    auto task = pplx::create_task([this]()
    {
        return http_client(U("http://" + getLocalIPAddress() + ":8081/BrainChain/Start/")).request(methods::POST);
    });

    // Wait for all the outstanding I/O to complete and handle any exceptions
    try
    {
        task.wait();
    }
    catch (const std::exception &e)
    {
        printf("Error exception:%s\n", e.what());
    }
}

void HttpHandler::post_register(std::string ip)
{
    auto task = pplx::create_task([=]()
    {
        return http_client(U("http://" + ip + ":8081/BrainChain/Register/")).request(methods::POST);
    })
    .then([=](const http_response& response)
    {
        if (response.status_code() == 200)
        {
            bc.addToPeers(ip);
            web::json::value json = response.extract_json().get();
            try
            {
                auto chain = json.at("chain");
                auto peers = json.at("addresses");

                //Update peers list with response
                auto array = json.at(U("addresses")).as_array();
                if(array.size() != 0)
                {
                    for(auto & i : array)
                    {
                        if(i.as_string() != getLocalIPAddress())
                            bc.addToPeers(i.as_string());
                    }
                }

                bc.initChain(chain);

                std::cout << "Status Code: " << std::to_string(response.status_code()) << std::endl;
                std::cout << "Registration to node at " << ip << " successful" << std::endl;
            }
            catch (const std::exception& e)
            {
                std::cout << e.what() << std::endl;
                std::cout << "ERROR: ADDRESS NOT COLLECTED RESPONSE" << std::endl;
            }
        }
        else
        {
            std::cout << "Status Code " << std::to_string(response.status_code()) << std::endl;
            std::cout << "Registration to node at " << ip << "failed" << std::endl;
        }
    });


    // Wait for all the outstanding I/O to complete and handle any exceptions
    try
    {
        task.wait();
    }
    catch (const std::exception &e)
    {
        printf("Error exception:%s\n", e.what());
    }
}

void HttpHandler::post_entry(std::string timestamp, std::string author, std::string thought)
{
    auto task = pplx::create_task([=]()
    {
        json::value jsonObject;
        jsonObject[U("timestamp")] = json::value::string(U(timestamp));
        jsonObject[U("author")] = json::value::string(U(author));
        jsonObject[U("thought")] = json::value::string(U(thought));

        return http_client(U("http://"+ getLocalIPAddress() + ":8081/BrainChain/Pend/")).request(methods::POST, "", jsonObject.serialize(), U("application/json"));
    })
    .then([=](const http_response& response)
    {
        if (response.status_code() == 200)
        {
            std::cout << "Status Code " << std::to_string(response.status_code()) << std::endl;
            std::cout << "Status Thought Added Successfully " << std::to_string(response.status_code()) << std::endl;
        }
        else
        {
            std::cout << "Status Code: " << std::to_string(response.status_code()) << std::endl;
            std::cout << "Message: " << response.extract_string().get() << std::endl;
        }
    });

    // Wait for all the outstanding I/O to complete and handle any exceptions
    try
    {
        task.wait();
    }
    catch (const std::exception &e)
    {
        printf("Error exception:%s\n", e.what());
    }
}

void HttpHandler::get_think()
{
    auto task = pplx::create_task([]()
    {
        return http_client(U("http://" + getLocalIPAddress() +  ":8081/BrainChain/Think/")).request(methods::GET);
    }).then([=](const http_response& response)
    {
        if (response.status_code() == 200)
        {
            std::cout << "Status Code " << std::to_string(response.status_code()) << std::endl;
            std::cout << "Thought Added Successfully " << std::endl;
        }
        else
        {
            std::cout << "Status Code: " << std::to_string(response.status_code()) << std::endl;
            std::cout << "Message: " << response.extract_string().get() << std::endl;
        }
    });

    // Wait for all the outstanding I/O to complete and handle any exceptions
    try
    {
        task.wait();
    }
    catch (const std::exception &e)
    {
        printf("Error exception:%s\n", e.what());
    }
}

bool HttpHandler::post_consensus()
{
    json::value longestChain;
    int currentLength = bc.getLength();

    web::json::array addressArray = web::json::value::parse(bc.getPeers()).as_array();

    //Iterate through list of peer addresses
    for(auto & i : addressArray)
    {
        const std::string& ip = i.as_string();

        //Request a copy of every peers chain
        auto task = pplx::create_task([=]()
        {
            return http_client(U("http://" + ip + ":8081/BrainChain/Show/")).request(methods::GET);
        })
        .then([&](const http_response& response)
        {
            web::json::value json = response.extract_json().get();

            if (response.status_code() == 200)
            {
                try
                {
                    int length = json.at(U("chain")).as_array().size();
                    auto chain = json.at(U("chain"));

                    //If chain is longer than the this nodes copy then set it
                    //as the new longest chain
                    if(length > currentLength && BrainChain::verifyIntegrity(chain))
                    {
                        currentLength = length;
                        longestChain = chain;
                    }
                }
                catch (const std::exception& e)
                {
                    std::cout << e.what() << std::endl;
                    std::cout << "ERROR: CHAIN NOT COLLECTED RESPONSE" << std::endl;
                }
            }
            else
            {
                std::cout << "Status Code not 200" << std::endl;
            }
        });
        try
        {
            task.wait();
        }
        catch (const std::exception &e)
        {
            printf("Error exception:%s\n", e.what());
        }
    }

    if(!longestChain.is_null())
    {
        bc.initChain(longestChain);
        return true;
    }

    return false;
}

void HttpHandler::post_announceNew(const web::json::value& last)
{
    web::json::array addressArray = web::json::value::parse(bc.getPeers()).as_array();

    //Iterate through list of peer addresses
    for(auto & i : addressArray)
    {
        const std::string& ip = i.as_string();

        //Request a copy of every peers chain
        auto task = pplx::create_task([=]()
        {
            return http_client(U("http://" + ip + ":8081/BrainChain/Add/")).request(methods::POST, "", last.serialize(), U("application/json"));
        });

        try
        {
            task.wait();
        }
        catch (const std::exception &e)
        {
            printf("Error exception:%s\n", e.what());
        }
    }
}

std::string HttpHandler::getLocalIPAddress()
{
    struct ifaddrs *ifaddr, *ifa;
    int s;
    char host[NI_MAXHOST];

    if(getifaddrs(&ifaddr) == -1)
    {
        perror("getifaddrs");
    }

    for(ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next)
    {
        if(ifa->ifa_addr == nullptr)
            continue;

        s=getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host, NI_MAXHOST, nullptr, 0, NI_NUMERICHOST);

        if((strcmp(ifa->ifa_name, "wlp2s0") == 0 || strcmp(ifa->ifa_name, "enp0s3") == 0) && (ifa->ifa_addr->sa_family == AF_INET))
        {
            if(s != 0)
            {
                printf("getnameinfo() failed: %s\n", gai_strerror(s));
            }

            freeifaddrs(ifaddr);
            return host;
        }
    }

    freeifaddrs(ifaddr);
    return "";
}

HttpHandler::~HttpHandler()
{
    std::cout << "CLOSING LISTENER FOR: " + uri << std::endl;
    close().wait();
}