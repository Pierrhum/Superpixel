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

SuperPixels::SuperPixels(Image *input,  int displayMode, int nbPixels, int nbSuperPixels, double Step, int weight, int iteration) {
    this->img = input;
    this->N = nbPixels;
    this->K = nbSuperPixels;
    this->S = Step;
    this->m = weight;
    this->pixels = GetPixels();

    /** ALGO SLIC **/
    
    /// 1 - Initialisation des centres des superpixels.
    cout << "Initialisation des centres des superpixels." << endl;
    InitCenters();
    cout << centers.size() << " centres créés." << endl;

    if(displayMode==0) {
        DebugCenter();
        return;
    }


    /// 2 - Initialisation de la carte des superpixels et de la carte des distances.
    cout << "Initialisation de la carte des superpixels et de la carte des distances" << endl;
    InitClusters();
    cout << clusters.size() << " cartes créés." << endl;

    /**
     * Pour chaque centre, on calcule la distance des pixels se trouvant dans un rayon de 2S au centre.
     * Si la distance est plus petite que celle en mémoire pour le pixel, elle est modifiée en mémoire
     * et le pixel fait maintenant partie du superpixel.
     */
    for(int b=0; b < iteration; b++) {
        cout << "Loop " << b << endl;
        for (int j = 0; j < img->nW; j++) {
            for (int k = 0;k < img->nH; k++) {
                distances[j][k] = FLT_MAX;
            }
        }
        cout << "Calcul des distances" << endl;
        for(int i = 0; i < centers.size(); i++) {
            /* Only compare to pixels in a 2 x step by 2 x step region. */
            for (int Cx = centers[i]->x - S; Cx < centers[i]->x + S; Cx++) {
                for (int Cy = centers[i]->y - S; Cy < centers[i]->y + S; Cy++) {
                    /* Si les centres des pixels sont bien dans l'image */
                    if(Cx >= 0 && Cx < img->nW && Cy >= 0 && Cy < img->nH) {
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
            centers[i]->pR = 0;
            centers[i]->pG = 0;
            centers[i]->pB = 0;
            centers[i]->x = 0;
            centers[i]->y = 0;
        }
        
        cout << "Calcul des sommes pour la moyenne" << endl;
        for(int x = 0; x < img->nW; x++) {
            for(int y = 0; y < img->nH; y++) {
                // Center : centre super pixel
                int spix = clusters[x][y]; // Retourne super pixel associé au pixel x y

                // Couleurs du Pixel (x,y)
                Pixel* pix = pixels[y * img->nW + x];

                // Somme des valeurs du superpixel
                centers[spix]->pR += pix->pR;
                centers[spix]->pG += pix->pG;
                centers[spix]->pB += pix->pB;
                centers[spix]->x += x;
                centers[spix]->y += y;
                spix_cpt[spix] += 1;
            }
        }

        cout << "Normalisation de la moyenne" << endl;
        for(int i = 0; i < centers.size(); i++) {
            spix_cpt[i] = spix_cpt[i]==0 ? 1 : spix_cpt[i];
            centers[i]->pR /= spix_cpt[i];
            centers[i]->pG /= spix_cpt[i];
            centers[i]->pB /= spix_cpt[i];
            centers[i]->x /= spix_cpt[i];
            centers[i]->y /= spix_cpt[i];
        }
    /** TODO : Fin boucle
      * Les étapes 2 et 3 sont re-effectuées jusqu'à ce que l'algorithme converge, c'est-à-dire, que le déplacement moyen des centres entre
      * deux itérations soit plus petit qu'un certain seuil.
      */
    }

    cout << "Connectivity" << endl;
    if(displayMode<=2)
        Connectivity();


    cout << "Ecriture de l'image" << endl;
    for(int x = 0; x < img->nW; x++) {
        for(int y = 0; y < img->nH; y++) {
            img->ImgData[y*3 * img->nW + (x*3)] = 0;
            img->ImgData[y*3 * img->nW + (x*3 + 1)] = 0;
            img->ImgData[y*3 * img->nW + (x*3 + 2)] = 0;
        }
    }
    for(int x = 0; x < img->nW; x++) {
        for(int y = 0; y < img->nH; y++) {
            Center* c = centers[clusters[x][y]];
            img->ImgData[y*3 * img->nW + (x*3)] = c->pR;
            img->ImgData[y*3 * img->nW + (x*3 + 1)] = c->pG;
            img->ImgData[y*3 * img->nW + (x*3 + 2)] = c->pB;
        }
    }

    if(displayMode==2)
        DrawContour();

    
}

vector<Pixel*> SuperPixels::GetPixels()
{
    vector<Pixel*> pixels;
    for(int i = 0; i < img->nTaille3; i+=3) {
        Pixel *p = (struct Pixel*) malloc(sizeof(Pixel));
        p->pR = img->ImgData[i];
        p->pG = img->ImgData[i + 1];
        p->pB = img->ImgData[i + 2];
        pixels.push_back(p);
    }
    return pixels;
}

Image *SuperPixels::GetImage() {
    return img;
}

/**
 * Les pixels ayant tous des voisins valides sont les premiers à être modifiés et être inclus au superpixel de leurs voisins. 
 * Ensuite, itérativement, les pixels avec le plus de voisins valides sont modifiés jusqu'à ce que tous les pixels aient été traités. 
 * Dans le cas où un pixel aurait plusieurs superpixels différents comme voisins, le superpixel le plus fréquent prend le pixel.
 */

void SuperPixels::Connectivity() {
    const int dx[8] = {-1, -1,  0,  1, 1, 1, 0, -1};
	const int dy[8] = { 0, -1, -1, -1, 0, 1, 1,  1};

    for(int b=0; b < 5; b++) {
        for(int i = 0; i < img->nW; i++) {
                for(int j = 0; j < img->nH; j++) {
                    int cpt = 0;
                    // Cluster, cpt
                    map<int, int> newClusters;
                    /* Compare the pixel to its neighbours. */
                    for (int k = 0; k < 8; k++) {
                        int x = i + dx[k];
                        int y = j + dy[k];
                        if (x >= 0 && x < img->nW && y >= 0 && y < img->nH) {
                            if(clusters[i][j] != clusters[x][y]) {
                                auto c = newClusters.find(clusters[x][y]);
                                if(c != newClusters.end()) {
                                    newClusters[clusters[x][y]] += 1 ;

                                }
                                else
                                    newClusters.insert(pair<int,int>(clusters[x][y], 1));
                                cpt++;
                            }
                        }
                    }
                    if(cpt > 4) {
                        int frequency=0;
                        int newCluster;
                        for(auto item : newClusters) {
                            if(item.second > frequency) {
                                frequency = item.second;
                                newCluster = item.first;
                            }
                        }
                        // New cluster
                        clusters[i][j] = newCluster;
                    }
                }
            }
    }
    
}

/** Initialisation des centres des superpixels.
 * Les centres sont équidistants (distance S) et sont en 5 dimensions (R,G,B,x,y) pour leurs coordonnées
 * spatiales et la couleur du pixel dans l'espace RGB.
 */
void SuperPixels::InitCenters() {


    cout << img->nW / 2 << endl;
    cout << S << endl;

    for(int x = S; x <= img->nW - S/2; x+=S) {
        for(int y = S; y <= img->nH - S/2; y+=S) {

            vector<int> centerPos = AdaptCenter(x,y);

            Pixel* p = pixels[y * img->nW + x];

            // Pixel à comparer

            Center* c = (struct Center*) malloc(sizeof(Center));
            c->pR = p->pR;
            c->pG = p->pG;
            c->pB = p->pB;
            c->x = x;
            c->y = y;

            centers.push_back(c);
        }
    }
}
/**
 * Les positions initiales des centres sont légèrement déplacées en x,y de ±1 pixel afin qu'ils tombent sur un emplacement x,y 
 * avec un petit gradient pour éviter d'avoir un centre sur une arête.
 */
vector<int> SuperPixels::AdaptCenter(int x, int y) {
    float min_grad = FLT_MAX;
    vector<int> centerPos;
    centerPos.resize(2,0);

    for(int i=(x-1); i < x+2; i++) {
        for(int j=(y-1); j < y+2; j++) {
            Pixel* p1 = pixels[(j+1) * img->nW + i];
            Pixel* p2 = pixels[j * img->nW + i+1];
            Pixel* p3 = pixels[j * img->nW + i];
            double i1 = p1->pR * 0.11 + p1->pG * 0.59 + p1->pB * 0.3;
            double i2 = p2->pR * 0.11 + p2->pG * 0.59 + p2->pB * 0.3;
            double i3 = p3->pR * 0.11 + p3->pG * 0.59 + p3->pB * 0.3;
            if(sqrt(pow(i1 - i3, 2)) + sqrt(pow(i2 - i3,2)) < min_grad) {
                min_grad = fabs(i1 - i3) + fabs(i2 - i3);
                centerPos[0] = i;
                centerPos[1] = j;
            }
        }
    }

    return centerPos;

}


/** Initialisation de la carte des superpixels et de la carte des distances. 
 * Les distances sont initialisées à l'infini et les pixels sont attribués au superpixel 0.
 */
void SuperPixels::InitClusters() {
    for(int x = 0; x < this->img->nW; x++) {
        vector<int> cluster;
        vector<float> distance;
        for(int y = 0; y < this->img->nH; y++) {
            // x : 1279 y : 853
            cluster.push_back(0);
            distance.push_back(FLT_MAX);
        }
        clusters.push_back(cluster);
        distances.push_back(distance);
    }
}

double SuperPixels::GetDistance(Center* Ck, int Xi, int Yi) {
    
    // Pixel à comparer
    Pixel* pix = pixels[Yi * img->nW + Xi]; 

    // Calcul de la distance
    double dRGB = sqrt(pow(Ck->pR - pix->pR, 2) + pow(Ck->pG - pix->pG, 2) + pow(Ck->pB - pix->pB, 2));
    double dXY = sqrt(pow(Ck->x - Xi, 2) + pow(Ck->y - Yi, 2));
    double dist = dRGB + m/S * dXY;

    return dist;
}

void SuperPixels::DrawContour() {
    const int dx[8] = {-1, -1,  0,  1, 1, 1, 0, -1};
	const int dy[8] = { 0, -1, -1, -1, 0, 1, 1,  1};

    vector<bool> line;
    line.resize(img->nH, false);
    vector<vector<bool>> draw;
    draw.resize(img->nW, line);

    for(int i = 0; i < img->nW; i++) {
        for(int j = 0; j < img->nH; j++) {
            /* Compare the pixel to its 8 neighbours. */
            for (int k = 0; k < 8; k++) {
                int x = i + dx[k];
                int y = j + dy[k];
                if (x >= 0 && x < img->nW && y >= 0 && y < img->nH) {
                    if(!draw[x][y] && !draw[i][j] && clusters[i][j] != clusters[x][y]) {
                        draw[x][y] = true;
                        img->ImgData[y*3 * img->nW + (x*3)] = 255;
                        img->ImgData[y*3 * img->nW + (x*3 + 1)] = 0;
                        img->ImgData[y*3 * img->nW + (x*3 + 2)] = 0;
                    }
                }
            }
        }
    }
}

void SuperPixels::DebugCenter() {
    const int dx[8] = {-1, -1,  0,  1, 1, 1, 0, -1};
	const int dy[8] = { 0, -1, -1, -1, 0, 1, 1,  1};

    for(int x = 0; x < img->nW; x++) {
        for(int y = 0; y < img->nH; y++) {
            img->ImgData[y*3 * img->nW + (x*3)] = 0;
            img->ImgData[y*3 * img->nW + (x*3 + 1)] = 0;
            img->ImgData[y*3 * img->nW + (x*3 + 2)] = 0;
        }
    }
    int radius=5;
    for(Center* c : centers) {
        for (int k = 0; k < 8; k++) {
            for(int r=1; r < radius; r++) {
                int x = c->x + (dx[k]*r);
                int y = c->y + (dy[k]*r);
                if (x >= 0 && x < img->nW && y >= 0 && y < img->nH) {
                    img->ImgData[y*3 * img->nW + (x*3)] = 255;
                }

            }
        }
        
    }
}
