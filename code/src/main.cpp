#include "Image.h"

int main(int argc, char* argv[])
{
  char cNomImgLue[250], cNomImgEcrite[250] = "../output.ppm";
  int K, m;
  
  if (argc != 4)
     {
       printf("Usage: ImageIn.ppm Regions \n"); 
       exit (1) ;
     }
   
   sscanf (argv[1],"%s",cNomImgLue) ;
   sscanf (argv[2],"%d",&K);
    sscanf (argv[3],"%d",&m);

   Image *input = new Image(cNomImgLue);
   Image *output = input->ToSuperPixelsBySLIC(K, m);
   output->WriteFile(cNomImgEcrite);
}