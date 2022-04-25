#include <iostream>
#include "SLIC.h"

SLIC::SLIC(Image *input, int displayMode, int nbPixels, int nbSuperPixels, double Step, int weight) {
    this->img = input;
    this->N = nbPixels;
    this->K = nbSuperPixels;
    this->S = Step;
    this->m = weight;
    InitPixels();

    /** ALGO SLIC **/
    
    /// 1 - Initialisation des centres des superpixels.
    cout << "Initialisation des centres des superpixels." << endl;
    InitCenters();

    if(displayMode==0) {
        DebugCenter(5);
        return;
    }

    /// 2 - Initialisation de la carte des superpixels et de la carte des distances.
    cout << "Initialisation de la carte des superpixels et de la carte des distances" << endl;
    InitClusters();

    /**
     * Pour chaque centre, on calcule la distance des pixels se trouvant dans un rayon de 2S au centre.
     * Si la distance est plus petite que celle en mémoire pour le pixel, elle est modifiée en mémoire
     * et le pixel fait maintenant partie du superpixel.
     */
    for(int b=0; b < 10; b++) {
        cout << "\x1B[2J\x1B[H";
        cout << "Loop " << b << endl;
        for (int j = 0; j < img->nW; j++) {
            for (int k = 0;k < img->nH; k++) {
                Pixels[k * img->nW + j]->dist = FLT_MAX;
            }
        }
        cout << "Calcul des distances" << endl;
        for(int i = 0; i < SuperPixels.size(); i++) {
            /* Only compare to pixels in a 2 x step by 2 x step region. */
            for (int Cx = SuperPixels[i]->center->x - S; Cx < SuperPixels[i]->center->x + S; Cx++) {
                for (int Cy = SuperPixels[i]->center->y - S; Cy < SuperPixels[i]->center->y + S; Cy++) {
                    /* Si les centres des pixels sont bien dans l'image */
                    if(Cx >= 0 && Cx < img->nW && Cy >= 0 && Cy < img->nH) {
                        // Pixel à comparer
                        Pixel* pix = Pixels[Cy * img->nW + Cx]; 
                        double dist = SuperPixels[i]->GetDistance(pix, m, S);

                        // Association du pixel au superpixel si distance inférieure au précédent superpixel
                        if(dist < pix->dist) {
                            pix->dist = dist;
                            pix->cluster = i;
                        }
                    }
                }
            }
        }


        /** TODO : Boucle : étape 2.
         * Les positions des centres (5 dimensions) des superpixels sont mises à jour selon la valeur moyenne de tous les pixels du superpixel.
         */

        vector<int> spix_cpt;
        spix_cpt.resize(SuperPixels.size(), 0);

        cout << "Réinitialisation des valeurs des centres" << endl;
        // Réinitialisation des valeurs des centres des superpixels, pour établir les valeurs moyennes
        for(int i = 0; i < SuperPixels.size(); i++) {
            SuperPixels[i]->center->color.R = 0;
            SuperPixels[i]->center->color.G = 0;
            SuperPixels[i]->center->color.B = 0;
            SuperPixels[i]->center->x = 0;
            SuperPixels[i]->center->y = 0;
        }
        
        cout << "Calcul des sommes pour la moyenne" << endl;
        for(int x = 0; x < img->nW; x++) {
            for(int y = 0; y < img->nH; y++) {
                // Couleurs du Pixel (x,y)
                Pixel* pix = Pixels[y * img->nW + x];
                // Center : centre super pixel
                int spix = pix->cluster; // Retourne super pixel associé au pixel x y


                // Somme des valeurs du superpixel
                SuperPixels[spix]->center->color.R += pix->color.R;
                SuperPixels[spix]->center->color.G += pix->color.G;
                SuperPixels[spix]->center->color.B += pix->color.B;
                SuperPixels[spix]->center->x += x;
                SuperPixels[spix]->center->y += y;
                spix_cpt[spix] += 1;
            }
        }

        cout << "Normalisation de la moyenne" << endl;
        for(int i = 0; i < SuperPixels.size(); i++) {
            spix_cpt[i] = spix_cpt[i]==0 ? 1 : spix_cpt[i];
            SuperPixels[i]->center->color.R /= spix_cpt[i];
            SuperPixels[i]->center->color.G /= spix_cpt[i];
            SuperPixels[i]->center->color.B /= spix_cpt[i];
            SuperPixels[i]->center->x /= spix_cpt[i];
            SuperPixels[i]->center->y /= spix_cpt[i];
        }
    /** TODO : Fin boucle
      * Les étapes 2 et 3 sont re-effectuées jusqu'à ce que l'algorithme converge, c'est-à-dire, que le déplacement moyen des centres entre
      * deux itérations soit plus petit qu'un certain seuil.
      */
    }

    cout << "Connectivity" << endl;
    //Connectivity();

    for(Pixel* pixel : Pixels) {
        SuperPixels[pixel->cluster]->pixels.push_back(pixel);
    }

    cout << "Ecriture de l'image" << endl;
    for(Pixel* pixel : Pixels) {
        SuperPixel* spix = SuperPixels[pixel->cluster];
        img->ImgData[(pixel->y*3) * img->nW + (pixel->x*3)] = spix->center->color.R;
        img->ImgData[(pixel->y*3) * img->nW + (pixel->x*3 + 1)] = spix->center->color.G;
        img->ImgData[(pixel->y*3) * img->nW + (pixel->x*3 + 2)] = spix->center->color.B;
    }
    
    cout << SuperPixels.size() << " superpixels créés." << endl;

    if(displayMode==2)
        DrawContour();
}

