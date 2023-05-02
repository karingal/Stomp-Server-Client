#include <SocketReader.h>
#include <User.h>
#include <StompProtocol.h>
#include <ConnectionHandler.h>
#include "../include/StompProtocol.h"
#include "ConnectionHandler.h"
#include <stdlib.h>
#include <list>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <thread>
#include "../include/event.h"
#include "../include/Game.h"

using namespace std;

SocketReader::SocketReader(ConnectionHandler &ch, User &user) : connectionHandler_(ch), user_(user){
}

vector<string> SocketReader::splitByRow(string messageTosplit) {
    string tmp; 
    std::istringstream stream(messageTosplit);
    vector<string> message;

    while(getline(stream, tmp, '\n')) {
        message.push_back(tmp);
    }
    return message;  
 }

void SocketReader::run() {
    while (user_.isConnected) {
        std::string answer;
        if (!connectionHandler_.getFrameAscii(answer, '\0')) {
            std::cout << "Could not connect to server" << std::endl;
            break;
        }
        
        vector<string> splittedAnswer = splitByRow(answer);
        
        if (splittedAnswer[0].length() == 9 && splittedAnswer[0].compare("CONNECTED") == 0) {
            std::cout << "Login successful" << std::endl;
            user_.isConnected = true;
        }

        else if (splittedAnswer[0].length() == 5 && splittedAnswer[0].compare("ERROR") == 0) {
            string toCheck = "message: user already logged in";
            if (splittedAnswer[1].length() == toCheck.length() && splittedAnswer[1].compare(toCheck) == 0) {
                std::cout << "The client is already logged in" << std::endl;
            }
            user_.isConnected = false;
            connectionHandler_.close();
        }
        
        else if (splittedAnswer[0].length() == 7 && splittedAnswer[0].compare("RECEIPT") == 0) {
            user_.isConnected = false;
            connectionHandler_.close();
        }

        else if (splittedAnswer[0].length() == 7 && splittedAnswer[0].compare("MESSAGE") == 0) {
            int index = splittedAnswer[1].find(":");
            string destination = splittedAnswer[1].substr(index+1);
            int indexRow = answer.find("\n\n");
            string frameBody = answer.substr(indexRow+2);
            Event currEvent(frameBody);
            for (Game &game : user_.gamesPerClient) {
                string userName = currEvent.userName;
                if (game.gameName_.length() == destination.length() && game.gameName_.compare(destination) == 0) {
                    if (game.userToEvent.count(userName) != 0) {
                        vector<Event> &EventsListForUser = game.userToEvent[userName];
                        EventsListForUser.push_back(currEvent);
                    }
                    else {
                        vector<Event> EventsListForUser;
                        EventsListForUser.push_back(currEvent);
                        game.userToEvent[userName] = EventsListForUser;
                    }
                }
            }
        } 
    }      
}    