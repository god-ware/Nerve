#include "BrainChain.h"

#include <utility>

BrainChain::BrainChain() = default;

void BrainChain::createGenesisBrain()
{
    brainChain.clear();

    addToPending("0/0/0", "Franco", "Genesis");

    //genesis is the block that starts the chain
    Brain genesis = Brain(jsonThoughts);
    jsonThoughts.clear();
    indexCount++;
    genesis.setIndex(indexCount);//index is set
    genesis.setPreviousHash("0000000000000000000000000000000000000000000000000000000000000000");
    genesis.setJson();
    genesis.setBhash(genesis.trainBrain(difficulty));//hash for brain is calculated

    brainChain.push_back(genesis);//genesis is added to chain
}

//Adds new brain to the chain
void BrainChain::add(Brain& brain, bool b)
{
    if(b)
    {
        indexCount++;
        brain.setIndex(indexCount);
        brain.setPreviousHash(retPreviousBhash());
        brain.setJson();
        brain.setBhash(brain.trainBrain(difficulty));
    }

    //Zeros is the string of consecutive '0's depending on difficulty
    //Ex: difficulty = 4 : "0000"
    std::string zeros(difficulty, '0');

    //Three checks are made to verify the block hasn't been tampered with.
    //1. Checking that the brains hash has the right amount of leading zeros corresponding to the difficulty
    //2. Checking that the brains previousHash variable is equal to the actual hash of the last brain in the brain chain
    //3. Checking that the hash of the brain about to be added is the value it's supposed to be

    if(brain.getBhash().substr(0,difficulty) == zeros && brain.getPreviousBhash() == retPreviousBhash() && brain.getBhash() == Brain::calculateBhash(brain.getJson()))
    {
        brainChain.push_back(brain);
    }
    else
    {
        std::cout << "ERROR: PROBLEM WITH BLOCK HASH" << std::endl;
    }
}

//Returns recalculated hash
std::string BrainChain::retPreviousBhash()
{
    //Returns the hash of the last brain in the chain
    std::string buffer = brainChain.back().getJson();
    return Brain::calculateBhash(buffer);
}

void BrainChain::addToPending(std::string timestamp, std::string author, std::string string)
{
    web::json::value thought;

    thought["timestamp"] = web::json::value::string(std::move(timestamp));
    thought["author"] = web::json::value::string(std::move(author));
    thought["string"] = web::json::value::string(std::move(string));

    jsonThoughts.push_back(thought);
}

bool BrainChain::think()
{
    if(!jsonThoughts.empty())
    {
        Brain newBrain = Brain(jsonThoughts);
        jsonThoughts.clear();
        add(newBrain, true);
        return true;
    }
    else
    {
        return false;
    }
}

std::string BrainChain::getChain()
{
    std::vector<web::json::value> chain;
    web::json::value brain;
    utility::stringstream_t stream;

    for (auto i : brainChain)
    {
        brain["hash"]  = web::json::value::string(i.getBhash(),0);
        brain["brain"] = web::json::value::parse(i.getJson());
        chain.push_back(brain);
    }
    web::json::value arr = web::json::value::array(chain);

    return arr.serialize();
}

void BrainChain::addToPeers(const std::string& address)
{
    peerAddresses.push_back(address);
    peerAddresses.unique();
}

std::string BrainChain::getPeers()
{
    std::vector<web::json::value> chain;
    web::json::value brain;
    utility::stringstream_t stream;

    web::json::value result = web::json::value::array();

    int i = 0;
    for(const std::string& p : peerAddresses)
    {
        result[i++] = web::json::value(p);
        i++;
    }

    return result.serialize();
}

void BrainChain::initChain(web::json::value chain)
{
    brainChain.clear();

    auto array = chain.as_array();
    web::json::value brain;
    web::json::value json;

    for(auto & i : array)
    {
        std::vector<web::json::value> newJson;

        std::string hash = i.at("hash").as_string();
        auto index = i.at("brain").at("index").as_string();
        auto timestamp = i.at("brain").at("timestamp").as_string();
        auto prevhash = i.at("brain").at("previous_hash").as_string();
        auto nonce = i.at("brain").at("nonce").as_string();

        auto container = i.at("brain").at("data").as_array();
        for(auto & j : container)
        {
            web::json::value jsn;
            jsn["timestamp"] = j.at("timestamp");
            jsn["author"] = j.at("author");
            jsn["string"] = j.at("string");
            newJson.push_back(jsn);
        }

        Brain newBrain = Brain(newJson);

        newBrain.setIndex(std::stoi(index));
        newBrain.setPreviousHash(prevhash);
        newBrain.setNonce(std::stoi(nonce));
        newBrain.setBhash(hash);
        newBrain.setTimestamp(timestamp);
        newBrain.setJson();

        std::cout << getChain() << std::endl;

        if(newBrain.getPreviousBhash() == "0000000000000000000000000000000000000000000000000000000000000000")
        {
            brainChain.push_back(newBrain);
        }
        else
        {
            add(newBrain, false);
        }
    }
}

bool BrainChain::verifyIntegrity(web::json::value chain)
{
    web::json::array chainArray = chain.as_array();
    std::string previousHash = "0000000000000000000000000000000000000000000000000000000000000000";

    for(auto & i : chainArray)
    {
        std::string hash = i.at("hash").as_string();
        //Turn json into string to calculate hash
        utility::stringstream_t stream;
        auto brain = i.at("brain");
        brain.serialize(stream);
        std::string json = stream.str();

        if(hash != Brain::calculateBhash(json) || previousHash != brain.at("previous_hash").as_string())
        {
            std::cout << "CHAIN IS COMPROMISED" << std::endl;
            return false;
        }

        previousHash = hash;
    }

    return true;
}

int BrainChain::getLength()
{
    return brainChain.size();
}

bool BrainChain::addNoTrain(web::json::value brain, std::string hash)
{
    web::json::array dataArray = brain.at("data").as_array();
    std::vector<web::json::value> newJson;
    for(auto & i : dataArray)
    {
        newJson.push_back(i);
    }

    Brain lzBrain = Brain(newJson);

    lzBrain.setIndex(std::stoi(brain.at("index").as_string()));
    lzBrain.setPreviousHash(brain.at("previous_hash").as_string());
    lzBrain.setNonce(std::stoi(brain.at("nonce").as_string()));
    lzBrain.setBhash(std::move(hash));
    lzBrain.setTimestamp(brain.at("timestamp").as_string());
    lzBrain.setJson();
    try
    {
        add(lzBrain, false);
        return true;
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << std::endl;
        return false;
    }
}

bool BrainChain::isEmpty()
{
    if(brainChain.empty())
    {
        return true;
    }

    return false;
}