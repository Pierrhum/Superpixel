//
// Created by pierrhum on 02/03/2022.
//

#ifndef SUPERPIXELS_IMAGE_HPP
#define SUPERPIXELS_IMAGE_HPP

#include <stdio.h>
#include "image_ppm.h"

class Image {
private:
    OCTET *ImgData;
    int nH = 0;
    int nW = 0; 
    int nTaille = 0;
    int nTaille3 = 0;

public:
    Image() {};
    Image(char* fileName);
    Image* ToSuperPixels(int K);
    void WriteFile(char* fileName);
};


#endif //SUPERPIXELS_IMAGE_HPP