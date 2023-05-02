#include <stdlib.h>
#include "../include/ConnectionHandler.h"
#include "../include/SocketReader.h"
#include "../include/StompProtocol.h"
#include "../include/User.h"
#include <list>
#include <string>
#include <unordered_map>
#include <iostream>
#include <sstream>
#include <thread>

using namespace std;

int main(int argc, char *argv[]) {

    while (1) {
        const short bufsize = 1024;
        char buf[bufsize];
        std::cin.getline(buf, bufsize);
        std::string login(buf);

        int index1 = login.find(":");
        std::string host = login.substr(6,index1-6);
        int index2 = login.find(" ",index1);
        short port = (short)stoi(login.substr(index1+1,index2-index1-1));
    
        
        ConnectionHandler connectionHandler(host, port);
        if (!connectionHandler.connect()) {
            std::cerr << "Cannot connect to " << host << ":" << port << std::endl;
            return 1;
        }

        User currUser;
        SocketReader socketReaderThread(connectionHandler,currUser);
        std::thread th2(&SocketReader::run, &socketReaderThread);

        vector<string> splittedString = StompProtocol::splitString(login);
        list<string> framesToSend = StompProtocol::createStringToFrame(splittedString,currUser);

        for (string &currFrame : framesToSend) { 
            if (!connectionHandler.sendFrameAscii(currFrame, '\0')) {
                std::cout << "Could not connect to server" << std::endl;
                break;
            }
        }
        while (1) {
            const short bufsize = 1024;
            char buf[bufsize];
            std::cin.getline(buf, bufsize);
            std::string line(buf);
    
            vector<string> splittedString = StompProtocol::splitString(line);
            list<string> framesToSend = StompProtocol::createStringToFrame(splittedString,currUser);

            for (string &currFrame : framesToSend) { 
                if (!connectionHandler.sendFrameAscii(currFrame, '\0')) {
                    std::cout << "Could not connect to server" << std::endl;
                    break;
                }
            }

            if (splittedString[0] == "logout") {
                currUser.isConnected = false;
                break;
            }
        }
        th2.join();
    }
    return 0;
}