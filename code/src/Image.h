//
// Created by pierrhum on 02/03/2022.
//

#ifndef SUPERPIXELS_IMAGE_H
#define SUPERPIXELS_IMAGE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "Image_ppm.h"

typedef unsigned char OCTET;

class Image : public Image_ppm {
private:
    OCTET *ImgData;
    int nH = 0;
    int nW = 0; 
    int nTaille = 0;
    int nTaille3 = 0;

public:
    Image() {};
    Image(char* fileName);
    Image* ToSuperPixelsBySLIC(int K);
    void WriteFile(char* fileName);
};


#endif //SUPERPIXELS_IMAGE_H