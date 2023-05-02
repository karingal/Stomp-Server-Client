#include "../include/StompProtocol.h"
#include "../include/event.h"
#include "../include/User.h"
#include <boost/algorithm/string.hpp>
#include "../include/Game.h"
#include <iostream>
#include <string>
#include <list>


using namespace boost::algorithm;
using namespace std;

StompProtocol::StompProtocol() : body(), stompCommand(), headers()
{
}

vector<string> StompProtocol::splitString(string messageTosplit) {
    string tmp; 
    std::istringstream stream(messageTosplit);
    vector<string> message;

    while(getline(stream, tmp, ' ')) {
        message.push_back(tmp);
    }
    return message;  
 }

list<string> StompProtocol::createStringToFrame(vector<string> message, User &currUser) {
    string ans;
    string newLine = "\n";
    list<string> ansList;

    if (message[0] == "login") {
        string login = message[2];
        currUser.userName = login;
        string passcode = message[3];
        ans = "CONNECT" + newLine +
            "accept-version:1.2" + newLine +
            "host:stomp.cs.bgu.ac.il" + newLine + 
            "login:" + login + newLine + 
            "passcode:" + passcode + newLine +
            newLine + 
            "\0";
        ansList.push_back(ans);
    }

    else if (message[0] == "join") {
        string destination = message[1];    
        string subIdCounterStr = std::to_string(currUser.subIdCounter);
        ans = "SUBSCRIBE" + newLine +
            "destination:" + destination + newLine +
            "id:" + subIdCounterStr + newLine +
            newLine +
            "\0";
        currUser.gameToSubId[message[1]] = currUser.subIdCounter;
        currUser.subIdCounter++;
        Game newGame(destination);
        currUser.gamesPerClient.push_back(newGame);
        ansList.push_back(ans);  
    }

    else if (message[0] == "exit") {
        string gameName = message[1];
        if (currUser.gameToSubId.count(gameName) != 0) {
            int subId = currUser.gameToSubId.at(gameName);
            string subIdStr = std::to_string(subId);
            ans = "UNSUBSCRIBE" + newLine +
                "id:" + subIdStr + newLine +
                newLine +
                "\0";
            currUser.gameToSubId.erase(gameName);
            currUser.gamesPerClient.remove_if([gameName](Game game){return game.gameName_==gameName;});
            ansList.push_back(ans);
        }

    }

    else if (message[0] == "report") {
        string filePath = message[1];
        names_and_events nne = parseEventsFile(filePath);
        for (Event &currEvent : nne.events) {
            string generalUpdatesStr = "";
            for (auto& kv : currEvent.get_game_updates()) {
                generalUpdatesStr += kv.first + ":" + kv.second + newLine;
            }
            string generalUpdatesTeamAStr = "";
            for (auto& kv : currEvent.get_team_a_updates()) {
                generalUpdatesTeamAStr += kv.first + ":" + kv.second + newLine;
            }
            string generalUpdatesTeamBStr = "";
            for (auto& kv : currEvent.get_team_b_updates()) {
                generalUpdatesTeamBStr += kv.first + ":" + kv.second + newLine;
            }
            string destination = nne.team_a_name + "_" + nne.team_b_name;
            ans = "SEND" + newLine +
                "destination:" +  destination + newLine +
                newLine +
                "user:" + currUser.userName + newLine +
                "team a:" + nne.team_a_name + newLine +
                "team b:" + nne.team_b_name + newLine +
                "event name:" + currEvent.get_name() + newLine +
                "time:" + std::to_string(currEvent.get_time()) + newLine +
                "general game updates:" + newLine +
                generalUpdatesStr + 
                "team a updates:" + newLine + 
                generalUpdatesTeamAStr +
                "team b updates:" + newLine + 
                generalUpdatesTeamBStr +
                "description:" + newLine + 
                currEvent.get_description() + newLine +
                "\0";
            ansList.push_back(ans);
        }
    }

    else if (message[0] == "summary") {
        string gameName = message[1];
        string userName = message[2];
        string file = message[3];
        for (Game &currGame : currUser.gamesPerClient) {
            string currGameName = currGame.gameName_;
            if (gameName.length() == currGameName.length() && gameName.compare(currGameName) == 0) {
                currGame.createSummary(userName,file);
            }
        }
    }

    else if (message[0] == "logout") {
        int receiptId = currUser.receiptIdCounter;
        string receiptIdStr = std::to_string(receiptId);
        ans = "DISCONNECT" + newLine +
            "receipt:" + receiptIdStr + newLine +
            newLine +  
            "\0";
        currUser.receiptIdCounter++;
        ansList.push_back(ans);
    }
    return ansList;          
 }
