//
// Created by pierrhum on 05/04/2022.
//

#include <iostream>
#include "SuperPixels.h"

SuperPixel::SuperPixel(Pixel* center)  {
    this->center = center;
    this->color = center->color;
    this->pixels.push_back(center);
}

double SuperPixel::GetDistance(Pixel* pixel, Color color, int m, int S)  {
    // Calcul de la distance
    double dRGB = sqrt(pow(center->color.R - pixel->color.R, 2) + pow(center->color.G - pixel->color.G, 2) + pow(center->color.B - pixel->color.B, 2));
    double dXY = sqrt(pow(center->x - pixel->x, 2) + pow(center->y - pixel->y, 2));
    double dist = dRGB + m/S * dXY;

    return dist;
}

bool SuperPixel::AddPixel(vector< Pixel* > Pixels, int nW, int nH, int m, double S)  {
    bool found = false;
    const int dx[4] = {-1,  0,  1,  0};
	const int dy[4] = { 0, -1,  0,  1};
    double min = FLT_MAX;
    Pixel* newPixel;
    for(Pixel* pix : pixels) {
        for(int k=0; k < 4; k++) {
            int x = pix->x + dx[k];
            int y = pix->y + dy[k];
            if(x >= 0 && x < nW && y >= 0 && y < nH) {
                Pixel* tmp = Pixels[x * nW + y];
                if(!tmp->belongSuperPixel) {
                    double dist = GetDistance(tmp, color, m, S);
                    if(dist < min) {
                        newPixel = tmp;
                        min = dist;
                        found = true;
                    }
                }
            }
        }
    }
    if(found) {
        newPixel->belongSuperPixel = true;
        this->pixels.push_back(newPixel);
    }
    return found;
}

SLIC::SLIC(Image *input, int nbPixels, int nbSuperPixels, double Step, int weight) {
    this->img = input;
    this->N = nbPixels;
    this->K = nbSuperPixels;
    this->S = Step;
    this->m = weight;
    Pixels.resize(img->nW * img->nH);
    
    for(int x=0; x < img->nW; x++) {
        for(int y=0; y < img->nH; y++) {
            Color color;
            color.R = img->ImgData[y*3 * img->nW + (x*3)];
            color.G = img->ImgData[y*3 * img->nW + (x*3 + 1)];
            color.B = img->ImgData[y*3 * img->nW + (x*3 + 2)];
            Pixel* pix = (struct Pixel*) malloc(sizeof(Pixel));
            pix->x = x;
            pix->y = y;
            pix->color = color;
            Pixels[x * img->nW + y] = pix;
        }
    }
    cout << Pixels.size() << endl;

    /** ALGO SLIC **/
    cout << "InitCenter" << endl;
    InitCenters();

    int remaining = nbPixels - 9;
    int old_percentage = 0;
    int percentage = 0;
    cout << percentage << "%" << endl;

    cout << remaining << endl;
    for(int i=0; i < 2000; i++) {
        for(SuperPixel* spix : SuperPixels) {
            bool found = spix->AddPixel(Pixels, img->nW, img->nH, m, S);
            if(found)
                remaining--;
        }
        if(old_percentage == 99)
            break;
        percentage = ((nbPixels - remaining)* 100) / nbPixels;
        if(percentage != old_percentage) {
            old_percentage = percentage;
            cout << "\x1B[2J\x1B[H";
            cout << percentage << "%" << endl;
        }
    }
    cout << remaining << endl;
    
    cout << "Ecriture de l'image" << endl;
    for(int x = 0; x < img->nW; x++) {
        for(int y = 0; y < img->nH; y++) {
            img->ImgData[y*3 * img->nW + (x*3)] = 0;
            img->ImgData[y*3 * img->nW + (x*3 + 1)] = 0;
            img->ImgData[y*3 * img->nW + (x*3 + 2)] = 0;
        }
    }
    for(SuperPixel* spix : SuperPixels) {
        for(Pixel* pix : spix->pixels) {
            int x = pix->x;
            int y = pix->y;
            img->ImgData[y*3 * img->nW + (x*3)]= spix->color.R;
            img->ImgData[y*3 * img->nW + (x*3 + 1)] = spix->color.G;
            img->ImgData[y*3 * img->nW + (x*3 + 2)] = spix->color.B;
        }
    }

    
}

Image *SLIC::GetImage() {
    return img;
}

/** Initialisation des centres des superpixels.
 * Les centres sont équidistants (distance S) et sont en 5 dimensions (R,G,B,x,y) pour leurs coordonnées
 * spatiales et la couleur du pixel dans l'espace RGB.
 */

void SLIC::InitCenters() {
    int nbCenterW = (int)floor(img->nW / S);
    int nbCenterH = (int)floor(img->nH / S);
    
    for(int x = S/2; x <= img->nW - S/2; x+=S) {
        for(int y = S/2; y <= img->nH - S/2; y+=S) {
            Pixel* center = Pixels[x * img->nW + y];
            center->belongSuperPixel = true;
            SuperPixel* spix = new SuperPixel(center);

            SuperPixels.push_back(spix);
        }
    }
}

void SLIC::DrawContour() {
    const int dx[8] = {-1, -1,  0,  1, 1, 1, 0, -1};
	const int dy[8] = { 0, -1, -1, -1, 0, 1, 1,  1};

    vector<bool> line;
    line.resize(img->nH, false);
    vector<vector<bool>> draw;
    draw.resize(img->nW, line);

    for(int i = 0; i < img->nW; i++) {
        for(int j = 0; j < img->nH; j++) {
            /* Compare the pixel to its 8 neighbours. */
            for (int k = 0; k < 8; k++) {
                int x = i + dx[k];
                int y = j + dy[k];
                if (x >= 0 && x < img->nW && y >= 0 && y < img->nH) {
                    if(!draw[x][y] && !draw[i][j] && clusters[i][j] != clusters[x][y]) {
                        draw[x][y] = true;
                        img->ImgData[y*3 * img->nW + (x*3)] = 255;
                        img->ImgData[y*3 * img->nW + (x*3 + 1)] = 0;
                        img->ImgData[y*3 * img->nW + (x*3 + 2)] = 0;
                    }
                }
            }
        }
    }
}

void SLIC::DebugCenter() {
    const int dx[8] = {-1, -1,  0,  1, 1, 1, 0, -1};
	const int dy[8] = { 0, -1, -1, -1, 0, 1, 1,  1};
    
    for(int x = 0; x < img->nW; x++) {
        for(int y = 0; y < img->nH; y++) {
            img->ImgData[y*3 * img->nW + (x*3)] = 0;
            img->ImgData[y*3 * img->nW + (x*3 + 1)] = 0;
            img->ImgData[y*3 * img->nW + (x*3 + 2)] = 0;
        }
    }
    
    for(SuperPixel* spix : SuperPixels) {
        Pixel* c = spix->center;
        for (int k = 0; k < 8; k++) {
            int x = c->x + dx[k];
            int y = c->y + dy[k];
            img->ImgData[y*3 * img->nW + (x*3)] = 255;
        }
        
    }
}
