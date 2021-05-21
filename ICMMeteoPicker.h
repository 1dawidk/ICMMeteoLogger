//
// Created by Dawid Kulpa on 19.05.2021.
//

#ifndef ICMMETEOLOGGER_ICMMETEOPICKER_H
#define ICMMETEOLOGGER_ICMMETEOPICKER_H

#include <string>
#include "dkulpaclibs/misc/Clock.h"
#include "curl/curl.h"
#include <fstream>

using namespace std;

class ICMMeteoPicker {
public:
    static int getMeteograph(int x, int y, const string &dest);

private:
    static const string graphURL;
    static string prepareURL(int x, int y);
    static size_t write_data(char *buf, size_t size, size_t nmemb, void *userp);
};


#endif //ICMMETEOLOGGER_ICMMETEOPICKER_H
