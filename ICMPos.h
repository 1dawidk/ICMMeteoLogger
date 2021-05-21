//
// Created by Dawid Kulpa on 21.05.2021.
//

#ifndef ICMMETEOLOGGER_ICMPOS_H
#define ICMMETEOLOGGER_ICMPOS_H

#include <string>
#include <vector>
#include <fstream>

#define ICMMAP_X_PARAM_A    16.350365
#define ICMMAP_X_PARAM_B    (-93.875912)
#define ICMMAP_Y_PARAM_C    (-24.700071)
#define ICMMAP_Y_PARAM_D    1712.738885
#define ICMMAP_CITIES_FILE  "../cities_latlng"

using namespace std;


class ICMPos {
public:
    ICMPos(int x, int y);
    explicit ICMPos(const string& city);

    int getX();
    int getY();

    static void findCity(const string& name, int &x, int &y);
private:
    int x;
    int y;
};


#endif //ICMMETEOLOGGER_ICMPOS_H
