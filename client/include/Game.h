#pragma once

#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <list>
#include "../include/event.h"

using namespace std;

class Game {
private:

public:
    Game(string gameName);
    void createSummary(string userName, string file);
    map<string,vector<Event>> userToEvent;
    string gameName_;
};