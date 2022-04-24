//
// Created by pierrhum on 05/04/2022.
//

#ifndef CODE_SUPERPIXELS_H
#define CODE_SUPERPIXELS_H


#include "Image.h"
#include <cstdlib>
#include <cfloat>
#include <vector>

using namespace std;

struct Pixel {
    int pR=0;
    int pG=0;
    int pB=0;
};

struct Center {
    int pR=0;
    int pG=0;
    int pB=0;
    int x=0;
    int y=0;
};

class SuperPixels {
private:
    int N;
    int K;
    int m;
    double S;
    Image *img;

    vector<Pixel*> pixels;
    vector< Center* > centers;
    vector<vector<int>> clusters;
    vector<vector<float>> distances;

    void InitCenters();
    void DebugCenter();
    double GetDistance(Center* Ck, int Xi, int Yi);
    vector<Pixel*> GetPixels();

public:
    SuperPixels(Image* input, int nbPixels, int nbSuperPixels, double Step, int weight);
    void DrawContour();
    Image* GetImage();
};


#endif //CODE_SUPERPIXELS_H
