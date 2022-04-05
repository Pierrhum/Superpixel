#include "Image.h"

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
    FILE *f_image;
    int taille_image = 3*nW * nH;

    if( (f_image = fopen(fileName, "wb")) == NULL)
    {
        printf("\nPas d'acces en ecriture sur l'image %s \n", fileName);
        exit(EXIT_FAILURE);
    }
    else
    {
        fprintf(f_image,"P6\r") ;                               /*ecriture entete*/
        fprintf(f_image,"%d %d\r255\r", nW, nH) ;

        if( (fwrite((OCTET*)ImgData, sizeof(OCTET), taille_image, f_image))
            != (size_t)(taille_image))
        {
            printf("\nErreur d'ecriture de l'image %s \n", fileName);
            exit(EXIT_FAILURE);
        }
        fclose(f_image);
    }
}
