#include "Image.h"
#include "SuperPixels.h"
#include <iostream>
#include <vector>

Image::Image(char* fileName) {
    // Lecture de l'image et initialisation des variables
    lire_nb_lignes_colonnes_image_ppm(fileName, &nH, &nW);
    nTaille = nH * nW;
    nTaille3 = nTaille * 3;
    allocation_tableau(ImgData, OCTET, nTaille3);
    lire_image_ppm(fileName, ImgData, nH * nW);
}

Image* Image::ToSuperPixelsBySLIC(int K, int m, bool displayContour) {
    // Copie de l'input
    Image* img = new Image();
    img->ImgData = ImgData;
    img->nH = nH;
    img->nW = nW;
    img->nTaille = nTaille;
    img->nTaille3 = nTaille3;

    // Création de l'image en Superpixels
    SLIC output = SLIC(img, img->nTaille, K, sqrt((nTaille)/K), m);
    if(displayContour)
        output.DrawContour();

    // Retourne l'image
    return output.GetImage();
}

void Image::WriteFile(char* fileName) {
   ecrire_image_ppm(fileName, ImgData,  nH, nW);
}

void Image::rle_encode(const OCTET* data, const uint data_length, std::vector<uint>& encoded_data) {

    uint i = 0;

    while(i < data_length) {

        uint count = 1;
        
        OCTET red_value = data[i];
        OCTET green_value = data[i+1];
        OCTET blue_value = data[i+2];
        
        uint j = i;

        while(j < data_length) {

            if(data[j] == data[j+3] && data[j+1] == data[j+4] && data[j+2] == data[j+5]) {

                count++;
                j += 3;
            }

            else break;
        }

        encoded_data.push_back(count);
        encoded_data.push_back(red_value);
        encoded_data.push_back(green_value);
        encoded_data.push_back(blue_value);

        i = j + 3;
    }
}

void Image::rle_decode(const std::vector<uint>& encoded_data, OCTET** decoded_data, uint& decoded_data_length) {

    uint encoded_data_length = encoded_data.size();

    decoded_data_length = 0;

    for(uint i = 0; i < encoded_data_length; i += 4) decoded_data_length += encoded_data[i];

    decoded_data_length *= 3;

    allocation_tableau(*decoded_data,OCTET,decoded_data_length);

    uint k = 0;

    for(uint i = 0; i < encoded_data_length; i += 4) {

        uint run_count = encoded_data[i];

        OCTET run_red_value = encoded_data[i+1];
        OCTET run_green_value = encoded_data[i+2];
        OCTET run_blue_value = encoded_data[i+3];

        for(uint j = 0; j < run_count; j++) {

            decoded_data[0][k] = run_red_value;
            decoded_data[0][k+1] = run_green_value;
            decoded_data[0][k+2] = run_blue_value;
            k += 3;
        }
    }
}