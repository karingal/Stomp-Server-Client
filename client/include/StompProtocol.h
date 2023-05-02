#pragma once

#include "../include/ConnectionHandler.h"
#include "../include/User.h"
#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include <list>


using std::unordered_map;
using std::string;
using std::vector;
using namespace std;

class StompProtocol {
private:
static int subIdCounter; 



public:
    string body;
    string stompCommand;
    unordered_map<string,string> headers;
    StompProtocol();
    StompProtocol(vector<string> message);
    string toString();
    void addHeader(string header);
    void setStompCommand(string command);
    static vector<string> splitString(string messageTosplit);
    static list<string> createStringToFrame(vector<string> message, User& currUser);

};

