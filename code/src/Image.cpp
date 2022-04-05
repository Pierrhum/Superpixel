#include "Image.h"

Image::Image(char* fileName) {
    
   lire_nb_lignes_colonnes_image_ppm(fileName, &nH, &nW);
   nTaille = nH * nW;
   nTaille3 = nTaille * 3;
   allocation_tableau(ImgData, OCTET, nTaille3);
   lire_image_ppm(fileName, ImgData, nH * nW);
}

Image* Image::ToSuperPixelsBySLIC(int K) {
    Image* img = new Image();
    img->ImgData = ImgData;
    img->nH = nH;
    img->nW = nW;
    img->nTaille = nTaille;
    img->nTaille3 = nTaille3;

    /// ALGO

    /** TODO : Initialisation des centres des superpixels.
     * Les centres sont équidistants (distance S) et sont en 5 dimensions (x,y,L,a,b) pour leurs coordonnées
     * spatiales et la couleur du pixel dans l'espace LAB.
     */

     /** TODO : Initialisation de la carte des superpixels et de la carte des distances.
      * Les distances sont initialisées à l'infini et les pixels sont attribués au superpixel 0.
      */

    /** TODO : Boucle : étape 1.
     * Pour chaque centre, on calcule la distance des pixels se trouvant dans un rayon de 2S au centre.
     * Si la distance est plus petite que celle en mémoire pour le pixel, elle est modifiée en mémoire
     * et le pixel fait maintenant partie du superpixel.
     */

    /** TODO : Boucle : étape 2.
      * Les positions des centres (5 dimensions) des superpixels sont mises à jour selon la valeur moyenne de tous les pixels du superpixel.
      */

    /** TODO : Fin boucle
      * Les étapes 2 et 3 sont re-effectuées jusqu'à ce que l'algorithme converge, c'est-à-dire, que le déplacement moyen des centres entre
      * deux itérations soit plus petit qu'un certain seuil.
      */


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
