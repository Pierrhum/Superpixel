//
// Created by pierrhum on 02/03/2022.
//

#ifndef SUPERPIXELS_IMAGE_H
#define SUPERPIXELS_IMAGE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <vector>
#include <iostream>
#include "Image_ppm.h"

typedef unsigned char OCTET;

class Image : public Image_ppm {
	
public:
    OCTET *ImgData;
    int nH = 0;
    int nW = 0;
    int nTaille = 0;
    int nTaille3 = 0;

    Image() {};
    Image(char* fileName);
    Image* ToSuperPixelsBySLIC(int K, int m, bool displayContour);
    void WriteFile(char* fileName);
    void rle_encode(const OCTET* data, const uint data_length, std::vector<uint>& encoded_data);
    void rle_decode(const std::vector<uint>& encoded_data, OCTET** decoded_data, uint& decoded_data_length);
};


#endif //SUPERPIXELS_IMAGE_H