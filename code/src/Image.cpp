#include "Image.hpp"

Image::Image(char* fileName) {
    
   lire_nb_lignes_colonnes_image_ppm(fileName, &nH, &nW);
   nTaille = nH * nW;
   nTaille3 = nTaille * 3;
   allocation_tableau(ImgData, OCTET, nTaille3);
   lire_image_ppm(fileName, ImgData, nH * nW);
}

Image* Image::ToSuperPixels(int K) {
    Image* img = new Image();
    img->ImgData = ImgData;
    img->nH = nH;
    img->nW = nW;
    img->nTaille = nTaille;
    img->nTaille3 = nTaille3;
    
    return img;
}

void Image::WriteFile(char* fileName) {

   ecrire_image_ppm(fileName, ImgData,  nH, nW);
}