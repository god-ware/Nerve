#ifndef BRAINCHAIN_H
#define BRAINCHAIN_H

#include "Brain.h"
#include <list>

class BrainChain
{
public:
    BrainChain();

    void add(Brain& brain, bool b);
    bool addNoTrain(web::json::value chain, std::string hash);
    void createGenesisBrain();

    std::string retPreviousBhash();
    void addToPending(std::string timestamp, std::string author, std::string thought);
    bool think();
    void addToPeers(const std::string& address);
    std::string getPeers();
    std::string getChain();
    int getLength();
    void initChain(web::json::value chain);
    static bool verifyIntegrity(web::json::value chain);
    bool isEmpty();

private:
    std::list<Brain> brainChain;
    std::vector<web::json::value> jsonThoughts;
    std::list<std::string> peerAddresses;
    int indexCount = -1;
    int difficulty = 3;//SET DIFFICULTY MANUALY HERE FOR NOW
};

#endif // BRAINCHAIN_H
