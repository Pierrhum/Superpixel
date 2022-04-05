//
// Created by pierrhum on 05/04/2022.
//

#include "SuperPixels.h"

SuperPixels::SuperPixels(Image *input, int nbPixels, int nbSuperPixels, double Step) {
    this->img = input;
    this->N = nbPixels;
    this->K = nbSuperPixels;
    this->S = Step;

    /// ALGO

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

/** TODO : Initialisation des centres des superpixels.
 * Les centres sont équidistants (distance S) et sont en 5 dimensions (x,y,L,a,b) pour leurs coordonnées
 * spatiales et la couleur du pixel dans l'espace LAB.
 */
void SuperPixels::InitCenters() {
    //for(double i = S; i < img-> )
}
