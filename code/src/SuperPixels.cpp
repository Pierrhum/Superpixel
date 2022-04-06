//
// Created by pierrhum on 05/04/2022.
//

#include <iostream>
#include "SuperPixels.h"

#define R 0
#define G 1
#define B 2
#define X 3
#define Y 4

SuperPixels::SuperPixels(Image *input, int nbPixels, int nbSuperPixels, double Step, int weight) {
    this->img = input;
    this->N = nbPixels;
    this->K = nbSuperPixels;
    this->S = Step;
    this->m = weight;

    /** ALGO SLIC **/
    
    /// Initialisation des centres des superpixels.
    InitCenters();


    /** TODO : Initialisation de la carte des superpixels et de la carte des distances.
     * Les distances sont initialisées à l'infini et les pixels sont attribués au superpixel 0.
     */
    for(int y = 0; y < this->img->nH * 3; y++) {
        vector<int> cluster;
        vector<float> distance;
        for(int x = 0; x < this->img->nW; x++) {
            // x : 1279 y : 853
            cluster.push_back(0);
            distance.push_back(FLT_MAX);
        }
        clusters.push_back(cluster);
        distances.push_back(distance);
    }

    /**
     * Pour chaque centre, on calcule la distance des pixels se trouvant dans un rayon de 2S au centre.
     * Si la distance est plus petite que celle en mémoire pour le pixel, elle est modifiée en mémoire
     * et le pixel fait maintenant partie du superpixel.
     */
    for(int i = 0; i < centers.size(); i++) {
        /* Only compare to pixels in a 2 x step by 2 x step region. */
        for (int Cx = centers[i][X] - S; Cx < centers[i][X] + S; Cx++) {
            for (int Cy = centers[i][Y] - S; Cy < centers[i][Y] + S; Cy++) {
                /* Si les centres des pixels sont bien dans l'image */
                if(Cx >= 0 && Cx < img->nH && Cy >= 0 && Cy < img->nW) {
                    // Pixel à comparer
                    OCTET pR = img->ImgData[Cy * img->nW + Cx];
                    OCTET pG = img->ImgData[Cy * img->nW + Cx + 1];
                    OCTET pB = img->ImgData[Cy * img->nW + Cx + 2];

                    // Calcul de la distance
                    double dRGB = sqrt(pow(centers[i][R] - pR, 2) + pow(centers[i][G] - pG, 2) + pow(centers[i][B] - pB, 2));
                    double dXY = sqrt(pow(centers[i][X] - Cx, 2) + pow(centers[i][Y] - Cy, 2));
                    double dist = dRGB + m/S * dXY;

                    // Association du pixel au superpixel si distance inférieure au précédent superpixel
                    if(dist < distances[Cx][Cy]) {
                        distances[Cx][Cy] = dist;
                        clusters[Cx][Cy] = i;
                    }
                }
            }
        }
    }

    /** TODO : Boucle : étape 2.
      * Les positions des centres (5 dimensions) des superpixels sont mises à jour selon la valeur moyenne de tous les pixels du superpixel.
      */

    vector<int> spix_cpt;
    spix_cpt.resize(centers.size(), 0);

    // Réinitialisation des valeurs des centres des superpixels, pour établir les valeurs moyennes
    for(int i = 0; i < centers.size(); i++) {
        centers[i][R] = 0;
        centers[i][G] = 0;
        centers[i][B] = 0;
        centers[i][X] = 0;
        centers[i][Y] = 0;
    }
    cout << "yalaaaaaaaaaa ...a?" << endl;

    for(int y = 0; y < img->nH*3; y++) {
        for(int x = 0; x < img->nW; x++) {
            // Center : centre super pixel
            int spix = clusters[y][x]; // Retourne super pixel associé au pixel x y

            // Couleurs du Pixel (x,y)
            int pR = img->ImgData[y * img->nW + x];
            int pG = img->ImgData[y * img->nW + x + 1];
            int pB = img->ImgData[y * img->nW + x + 2];

            // Somme des valeurs du superpixel
            centers[spix][R] += pR;
            centers[spix][G] += pG;
            centers[spix][B] += pB;
            centers[spix][X] += x;
            centers[spix][Y] += y;
            spix_cpt[spix] += 1;
        }
    }
    cout << "yalaaaaaaaaaaAAAAAAAAAAAAa" << endl;

    for(int i = 0; i < centers.size(); i++) {
        centers[i][R] /= spix_cpt[i];
        centers[i][G] /= spix_cpt[i];
        centers[i][B] /= spix_cpt[i];
        centers[i][X] /= spix_cpt[i];
        centers[i][Y] /= spix_cpt[i];
    }

    /** TODO : Fin boucle
      * Les étapes 2 et 3 sont re-effectuées jusqu'à ce que l'algorithme converge, c'est-à-dire, que le déplacement moyen des centres entre
      * deux itérations soit plus petit qu'un certain seuil.
      */

    for(int y = 0; y < img->nH * 3; y++) {
        for(int x = 0; x < img->nW; x++) {
            vector<double> pix = centers[clusters[y][x]];
            //cout << "x : " << x << " - y : " << y << " - SuuuupaPixel : " << clusters[y][x] << endl;
            img->ImgData[y * img->nW + x] = pix[R];
            img->ImgData[y * img->nW + x + 1] = pix[G];
            img->ImgData[y * img->nW + x + 2] = pix[B];
        }
    }


}

Image *SuperPixels::GetImage() {
    return img;
}

/** Initialisation des centres des superpixels.
 * Les centres sont équidistants (distance S) et sont en 5 dimensions (R,G,B,x,y) pour leurs coordonnées
 * spatiales et la couleur du pixel dans l'espace RGB.
 */
void SuperPixels::InitCenters() {
    for(int y = S; y < img->nH*3 - S/2; y+=S) {
        for(int x = S; x < img->nW*3 - S/2; x+=S) {
            vector<double> center;

            int pR = img->ImgData[y * img->nW + x];
            int pG = img->ImgData[y * img->nW + x + 1];
            int pB = img->ImgData[y * img->nW + x + 2];

            center.push_back(pR);
            center.push_back(pG);
            center.push_back(pB);
            center.push_back(x);
            center.push_back(y);

            centers.push_back(center);
        }
    }
}
