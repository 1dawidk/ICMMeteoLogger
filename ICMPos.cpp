//
// Created by Dawid Kulpa on 21.05.2021.
//

#include "ICMPos.h"

ICMPos::ICMPos(int x, int y) {
    this->x= x;
    this->y= y;
}

ICMPos::ICMPos(const string &city) {
    ICMPos::findCity(city, this->x, this->y);
}

void ICMPos::findCity(const string &name, int &x, int &y) {
    vector<string> cityNames;
    vector<int> cityX;
    vector<int> cityY;
    ifstream file;

    file.open(ICMMAP_CITIES_FILE);
    if(file.is_open()){
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
            x = (int) (lng * ICMMAP_X_PARAM_A + ICMMAP_X_PARAM_B);
            y = (int) (lat * ICMMAP_Y_PARAM_C + ICMMAP_Y_PARAM_D);

            lastW = line.find_last_not_of(' ', lngStart);
            name = line.substr(0, lastW + 1);
            cityNames.push_back(name);
            cityX.push_back(x);
            cityY.push_back(y);

            getline(file, line);
        }
    }

    for(int i=0; i<cityNames.size(); i++){
        if(cityNames[i]==name){
            x= cityX[i];
            y= cityY[i];
            break;
        }
    }
}

int ICMPos::getX() {
    return x;
}

int ICMPos::getY() {
    return y;
}
