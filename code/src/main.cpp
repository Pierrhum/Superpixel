#include "Image.h"

int main(int argc, char* argv[])
{
  char cNomImgLue[250], cNomImgEcrite[250] = "../output.ppm";
  int displayContour, K, m;
  
  if (argc != 5)
     {
       printf("Usage: img[ImageIn] displayContour[0=true] K[Regions] m[Weight] \n"); 
       exit (1) ;
     }
   
   sscanf (argv[1],"%s",cNomImgLue);
   sscanf (argv[2],"%d",&displayContour);
   sscanf (argv[3],"%d",&K);
   sscanf (argv[4],"%d",&m);

   Image *input = new Image(cNomImgLue);
   Image *output = input->ToSuperPixelsBySLIC(K, m, displayContour);
   output->WriteFile(cNomImgEcrite);
   free(output->ImgData);
}