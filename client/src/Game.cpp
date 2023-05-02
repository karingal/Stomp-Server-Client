#include "../include/Game.h"
#include <fstream>
#include <vector>

using namespace std;


Game::Game(string gameName) : userToEvent(), gameName_(gameName){
}

void Game::createSummary(string userName, string file) {
    string filePath = file;
    ofstream oFile;
    oFile.open(filePath);
    map<string,string> generalUpdates;
    map<string,string> teamAUpdates;
    map<string,string> teamBUpdates;
    int index = gameName_.find("_");
    string teamA = gameName_.substr(0, index);
    string teamB = gameName_.substr(index+1);
    if (oFile.is_open()) {
        vector<Event> &gameUserUpdates = userToEvent[userName];
        for (Event &currEvent : gameUserUpdates) {
            for (auto& kv : currEvent.get_game_updates()) {
                string key = kv.first;
                string value = kv.second;
                generalUpdates[key] = value;
            }
            for (auto& kv : currEvent.get_team_a_updates()) {
                string key = kv.first;
                string value = kv.second;
                teamAUpdates[key] = value;
            }
            for (auto& kv : currEvent.get_team_b_updates()) {
                string key = kv.first;
                string value = kv.second;
                teamBUpdates[key] = value;
            }
        }

        string generalStats = "";
        for (auto& kv : generalUpdates) {
            string key = kv.first;
            string value = kv.second;
            generalStats += key + ":" + value + "\n";
        }

        string teamAStats = "";
        for (auto& kv : teamAUpdates) {
            string key = kv.first;
            string value = kv.second;
            teamAStats += key + ":" + value + "\n";
        }

        string teamBStats = "";
        for (auto& kv : teamBUpdates) {
            string key = kv.first;
            string value = kv.second;
            teamBStats += key + ":" + value + "\n";
        }

        string gameReports = "";
        for (Event &currEvent : gameUserUpdates) {
            gameReports += std::to_string(currEvent.get_time()) + " - " + currEvent.get_name() + ":" + "\n"
            + "\n" 
            + currEvent.get_description() + "\n\n\n";
        }


        string summaryOutput = teamA + " vs " + teamB + "\n"
                        + "Game stats:" + "\n"
                        + "General stats:" + "\n"
                        + generalStats
                        + teamA + " stats:" + "\n"
                        + teamAStats
                        + teamB + " stats:" + "\n"
                        + teamBStats
                        + "Game event reports:" + "\n"
                        + gameReports;

        oFile << summaryOutput;
        oFile.close();
    }
    else {
        cout << "Could not open file at " << filePath << std::endl;
    }
}