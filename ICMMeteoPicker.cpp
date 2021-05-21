//
// Created by Dawid Kulpa on 19.05.2021.
//

#include "ICMMeteoPicker.h"

const string ICMMeteoPicker::graphURL="https://www.meteo.pl/um/metco/mgram_pict.php?ntype=0u&fdate=";

int ICMMeteoPicker::getMeteograph(int x, int y, const string &dest) {
    int res= 0;
    CURL  *curlHandle= curl_easy_init();

    string urlBuilder= ICMMeteoPicker::prepareURL(x, y);

    string datestamp= to_string(Clock::nowYear())+to_string(Clock::nowMonth())+to_string(Clock::nowDay());
    string filename= "x"+to_string(x)+"y"+to_string(y)+"_"+datestamp+"_"+to_string(Clock::nowHour())+".png";
    std::ofstream stream(dest+"/"+filename, std::ostream::binary);

    curl_easy_setopt(curlHandle, CURLOPT_URL, urlBuilder.c_str());
    curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, ICMMeteoPicker::write_data);
    curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, &stream);

    curl_easy_perform(curlHandle);

    curl_off_t cl;
    curl_easy_getinfo(curlHandle, CURLINFO_SIZE_DOWNLOAD_T, &cl);
    if(cl<500)
        res= 1;

    curl_easy_cleanup(curlHandle);
    stream.close();

    return res;
}

string ICMMeteoPicker::prepareURL(int x, int y) {
    string url= ICMMeteoPicker::graphURL;

    //Append year
    url+= to_string(Clock::nowYear());

    //Get time
    int month= Clock::nowMonth();
    int day= Clock::nowDay();
    int hour= Clock::nowHour();

    //Append month
    if(month<10)
        url+="0";
    url+= to_string(month);

    //Append day
    if(hour<7)
        day--;
    if(day<10)
        url+="0";
    url+= to_string(day);

    //Append hour
    if(hour<7)
        url+= "18";
    else if (hour<12)
        url+= "00";
    else if (hour<19)
        url+= "06";
    else
        url+= "12";

    url+= "&row="+to_string(y);
    url+= "&col="+to_string(x);
    url+= "&lang=pl";

    return url;
}

size_t ICMMeteoPicker::write_data(char *buf, size_t size, size_t nmemb, void *userp) {
    if (userp) {
        std::ostream& os = *static_cast<std::ostream*>(userp);
        std::streamsize len = size * nmemb;
        if (os.write(static_cast<char*>(buf), len))
            return len;
    }

    return 0;
}
