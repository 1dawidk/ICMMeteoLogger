#include <iostream>
#include <string>
#include "dkulpaclibs/misc/Clock.h"
#include "curl/curl.h"
#include "ICMMeteoPicker.h"
#include "Logger.h"
#include <vector>
#include <fstream>
#include <unistd.h>

#define ICMWEATHERVIEW_X_PARAM_A    16.350365
#define ICMWEATHERVIEW_X_PARAM_B    (-93.875912)
#define ICMWEATHERVIEW_Y_PARAM_C    (-24.700071)
#define ICMWEATHERVIEW_Y_PARAM_D    1712.738885

using namespace std;

vector<string> cityNames;
vector<int> cityX;
vector<int> cityY;
vector<int> updateHours;

void initUpdateHours();
bool isUpdateNeeded(int lastUpdate);
void parseCitiesFile(const string &path);
int findCity(const string &cityName);


int main(int argc, char** argv) {
    bool runLoop= true;
    int lastUpdateHour=Clock::nowHour();
    Logger logger("/home/dkulpa/icmMeteo/icmpicker.log");

    initUpdateHours();

    //Create CURL handler
    cout << "Create CURL handler" << endl;

    //Download meteo image
    while (runLoop) {
        if(isUpdateNeeded(lastUpdateHour)){
            ICMMeteoPicker::getMeteograph(250, 406, "/home/dkulpa/icmLogs");
            lastUpdateHour= Clock::nowHour();
        }
        if(Clock::nowYear()>2030)
            runLoop= false;

        cout << "Sleeping... (1min)" << endl;
        usleep(60000000);
    }

    return 0;
}

void parseCitiesFile(const string &path) {
    ifstream file;

    file.open(path);
    if(!file.is_open()){
        cout << "Cities file not found!" << endl;
    } else {
        cout << "Cities file parsing!" << endl;
        string line;

        string latd;
        string latm;
        string lngd;
        string lngm;
        string name;

        float lat;
        float lng;
        int x;
        int y;

        getline(file, line);
        getline(file, line);
        getline(file, line);
        while (!line.empty()) {
            size_t latStart = line.find_last_of(' ');
            size_t lastW = line.find_last_not_of(' ', latStart);
            size_t lngStart = line.find_last_of(" )", lastW);

            latd = line.substr(latStart + 1, 2);
            latm = line.substr(latStart + 5, 2);
            lngd = line.substr(lngStart + 1, 2);
            lngm = line.substr(lngStart + 5, 2);

            lat = stof(latd) + stof(latm) / 60;
            lng = stof(lngd) + stof(lngm) / 60;
            x = (int) (lng * ICMWEATHERVIEW_X_PARAM_A + ICMWEATHERVIEW_X_PARAM_B);
            y = (int) (lat * ICMWEATHERVIEW_Y_PARAM_C + ICMWEATHERVIEW_Y_PARAM_D);

            lastW = line.find_last_not_of(' ', lngStart);
            name = line.substr(0, lastW + 1);
            cityNames.push_back(name);
            cityX.push_back(x);
            cityY.push_back(y);

            getline(file, line);
        }
    }
}

void initUpdateHours() {
    updateHours.push_back(2);
    updateHours.push_back(8);
    updateHours.push_back(14);
    updateHours.push_back(20);
}

bool isUpdateNeeded(int lastUpdate){
    if(Clock::nowHour()!=lastUpdate){
        for(int i=0; i<updateHours.size(); i++){
            if(Clock::nowHour()==updateHours[i]){
                cout << Clock::nowHour() << ":00 meteo update needed" << endl;
                return true;
            }
        }
    }
    return false;
}

int findCity(const string &cityName) {
    for(int i=0; i<cityNames.size(); i++){
        if(cityNames[i]==cityName){
            return i;
        }
    }

    return -1;
}