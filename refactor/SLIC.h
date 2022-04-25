//
// Created by pierrhum on 05/04/2022.
//

#ifndef CODE_SLIC_H
#define CODE_SLIC_H

#include "SuperPixels.h"
#include <vector>

class SLIC {
private:
    int N;
    int K;
    int m;
    double S;
    Image *img;

    vector<Pixel*> Pixels;
    vector< SuperPixel* > SuperPixels;

    // Init
    void InitPixels();
    void InitCenters();
    void InitClusters();
    vector<int> AdaptCenter(int x, int y);

    // Mode
    void DrawContour();
    void DebugCenter(int radius);
    void Connectivity();

public:
    SLIC(Image* input, int displayMode, int nbPixels, int nbSuperPixels, double Step, int weight);
    Image* GetImage();
};

#endif //CODE_SLIC_H