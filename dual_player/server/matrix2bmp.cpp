#include "matrix2bmp.hpp"

int main ()
{
    unsigned char image[sheight][swidth][BYTES_PER_PIXEL];
    char* imageFileName = (char*) "iitdmapImage.bmp";
    
    colorRect(image, 0, 0, width, height, GREEN_BG);
    
    makePaths(image);
    

    generateBitmapImage((unsigned char*) image, sheight, swidth, imageFileName);
    printf("Image generated!!");
}