/** Initialisation de la carte des superpixels et de la carte des distances. 
 * Les distances sont initialisées à l'infini et les pixels sont attribués au superpixel 0.
 */
void SLIC::InitPixels()
{
    Pixels.resize(img->nW * img->nH);

    for(int x=0; x < img->nW; x++) {
        for(int y=0; y < img->nH; y++) {
            Color color;
            color.R = img->ImgData[y*3 * img->nW + (x*3)];
            color.G = img->ImgData[y*3 * img->nW + (x*3 + 1)];
            color.B = img->ImgData[y*3 * img->nW + (x*3 + 2)];
            Pixel* pix = (struct Pixel*) malloc(sizeof(Pixel));
            pix->cluster = -1;
            pix->x = x;
            pix->y = y;
            pix->color.R = color.R;
            Pixels[y * img->nW + x] = pix;
        }
    }    
}

Image *SLIC::GetImage() {
    return img;
}

/**
 * Les pixels ayant tous des voisins valides sont les premiers à être modifiés et être inclus au superpixel de leurs voisins. 
 * Ensuite, itérativement, les pixels avec le plus de voisins valides sont modifiés jusqu'à ce que tous les pixels aient été traités. 
 * Dans le cas où un pixel aurait plusieurs superpixels différents comme voisins, le superpixel le plus fréquent prend le pixel.
 */

