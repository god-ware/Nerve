#include "Brain.h"

Brain::Brain(std::vector<web::json::value> dta)
{
    timestamp = calculateTimestamp();
    data = std::move(dta);
}

std::string Brain::trainBrain(int difficulty)
{
    //String constructor will repeat '0's corresponding to difficulty
    std::string zeros(difficulty, '0');

    while(bhash.substr(0, difficulty) != zeros)
    {
        nonce++;
        setJson();
        bhash = calculateBhash(jsonData);
    }
    return bhash;
}

//Returns current time and date
std::string Brain::calculateTimestamp()
{
    auto end = std::chrono::system_clock::now();
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
    std::string time =  std::ctime(&end_time);
    time.pop_back(); //Remove trailing \n

    return time;
}

void Brain::setIndex(int i)
{
    index = i;
}

void Brain::setBhash(std::string hash)
{
    bhash = std::move(hash);
}

void Brain::setTimestamp(std::string ts)
{
    timestamp = std::move(ts);
}

void Brain::setPreviousHash(std::string prevBhash)
{
    previousHash = std::move(prevBhash);
}

void Brain::setNonce(int nonse)
{
    nonce = nonse;
}

void Brain::setJson()
{
    utility::stringstream_t stream;
    web::json::value json;

    json["index"]         = web::json::value::string(getIndex());
    json["timestamp"]     = web::json::value::string(getTimeStamp());
    json["previous_hash"] = web::json::value::string(getPreviousBhash());
    json["nonce"]         = web::json::value::string(getNonce());
    json["data"]          = web::json::value::array(getData());

    json.serialize(stream);

    jsonData = stream.str();
}

std::string Brain::getIndex()
{
    return std::to_string(index);
}

std::vector<web::json::value> Brain::getData()
{
    return data;
}

std::string Brain::getBhash()
{
    return bhash;
}

std::string Brain::getPreviousBhash()
{
    return previousHash;
}

std::string Brain::getTimeStamp()
{
    return timestamp;
}

std::string Brain::getNonce()
{
    return std::to_string(nonce);
}

std::string Brain::getJson()
{
    return jsonData;
}

std::string Brain::calculateBhash(const std::string& str)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);
    std::stringstream ss;
    for(unsigned char i : hash)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)i;
    }

    return ss.str();
}