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

Image* Image::ToSuperPixelsBySLIC(int K, int m) {
    // Copie de l'input
    Image* img = new Image();
    img->ImgData = ImgData;
    img->nH = nH;
    img->nW = nW;
    img->nTaille = nTaille;
    img->nTaille3 = nTaille3;

    // Création de l'image en Superpixels
    SuperPixels output = SuperPixels(img, img->nTaille, K, sqrt((nTaille)/K), m);

    // Retourne l'image
    return output.GetImage();
}

void Image::WriteFile(char* fileName) {
   ecrire_image_ppm(fileName, ImgData,  nH, nW);
}
