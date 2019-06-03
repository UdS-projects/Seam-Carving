#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
 
#include "image.h"
#include "calculate.h"

void printArray2(int* array, int height)
{
    for(int i=0; i<height; i++)
    {
        printf("%i\n", array[i]);
    }
}

void printArray(int* array, int w, int h)
{
    for(int i=0; i<w*h; )
    {
        int j=0;
        while(j<w)
        {
            printf("%i ", array[i]);
            i++;
            j++;
        }
        printf("\n");
    }
}
    

int main(int const argc, char** const argv)
{
    if( argc < 2)    //|| strcmp("./carve", argv[0]) != 0) funktioniert nicht, da die tests es anders aufrufen
    {
        printf("The function carve requires at least one parameter.\n");
		printf("syntax: carve [-s] [-p] [-n <count>] <image file>\n");
		return EXIT_FAILURE;
	}

	FILE* f;
	int imageCheck = 0;

 	for(int i = 1; i < 5; ++i)
    {
         if((f = fopen(argv[i], "r")) != NULL)
         {
             imageCheck = i;
             break;
         }
     }
     if(imageCheck == 0)
     {
         printf("No valid image file given.\n");
         return EXIT_FAILURE;
     }
    
    image_t* img = image_init(f);
    if(img == NULL)
    {
        printf("Image file was broken.\n"); 
        return EXIT_FAILURE;
    }
    int width = img->w;
    int height = img->h;
    
    int tFlag = 0;
    int sFlag = 0;
    int pFlag = 0;
    int nFlag = 0;
    //nCount = -1 means no argument, aka execute all steps
    int nCount = -1;
    int option;
    //getopt ist eine hurensohn scheiße.
    while((option = getopt(argc, argv, "tspn:")) != -1)
    {
        switch(option)
        {
            case 't':
                tFlag = 1;
                break;
                
            case 's':
                sFlag = 1;
                break;
                
            case 'p':
                pFlag = 1;
                break;
                
            case 'n':
                nFlag = 1;
                if(strcmp(optarg, "0") == 0)
                {
                    nCount = 0;
                    break;
                }
                int optNum = atoi(optarg);
                if(optNum == 0 || optNum < -1 || optNum > width)
                {
                    printf("The operator -n only accepts numerical parameters smaller than image width and larger than -2.");
                    return EXIT_FAILURE;
                }
                nCount = optNum;
                break;
                
            case '?':
                //if s or p are set, state of n is not meaningful
                if(optopt == 'n' && sFlag == 0 && pFlag == 0)
                {
                    printf("The operator -n requires a parameter");
                    return EXIT_FAILURE;
                }
                break;
                
            default:
                break;
        }
    }
    
    if(tFlag == 1)
    {
//         printArray(img->data, width*3, height);
//         printf("\n");
//         image_t *img2 = resize_picture(img, 1);
//         printf("\n");
//         printArray(img2->data, img2->w*3, img2->h);
//         image_write_to_file(img);
//         image_destroy(img);
//         image_destroy(img2);
//         return EXIT_SUCCESS;
        
        image_write_to_file(img);
        image_destroy(img);
        return EXIT_SUCCESS;
    }
    
    if(sFlag == 1)
    {
        int brightness = calculateBrightness(img);
        printf("width: %u\n", width);
        printf("height: %u\n", height);
        printf("brightness: %u\n", brightness);
        image_destroy(img);
        return EXIT_SUCCESS;
    }
    
//     if(pFlag == 1)
//     {
//         int* array = local_energy(img);
//         printArray(array, width, height);
//         printf("\n");
//         printf("\n");
//         accumulated_energy(array, width, width*height);
//         printArray(array, width, height);
//         printf("\n");
//         printf("\n");
//         int* array2 = optimal_path(array, width, height);
//         free(array);
//         printArray2(array2, height);
//         free(array2);
//         image_destroy(img);
//         return EXIT_SUCCESS;
//     }

    if(pFlag == 1)
    {
        int* array = local_energy(img);
        accumulated_energy(array, width, width*height);
        int* array2 = optimal_path(array, width, height);
        free(array);
        printArray2(array2, height);
        free(array2);
        image_destroy(img);
        return EXIT_SUCCESS;
    }
    
    if(nFlag == 1)
    {
        if(nCount > -1)
        {
            for(int i=0; i<nCount; i++)
            {
                resize_picture(img, i);
            }
            image_write_to_file(img);
            image_destroy(img);
            return EXIT_SUCCESS;
        }
    }
    
    for(int i=0; i<width; i++)
    {
        resize_picture(img, i);
    }
    image_write_to_file(img);
    image_destroy(img);
    return EXIT_SUCCESS;
}
