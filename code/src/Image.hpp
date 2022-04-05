//
// Created by pierrhum on 02/03/2022.
//

#ifndef SUPERPIXELS_IMAGE_HPP
#define SUPERPIXELS_IMAGE_HPP

#include <stdio.h>
#include "image_ppm.h"

class Image {
private:
    OCTET *OriginalData;
    OCTET *ImgData;

public:
    Image(const char* fileName);
    void ToSuperPixels(int K);
    void WriteFile(const char* fileName);
};


#endif //SUPERPIXELS_IMAGE_HPP