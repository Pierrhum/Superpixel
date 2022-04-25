#include "Image.h"
#include <iostream>
#include <vector>

int main(int argc, char* argv[])
{
  char cNomImgLue[250], cNomImgEcrite[250] = "../output.ppm";
  int displayContour, K, m, iteration;
  iteration = 10;
  
  if (argc < 5 || argc > 6)
     {
       printf("Usage: img[ImageIn] displayContour[0=true] K[Regions] m[Weight] \n"); 
       exit (1) ;
     }
   sscanf (argv[1],"%s",cNomImgLue);
   sscanf (argv[2],"%d",&displayContour);
   sscanf (argv[3],"%d",&K);
   sscanf (argv[4],"%d",&m);

   if(argc==6)
    sscanf (argv[5],"%d",&iteration);

   Image *input = new Image(cNomImgLue);
   Image *output = input->ToSuperPixelsBySLIC(K, m, displayContour, iteration);
   
    std::vector<uint> encoded_output;

    output->rle_encode(output->ImgData,output->nTaille3,encoded_output);

    OCTET* decoded_output;
    uint decoded_output_length;

    output->rle_decode(encoded_output,&decoded_output,decoded_output_length);

    std::cout << output->nTaille3 << " > " << encoded_output.size() << " > " << decoded_output_length << std::endl;

    output->WriteFile(cNomImgEcrite);
    
    free(output->ImgData);
    free(decoded_output);
}