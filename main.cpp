#include "dkulpaclibs/misc/Clock.h"
#include "dkulpaclibs/misc/ExString.h"
#include "ICMMeteoPicker.h"
#include "dkulpaclibs/misc/Logger.h"
#include "ICMPos.h"
#include <vector>
#include <unistd.h>


using namespace std;

vector<int> updateHours;

void initUpdateHours();
bool isUpdateNeeded(int lastUpdate, Logger *logger);


int main(int argc, char** argv) {
    vector<ICMPos> places;

    if(argc>2){
        for(int i=2; i<argc; i++){
            if(string(argv[i]).find_first_of("0123456789")!=string::npos) {
                vector<string> parts = ExString::split(string(argv[i]), ",");
                places.emplace_back(atoi(parts[0].c_str()), atoi(parts[1].c_str()));
            } else {
                places.emplace_back(string(argv[i]));
            }
        }
    } else {
        cout << "Syntax: ICMMeteoLogger /meteographs/directory x1,y1 [x2,y2 x3,y3 ...]" << endl;
        return 1;
    }

    bool runLoop= true;
    int lastUpdateHour=Clock::nowHour();
    Logger logger(string(argv[1])+"/icmpicker.log");

    logger.write("Start");
    for(int i=0; i<places.size(); i++){
        logger.write("Places", (to_string(places[i].getX())+", "+to_string(places[i].getY())).c_str());
    }

    initUpdateHours();

    //Download meteo image
    while (runLoop) {
        if(isUpdateNeeded(lastUpdateHour, &logger)){
            for(int i=0; i<places.size(); i++) {
                ICMMeteoPicker::getMeteograph(places[i].getX(), places[i].getY(), argv[1]);
                logger.write("%d,%d meteograph downloaded", places[i].getX(), places[i].getY());
            }
            lastUpdateHour= Clock::nowHour();
        }
        if(Clock::nowYear()>2030)
            runLoop= false;

        logger.write("Sleeping 10min...");
        usleep(600000000);
    }

    return 0;
}

void initUpdateHours() {
    updateHours.push_back(2);
    updateHours.push_back(8);
    updateHours.push_back(14);
    updateHours.push_back(20);
}

bool isUpdateNeeded(int lastUpdate, Logger *logger){
    if(Clock::nowHour()!=lastUpdate){
        for(int i=0; i<updateHours.size(); i++){
            if(Clock::nowHour()==updateHours[i]){
                char s[256];
                sprintf(s, "%02d:00 meteo update needed", Clock::nowHour());
                logger->write("ICMMeteoLogger", s);
                return true;
            }
        }
    }
    return false;
}