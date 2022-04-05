#include "Image.hpp"

int main(int argc, char* argv[])
{
  char cNomImgLue[250], cNomImgEcrite[250] = "output.ppm";
  int K;
  
  if (argc != 3) 
     {
       printf("Usage: ImageIn.ppm Regions \n"); 
       exit (1) ;
     }
   
   sscanf (argv[1],"%s",cNomImgLue) ;
   sscanf (argv[3],"%d",&K);

   Image *output = new Image(cNomImgLue);
   output->ToSuperPixels(K);
   output->WriteFile(cNomImgEcrite);
}