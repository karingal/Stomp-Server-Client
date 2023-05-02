#pragma once

#include "../include/ConnectionHandler.h"
#include "../include/User.h"
#include <thread>
#include <iostream>

class SocketReader {
private:
   ConnectionHandler &connectionHandler_;
   User &user_;
public:
   SocketReader(ConnectionHandler &connectionHandler, User &user);
   static vector<string> splitByRow(string messageTosplit);
   void run(); 
};