//
// Created by pierrhum on 05/04/2022.
//

#include "SuperPixels.h"

SuperPixels::SuperPixels(Image *input, int nbPixels, int nbSuperPixels, double Step) {
    this->img = input;
    this->N = nbPixels;
    this->K = nbSuperPixels;
    this->S = Step;

    /** ALGO SLIC **/
    
    /// Initialisation des centres des superpixels.
    InitCenters();

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

}

Image *SuperPixels::GetImage() {
    return img;
}

/** Initialisation des centres des superpixels.
 * Les centres sont équidistants (distance S) et sont en 5 dimensions (R,G,B,x,y) pour leurs coordonnées
 * spatiales et la couleur du pixel dans l'espace RGB.
 */
void SuperPixels::InitCenters() {
    for(int i = S; i < img->nH - S/2; i+=S) {
        for(int j = S; j < img->nW - S/2; j+=S) {
            vector<double> center;

            OCTET R = img->ImgData[i * img->nW + j];
            OCTET G = img->ImgData[i * img->nW + j + 1];
            OCTET B = img->ImgData[i * img->nW + j + 2];

            center.push_back(R);
            center.push_back(G);
            center.push_back(B);
            center.push_back(i);
            center.push_back(j);

            centers.push_back(center);
        }
    }
}
