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
    bool belongSuperPixel=false;
    Color color;
    int x=0;
    int y=0;
};

class SuperPixel {
public:
    Pixel* center;
    Color color;
    vector<Pixel*> pixels;
    SuperPixel(Pixel* center);
    bool AddPixel(vector< Pixel* > Pixels, int nW, int nH, int m, double S);
    double GetDistance(Pixel* pixel, Color color, int m, int S);
};

class SLIC {
private:
    int N;
    int K;
    int m;
    double S;
    Image *img;

    vector< Pixel* > Pixels;
    vector< SuperPixel* > SuperPixels;
    vector<vector<int>> clusters;
    vector<vector<float>> distances;

    void InitCenters();
    vector<int> AdaptCenter(int x, int y);
    void DebugCenter();

public:
    SLIC(Image* input, int nbPixels, int nbSuperPixels, double Step, int weight);
    void DrawContour();
    Image* GetImage();
};


#endif //CODE_SUPERPIXELS_H
