//
// Created by pierrhum on 05/04/2022.
//

#ifndef CODE_SUPERPIXELS_H
#define CODE_SUPERPIXELS_H


#include "Image.h"
#include <vector>

using namespace std;

class SuperPixels {
private:
    int N;
    int K;
    double S;
    Image *img;

    vector< vector<double> > centers;

    void InitCenters();

public:
    SuperPixels(Image* input, int nbPixels, int nbSuperPixels, double Step);
    Image* GetImage();
};


#endif //CODE_SUPERPIXELS_H