void SLIC::Connectivity() {
    const int dx[8] = {-1, -1,  0,  1, 1, 1, 0, -1};
	const int dy[8] = { 0, -1, -1, -1, 0, 1, 1,  1};

    for(int b=0; b < 5; b++) {
        for(int i = 0; i < img->nW; i++) {
                for(int j = 0; j < img->nH; j++) {
                    Pixel* pix = Pixels[j * img->nW + i]; 
                    int cpt = 0;
                    // Cluster, cpt
                    map<int, int> newClusters;
                    /* Compare the pixel to its neighbours. */
                    for (int k = 0; k < 8; k++) {
                        int x = i + dx[k];
                        int y = j + dy[k];
                        if (x >= 0 && x < img->nW && y >= 0 && y < img->nH) {
                            Pixel* neighbour = Pixels[y * img->nW + x]; 
                            if(pix->cluster != neighbour->cluster) {
                                auto c = newClusters.find(neighbour->cluster);
                                if(c != newClusters.end()) {
                                    newClusters[neighbour->cluster] += 1 ;

                                }
                                else
                                    newClusters.insert(pair<int,int>(neighbour->cluster, 1));
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
                        pix->cluster = newCluster;
                    }
                }
            }
    }
    
}

/** Initialisation des centres des superpixels.
 * Les centres sont équidistants (distance S) et sont en 5 dimensions (R,G,B,x,y) pour leurs coordonnées
 * spatiales et la couleur du pixel dans l'espace RGB.
 */
void SLIC::InitCenters() {

    for(int x = S; x <= img->nW - S/2; x+=S) {
        for(int y = S; y <= img->nH - S/2; y+=S) {

            vector<int> centerPos = AdaptCenter(x,y);

            Pixel* center = Pixels[y * img->nW + x];
            center->cluster = SuperPixels.size();

            SuperPixel* spix = new SuperPixel(center);
            spix->pixels.push_back(center);

            SuperPixels.push_back(spix);
        }
    }
}
/**
 * Les positions initiales des centres sont légèrement déplacées en x,y de ±1 pixel afin qu'ils tombent sur un emplacement x,y 
 * avec un petit gradient pour éviter d'avoir un centre sur une arête.
 */
vector<int> SLIC::AdaptCenter(int x, int y) {
    float min_grad = FLT_MAX;
    vector<int> centerPos;
    centerPos.resize(2,0);

    for(int i=(x-1); i < x+2; i++) {
        for(int j=(y-1); j < y+2; j++) {
            Pixel* p1 = Pixels[(j+1) * img->nW + i];
            Pixel* p2 = Pixels[j * img->nW + i+1];
            Pixel* p3 = Pixels[j * img->nW + i];
            double i1 = p1->color.R * 0.11 + p1->color.G * 0.59 + p1->color.B * 0.3;
            double i2 = p2->color.R * 0.11 + p2->color.G * 0.59 + p2->color.B * 0.3;
            double i3 = p3->color.R * 0.11 + p3->color.G * 0.59 + p3->color.B * 0.3;
            if(sqrt(pow(i1 - i3, 2)) + sqrt(pow(i2 - i3,2)) < min_grad) {
                min_grad = fabs(i1 - i3) + fabs(i2 - i3);
                centerPos[0] = i;
                centerPos[1] = j;
            }
        }
    }

    return centerPos;

}

void SLIC::InitClusters() {
    for(int x=0; x < img->nW; x++) {
        for(int y=0; y < img->nH; y++) {
            Pixel* pix = Pixels[y * img->nW + x];
            if(pix->cluster == -1) {
                pix->cluster = 0;
                pix->dist = FLT_MAX;
            }
        }
    }
}

void SLIC::DrawContour() {
    const int dx[8] = {-1, -1,  0,  1, 1, 1, 0, -1};
	const int dy[8] = { 0, -1, -1, -1, 0, 1, 1,  1};

    vector<bool> line;
    line.resize(img->nH, false);
    vector<vector<bool>> draw;
    draw.resize(img->nW, line);

    for(int i = 0; i < img->nW; i++) {
        for(int j = 0; j < img->nH; j++) {
            Pixel* pix = Pixels[j * img->nW + i]; 
            /* Compare the pixel to its 8 neighbours. */
            for (int k = 0; k < 8; k++) {
                int x = i + dx[k];
                int y = j + dy[k];
                if (x >= 0 && x < img->nW && y >= 0 && y < img->nH) {
                    Pixel* neighbour = Pixels[y * img->nW + x]; 
                    if(!draw[x][y] && !draw[i][j] && pix->cluster != neighbour->cluster) {
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

void SLIC::DebugCenter(int radius) {
    const int dx[8] = {-1, -1,  0,  1, 1, 1, 0, -1};
	const int dy[8] = { 0, -1, -1, -1, 0, 1, 1,  1};
    
    for(int x = 0; x < img->nW; x++) {
        for(int y = 0; y < img->nH; y++) {
            img->ImgData[y*3 * img->nW + (x*3)] = 0;
            img->ImgData[y*3 * img->nW + (x*3 + 1)] = 0;
            img->ImgData[y*3 * img->nW + (x*3 + 2)] = 0;
        }
    }
    
    for(SuperPixel* spix : SuperPixels) {
        Pixel* c = spix->center;
        for (int k = 0; k < 8; k++) {
            for(int r=1; r < radius+1; r++) {
                int x = c->x + (dx[k] * r);
                int y = c->y + (dy[k] * r);
                img->ImgData[(y*3) * img->nW + (x*3)] = 255;
            }
        }
    }
}