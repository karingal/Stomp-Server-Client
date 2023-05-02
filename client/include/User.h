#pragma once

#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include <list>
#include "../include/Game.h"

using std::unordered_map;
using std::string;
using std::vector;
using namespace std;

class User {
private:
public:
    User();
    unordered_map<string,int> gameToSubId;
    int subIdCounter;
    int receiptIdCounter;
    string userName;
    volatile bool isConnected;
    list<Game> gamesPerClient;
};