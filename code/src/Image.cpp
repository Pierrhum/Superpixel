#include "Image.h"
#include "SuperPixels.h"

Image::Image(char* fileName) {
    // Lecture de l'image et initialisation des variables
    lire_nb_lignes_colonnes_image_ppm(fileName, &nH, &nW);
    nTaille = nH * nW;
    nTaille3 = nTaille * 3;
    allocation_tableau(ImgData, OCTET, nTaille3);
    lire_image_ppm(fileName, ImgData, nH * nW);
}

Image* Image::ToSuperPixelsBySLIC(int K, int m, int displayMode, int iteration) {
    // Copie de l'input
    Image* img = new Image();
    img->ImgData = ImgData;
    img->nH = nH;
    img->nW = nW;
    img->nTaille = nTaille;
    img->nTaille3 = nTaille3;

    // Création de l'image en Superpixels
    SuperPixels output = SuperPixels(img, displayMode, img->nTaille, K, sqrt((nTaille)/K), m, iteration);

    // Retourne l'image
    return output.GetImage();
}

void Image::WriteFile(char* fileName) {
   ecrire_image_ppm(fileName, ImgData,  nH, nW);
}

void Image::rle_encode(const OCTET* data, const unsigned int data_length, std::vector<unsigned int>& encoded_data) {

    unsigned int i = 0;

    while(i < data_length) {

        unsigned int count = 1;
        
        OCTET red_value = data[i];
        OCTET green_value = data[i+1];
        OCTET blue_value = data[i+2];
        
        unsigned int j = i;

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

void Image::rle_decode(const std::vector<unsigned int>& encoded_data, OCTET** decoded_data, unsigned int& decoded_data_length) {

    unsigned int encoded_data_length = encoded_data.size();

    decoded_data_length = 0;

    for(unsigned int i = 0; i < encoded_data_length; i += 4) decoded_data_length += encoded_data[i];

    decoded_data_length *= 3;

    allocation_tableau(*decoded_data,OCTET,decoded_data_length);

    unsigned int k = 0;

    for(unsigned int i = 0; i < encoded_data_length; i += 4) {

        unsigned int run_count = encoded_data[i];

        OCTET run_red_value = encoded_data[i+1];
        OCTET run_green_value = encoded_data[i+2];
        OCTET run_blue_value = encoded_data[i+3];

        for(unsigned int j = 0; j < run_count; j++) {

            decoded_data[0][k] = run_red_value;
            decoded_data[0][k+1] = run_green_value;
            decoded_data[0][k+2] = run_blue_value;
            k += 3;
        }
    }
}