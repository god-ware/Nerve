![banner](https://github.com/francobel/CTF-Writeups/blob/master/Nerve/banner.png "banner")
# Nerve: Simple RESTful API Based Blockchain
### Language: C++    Dependencies: OpenSSL, cpprestsdk    Tested With: Debian
## Purpose
**Creating a Blockchain implementation simple enough for anyone to use, regardless of experience.**
 
## Compilation Instructions:
1. **Install Dependencies:** sudo apt-get install build-essential libcpprest-dev openssl-dev cmake
2. **Clone Repository:** git clone http://github.com/francobel/Nerve.git  
3. **Navigate to cloned:** cd Nerve
4. **Create Make file:** cmake .
5. **Build Application:** make
6. **Start Blockchain:** ./Nerve

## Usage Instructions: 
![menu](https://github.com/francobel/CTF-Writeups/blob/master/Nerve/menu.png "menu")\
**Option 1:** Creates a blockchain node and adds the first block to it (Genesis Block).\
**Option 2:** Allows you to join a blockchain network by entering its nodes IP address.\
**Option 3:** Adds a thought (json composed of timestamp, author, message) to a queue.\
**Option 4:** Inserts the queue of thoughts into a block, the block is then mined and added to the blockchain.\
**Option 5:** Displays the blocks that compose the blockchain of the current node.\
**Option 6:** Displays the peers that are part of the blockchain network.
## Endpoints
#### Certain options are accessible through their corresponding endpoints.
#### All of the POST endpoints take Json as payloads. This is to allow other programs to interact with the blockchain.
**Option 1:** POST http://NodeIP:8081/BrainChain/Start \
**Option 2:** POST http://NodeIP:8081/BrainChain/Register \
**Option 3:** POST http://NodeIP:8081/BrainChain/Pend \
**Option 4:** GET  http://NodeIP:8081/BrainChain/Think \
**Option 5:** GET  http://NodeIP:8081/BrainChain/Show \
 \
**DISCLAIMER: Blockchain is stored in memory. Killing a node will delete it.** \
**DISCLAIMER: Listeners for endpoints are opened at the beginning of the program.**
