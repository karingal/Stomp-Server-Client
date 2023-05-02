#include "User.h"
#include <unordered_map>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <sstream>

User::User() : gameToSubId(), subIdCounter(0), receiptIdCounter(0),userName(), isConnected(true), gamesPerClient() {
    
}

