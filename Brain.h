#ifndef BRAIN_H
#define BRAIN_H

#include <openssl/sha.h>
#include <iostream>
#include <iomanip>
#include <utility>
#include <string>
#include <json.h>
#include <chrono>

class Brain
{
public:
    explicit Brain(std::vector<web::json::value> dta);

    static std::string calculateBhash(const std::string& str);
    static std::string calculateTimestamp();
           std::string trainBrain(int difficulty);

    void setIndex(int i);
    void setBhash(std::string hash);
    void setTimestamp(std::string ts);
    void setPreviousHash(std::string prevBhash);
    void setNonce(int nonse);
    void setJson();

    std::string getIndex();
    std::string getTimeStamp();
    std::vector<web::json::value>getData();
    std::string getBhash();
    std::string getPreviousBhash();
    std::string getNonce();
    std::string getJson();

private:
            int index = -1;
            int nonce = 0;
    std::string timestamp;
    std::vector<web::json::value> data;
    std::string previousHash;
    std::string bhash;
    std::string jsonData;
};

#endif // BRAIN_H
