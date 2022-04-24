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

    vector<Pixel> pixels = GetPixels();

    /** ALGO SLIC **/
    
    /// Initialisation des centres des superpixels.
    cout << "Initialisation des centers" << endl;
    InitCenters();
    cout << centers.size() << " centres créés." << endl;


    /** TODO : Initialisation de la carte des superpixels et de la carte des distances.
     * Les distances sont initialisées à l'infini et les pixels sont attribués au superpixel 0.
     */
    cout << "Initialisation de la carte des superpixels et de la carte des distances" << endl;
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
    cout << clusters.size() << " cartes créés." << endl;

    /**
     * Pour chaque centre, on calcule la distance des pixels se trouvant dans un rayon de 2S au centre.
     * Si la distance est plus petite que celle en mémoire pour le pixel, elle est modifiée en mémoire
     * et le pixel fait maintenant partie du superpixel.
     */
    cout << "Calcul des distances" << endl;
    for(int i = 0; i < centers.size(); i++) {
        /* Only compare to pixels in a 2 x step by 2 x step region. */
        for (int Cx = centers[i].x - S; Cx < centers[i].x + S; Cx++) {
            for (int Cy = centers[i].y - S; Cy < centers[i].y + S; Cy++) {
                /* Si les centres des pixels sont bien dans l'image */
                if(Cx >= 0 && Cx < img->nH && Cy >= 0 && Cy < img->nW) {
                    // Pixel à comparer
                    double dist = GetDistance(centers[i], Cx, Cy);

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

    cout << "Réinitialisation des valeurs des centres" << endl;
    // Réinitialisation des valeurs des centres des superpixels, pour établir les valeurs moyennes
    for(int i = 0; i < centers.size(); i++) {
        centers[i].pR = 0;
        centers[i].pG = 0;
        centers[i].pB = 0;
        centers[i].x = 0;
        centers[i].y = 0;
    }
    
    cout << "Calcul des sommes pour la moyenne" << endl;
    for(int y = 0; y < img->nH*3; y++) {
        for(int x = 0; x < img->nW; x++) {
            // Center : centre super pixel
            int spix = clusters[y][x]; // Retourne super pixel associé au pixel x y

            // Couleurs du Pixel (x,y)
            vector<int> pix = GetPixelAt(x,y); 

            // Somme des valeurs du superpixel
            centers[spix].pR += pix[R];
            centers[spix].pG += pix[G];
            centers[spix].pB += pix[B];
            centers[spix].x += x;
            centers[spix].y += y;
            spix_cpt[spix] += 1;
        }
    }

    cout << "Normalisation de la moyenne" << endl;
    for(int i = 0; i < centers.size(); i++) {
        spix_cpt[i] = spix_cpt[i]==0 ? 1 : spix_cpt[i];
        centers[i].pR /= spix_cpt[i];
        centers[i].pG /= spix_cpt[i];
        centers[i].pB /= spix_cpt[i];
        centers[i].x /= spix_cpt[i];
        centers[i].y /= spix_cpt[i];
    }

    /** TODO : Fin boucle
      * Les étapes 2 et 3 sont re-effectuées jusqu'à ce que l'algorithme converge, c'est-à-dire, que le déplacement moyen des centres entre
      * deux itérations soit plus petit qu'un certain seuil.
      */

    cout << "Ecriture de l'image" << endl;
    for(int y = 0; y < img->nH * 3; y++) {
        for(int x = 0; x < img->nW; x++) {
            Center c = centers[clusters[y][x]];
            //cout << "x : " << x << " - y : " << y << " - SuuuupaPixel : " << clusters[y][x] << endl;
            img->ImgData[y * img->nW + x] = c.pR;
            img->ImgData[y * img->nW + x + 1] = c.pG;
            img->ImgData[y * img->nW + x + 2] = c.pB;
        }
    }


}

vector<Pixel> SuperPixels::GetPixels()
{
    vector<Pixel> pixels;
    for(int i = 0; i < img->nTaille3; i+=3) {
        Pixel p;
        p.pR = img->ImgData[i];
        p.pG = img->ImgData[i + 1];
        p.pB = img->ImgData[i + 2];
        pixels.push_back(p);
    }
    return pixels;
}

Image *SuperPixels::GetImage() {
    return img;
}

/** Initialisation des centres des superpixels.
 * Les centres sont équidistants (distance S) et sont en 5 dimensions (R,G,B,x,y) pour leurs coordonnées
 * spatiales et la couleur du pixel dans l'espace RGB.
 */
void SuperPixels::InitCenters() {

    cout << "S : " << S << endl;

    for(int y = S/2; y < img->nH - S/2; y+=S) {
        for(int x = S/2; x < img->nW - S/2; x+=S) {
            vector<double> center;

            // Pixel à comparer
            vector<int> pix = GetPixelAt(x,y); 

            Center c;
            c.pR = pix[R];
            c.pG = pix[G];
            c.pB = pix[B];
            c.x = x;
            c.y = y;

            cout << c.x << " " << c.y << endl;

            centers.push_back(c);
        }
    }
}

double SuperPixels::GetDistance(Center Ck, int Xi, int Yi) {
    
    // Pixel à comparer
    vector<int> pix = GetPixelAt(Xi,Yi); 

    // Calcul de la distance
    double dRGB = sqrt(pow(Ck.pR - pix[R], 2) + pow(Ck.pG - pix[G], 2) + pow(Ck.pB - pix[B], 2));
    double dXY = sqrt(pow(Ck.x - Xi, 2) + pow(Ck.y - Yi, 2));
    double dist = dRGB + m/S * dXY;

    return dist;
}

vector<int> SuperPixels::GetPixelAt(int x, int y) {
    vector<int> rgb;

    rgb.push_back(img->ImgData[y*img->nW+x]);
    rgb.push_back(img->ImgData[y*img->nW+(x+1)]);
    rgb.push_back(img->ImgData[y*img->nW+(x+2)]);

    return rgb;
}
