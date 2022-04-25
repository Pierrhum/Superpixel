//
// Created by pierrhum on 05/04/2022.
//

#ifndef CODE_SUPERPIXELS_H
#define CODE_SUPERPIXELS_H


#include "Image.h"
#include <cstdlib>
#include <cfloat>
#include <vector>
#include <map>

using namespace std;

struct Color {
    int R=0;
    int G=0;
    int B=0;
};

struct Pixel {
    int cluster = 0;
    float dist = FLT_MAX;
    int x=0;
    int y=0;
    Color color;
};

class SuperPixel {
public:
    Pixel* center;
    Color color;
    vector<Pixel*> pixels;
    SuperPixel(Pixel* center);
    bool AddPixel(vector< Pixel* > Pixels, int nW, int nH, int m, double S);
    double GetDistance(Pixel* pix, int m, double S);
    void Draw(Image* img);
};




#endif //CODE_SUPERPIXELS_H
