#include "../include/event.h"
#include "../include/json.hpp"
#include "../include/SocketReader.h"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
using json = nlohmann::json;
using namespace std;

Event::Event(std::string team_a_name, std::string team_b_name, std::string name, int time,
             std::map<std::string, std::string> game_updates, std::map<std::string, std::string> team_a_updates,
             std::map<std::string, std::string> team_b_updates, std::string description)
    : team_a_name(team_a_name), team_b_name(team_b_name), name(name),
      time(time), game_updates(game_updates), team_a_updates(team_a_updates),
      team_b_updates(team_b_updates), description(description), userName()
{
}

Event::~Event()
{
}

const std::string &Event::get_team_a_name() const
{
    return this->team_a_name;
}

const std::string &Event::get_team_b_name() const
{
    return this->team_b_name;
}

const std::string &Event::get_name() const
{
    return this->name;
}

int Event::get_time() const
{
    return this->time;
}

const std::map<std::string, std::string> &Event::get_game_updates() const
{
    return this->game_updates;
}

const std::map<std::string, std::string> &Event::get_team_a_updates() const
{
    return this->team_a_updates;
}

const std::map<std::string, std::string> &Event::get_team_b_updates() const
{
    return this->team_b_updates;
}

const std::string &Event::get_description() const
{
    return this->description;
}

Event::Event(const std::string &frame_body) : team_a_name(""), team_b_name(""), name(""), time(0), game_updates(), team_a_updates(), team_b_updates(), description(""), userName("")
{
    vector<string> splittedStringByRow = SocketReader::splitByRow(frame_body);
    vector<string> headers;
    unsigned int i=0;
    while (i<splittedStringByRow.size()) {
        string line = splittedStringByRow[i];
        int index1 = line.find(":");
        string temp = line.substr(index1+1);
        bool inBlock = true;
        string block;
        if (temp.empty()) {
            while (inBlock) {
                string nextLine = splittedStringByRow[i+1];
                int index2 = nextLine.find(":");
                string temp2 = nextLine.substr(index2+1);
                if (!temp2.empty()) {
                    block += nextLine;
                    block += "\n";
                    i++;
                }
                else {
                    headers.push_back(block);
                    inBlock = false;
                    i++; //?
                }
            }
        }
        else {
            i++;
            headers.push_back(temp);
        }
    }
    userName = headers[0];
    team_a_name = headers[1];
    team_b_name = headers[2];
    name = headers[3];
    time = std::stoi(headers[4]); // trust me.
    description = headers[8];

    if (!headers[5].empty()) {
        vector<string> splittedGeneralUpdates = SocketReader::splitByRow(headers[5]);
        for (string currUpdate : splittedGeneralUpdates) {
            int currIndex = currUpdate.find(":");
            string key = currUpdate.substr(0,currIndex);
            string value = currUpdate.substr(currIndex+1);
            game_updates[key] = value;
        }
    }

    if (!headers[6].empty()) {
        vector<string> splittedAUpdates = SocketReader::splitByRow(headers[6]);
        for (string currUpdate : splittedAUpdates) {
            int currIndex = currUpdate.find(":");
            string key = currUpdate.substr(0,currIndex);
            string value = currUpdate.substr(currIndex+1);
            team_a_updates[key] = value;
        }
    }

    if (!headers[7].empty()) {
        vector<string> splittedBUpdates = SocketReader::splitByRow(headers[7]);
        for (string currUpdate : splittedBUpdates) {
            int currIndex = currUpdate.find(":");
            string key = currUpdate.substr(0,currIndex);
            string value = currUpdate.substr(currIndex+1);
            team_b_updates[key] = value;
        }
    }

}

names_and_events parseEventsFile(std::string json_path)
{
    std::ifstream f(json_path);
    json data = json::parse(f);

    std::string team_a_name = data["team a"];
    std::string team_b_name = data["team b"];

    // run over all the events and convert them to Event objects
    std::vector<Event> events;
    for (auto &event : data["events"])
    {
        std::string name = event["event name"];
        int time = event["time"];
        std::string description = event["description"];
        std::map<std::string, std::string> game_updates;
        std::map<std::string, std::string> team_a_updates;
        std::map<std::string, std::string> team_b_updates;
        for (auto &update : event["general game updates"].items())
        {
            if (update.value().is_string())
                game_updates[update.key()] = update.value();
            else
                game_updates[update.key()] = update.value().dump();
        }

        for (auto &update : event["team a updates"].items())
        {
            if (update.value().is_string())
                team_a_updates[update.key()] = update.value();
            else
                team_a_updates[update.key()] = update.value().dump();
        }

        for (auto &update : event["team b updates"].items())
        {
            if (update.value().is_string())
                team_b_updates[update.key()] = update.value();
            else
                team_b_updates[update.key()] = update.value().dump();
        }
        
        events.push_back(Event(team_a_name, team_b_name, name, time, game_updates, team_a_updates, team_b_updates, description));
    }
    names_and_events events_and_names{team_a_name, team_b_name, events};

    return events_and_names;
}