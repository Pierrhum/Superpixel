//
// Created by pierrhum on 05/04/2022.
//

#include <iostream>
#include "SuperPixels.h"

SuperPixel::SuperPixel(Pixel* center) {
    this->center = center;
}

double SuperPixel::GetDistance(Pixel* pix, int m, double S) {
    // Calcul de la distance
    double dRGB = sqrt(pow(center->color.R - pix->color.R, 2) + pow(center->color.G - pix->color.G, 2) + pow(center->color.B - pix->color.B, 2));
    double dXY = sqrt(pow(center->x - pix->x, 2) + pow(center->y - pix->y, 2));
    double dist = dRGB + m/S * dXY;

    return dist;
}

void SuperPixel::Draw(Image* img) {
    for(Pixel* pixel : pixels) {
        img->ImgData[(pixel->y*3) * img->nW + (pixel->x*3)] = center->color.R;
        img->ImgData[(pixel->y*3) * img->nW + (pixel->x*3 + 1)] = center->color.G;
        img->ImgData[(pixel->y*3) * img->nW + (pixel->x*3 + 2)] = center->color.B;
    }
}