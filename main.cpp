#include <iostream>
#include <string>
#include "dkulpaclibs/misc/Clock.h"
#include "curl/curl.h"
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

string downloadMeteo(CURL* curlH, int cityIdx);
size_t write_data(char *buf, size_t size, size_t nmemb, void *userp);
void processImage(const string &filename);
void initUpdateHours();
bool isUpdateNeeded(int lastUpdate);
void parseCitiesFile(const string &path);
int findCity(const string &cityName);


int main(int argc, char** argv) {
    bool runLoop= true;
    int lastUpdateHour=Clock::getHour();
    parseCitiesFile("./cities_latlng");
    int city= findCity(argv[1]);

    initUpdateHours();

    //Create CURL handler
    cout << "Create CURL handler" << endl;
    CURL  *curlHandle= curl_easy_init();

    //Download meteo image
    while (runLoop) {
        if(isUpdateNeeded(lastUpdateHour)){
            string fileName= downloadMeteo(curlHandle, city);
            processImage(fileName);
            lastUpdateHour= Clock::getHour();
        }
        if(Clock::getYear()>2030)
            runLoop= false;

        cout << "Sleeping... (1min)" << endl;
        usleep(60000000);
    }

    //CURL cleanup
    curl_easy_cleanup(curlHandle);

    return 0;
}

string downloadMeteo(CURL* curlH, int cityIdx){
    cout << "Download image:" << endl;

    string urlBuilder= "https://www.meteo.pl/um/metco/mgram_pict.php?ntype=0u&fdate=";

    //Append year
    urlBuilder+= to_string(Clock::getYear());

    //Get time
    int month= Clock::getMonth();
    int day= Clock::getDay();
    int hour= Clock::getHour();

    //Append month
    if(month<10)
        urlBuilder+="0";
    urlBuilder+= to_string(month);

    //Append day
    if(hour<7)
        day--;
    if(day<10)
        urlBuilder+="0";
    urlBuilder+= to_string(day);

    //Append hour
    if(hour<7)
        urlBuilder+= "18";
    else if (hour<12)
        urlBuilder+= "00";
    else if (hour<19)
        urlBuilder+= "06";
    else
        urlBuilder+= "12";

    urlBuilder+= "&row="+to_string(cityY[cityIdx]);
    urlBuilder+= "&col="+to_string(cityX[cityIdx]);
    urlBuilder+= "&lang=pl";

    string datestamp= to_string(Clock::getYear())+to_string(month)+to_string(day);
    string filename= "warszawa_"+datestamp+"_"+to_string(hour)+".png";
    std::ofstream stream(filename, std::ostream::binary);

    curl_easy_setopt(curlH, CURLOPT_URL, urlBuilder.c_str());
    curl_easy_setopt(curlH, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curlH, CURLOPT_WRITEDATA, &stream);

    curl_easy_perform(curlH);

    return filename;
}

size_t write_data(char *buf, size_t size, size_t nmemb, void *userp) {
    if (userp) {
        std::ostream& os = *static_cast<std::ostream*>(userp);
        std::streamsize len = size * nmemb;
        if (os.write(static_cast<char*>(buf), len))
            return len;
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
    updateHours.push_back(1);
    updateHours.push_back(7);
    updateHours.push_back(13);
    updateHours.push_back(19);
}

bool isUpdateNeeded(int lastUpdate){
    if(Clock::getHour()!=lastUpdate){
        for(int i=0; i<updateHours.size(); i++){
            if(Clock::getHour()==updateHours[i]){
                cout << Clock::getHour() << ":00 meteo update needed" << endl;
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

void processImage(const string &filename){

}