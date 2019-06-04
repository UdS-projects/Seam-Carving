#include <stdlib.h>
#include <string.h>

#include "image.h"
#include "calculate.h"

void image_destroy(image_t* img);
void printArray3(int* array, int w, int h)
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

image_t* image_init(FILE* f)
{
    char* header = malloc(5*sizeof(char));
    fscanf(f, "%s", header);
    if(strcmp(header, "P3") != 0)
    {
        printf("Image header line 1 isn't formatted correctly (use \"P3\\n\")");
        free(header);
        return NULL;
    }
    free(header);
    
    int w = 0;
    int h = 0;
    if(fscanf(f, "%i %i", &w, &h) != 2)
    {
        return NULL;
    }
    if(w <= 0 && h <= 0)
    {
        return NULL;
    }
    
    int i255 = 0;
    fscanf(f,"%i", &i255);
    if(255 != i255)
    {
        return NULL;
    }
    
    //printf("%li\n", sizeof(image_t));
    image_t *img = (image_t*) malloc(sizeof(image_t));
    img->w = w;
    img->h = h;
    img->data = malloc(w*h*3*sizeof(int));
    
    int currentPixel = 0;
    int pixelCount = 0;
    while(fscanf(f, "%i", &currentPixel) != EOF)
    {
        if(currentPixel < 0 || currentPixel > 255 || pixelCount >= w*h*3)
        {
            image_destroy(img);
            return NULL;
        }
        img->data[pixelCount] = currentPixel;
        pixelCount++;
    }
    
    if(pixelCount != w*h*3)
    {
        image_destroy(img);
        return NULL;
    }
    
    fclose(f);
    return img;
}

void image_destroy(image_t* img)
{
    free(img->data);
    free(img);
} 

image_t* resize_picture(image_t* img, int cc)  //Completed Columns
{
    if(cc > 0)
    {
        //kleineres Bild ohne die schwarzen Balken anlegen
        image_t *smaller_img = (image_t*) malloc(sizeof(image_t));
        smaller_img->w = ((img->w)-cc);
        smaller_img->h = img->h;
        smaller_img->data = malloc((smaller_img->w)*(smaller_img->h)*3*sizeof(int));
        
        //inhalt des großen bildes (links vom schwarzen balken) ins kleine schreiben
        int edgeCheck = 0;
        int indexBig = 0;
        for(int indexSmall = 0; indexSmall < smaller_img->w*smaller_img->h*3; indexSmall++)
        {
            if(edgeCheck != (smaller_img->w*3))
            {
                edgeCheck++;
                smaller_img->data[indexSmall] = img->data[indexBig];
                indexBig++;
            }
            else
            {
                edgeCheck = 1;
                indexBig = indexBig + cc*3;
                smaller_img->data[indexSmall] = img->data[indexBig];
                indexBig++;
            }
        }
        
        //printArray3(smaller_img->data, smaller_img->w*3, smaller_img->h);
        
        //optimal path im kleineren bild bekommen
        int* array = local_energy(smaller_img);
        accumulated_energy(array, smaller_img->w, smaller_img->w*smaller_img->h);
        int* array2 = optimal_path(array, smaller_img->w, smaller_img->h);
        free(array);
        
        //kleines bild ohne änderungen printen
        //printf("\n");
        //printArray3(array2, 1, smaller_img->h);
        
        //pixel rechts vom path nach links verschieben
        int width = smaller_img->w;
        int height = smaller_img->h;
        for(int i=0; i < smaller_img->h; i++)
        {
            int x_3 = array2[i] * 3;
            for(int j=0; j < width*3-x_3-3; j++)
            {
                smaller_img->data[(width*(height-1-i))*3+x_3+j] = smaller_img->data[(width*(height-i-1))*3+x_3+3+j];
            }
        }
        free(array2);
        
        //rechteste spalte auf schwarz setzen
        int check = 0;
        int integer = width*3-3;
        for(int i=0; i < smaller_img->w*smaller_img->h*3; i++)
        {
            if(check == integer)
            {
                for(int j=0; j<3; j++)
                {
                    smaller_img->data[i+j] = 0;
                }
                i += 2;
                check = 0;
                continue;
            }
            check++;
        }
        
        //kleines bild zurück ins große schreiben
        edgeCheck = 0;
        indexBig = 0;
        for(int indexSmall = 0; indexSmall < smaller_img->w*smaller_img->h*3; indexSmall++)
        {
            if(edgeCheck != (smaller_img->w*3))
            {
                edgeCheck++;
                img->data[indexBig] = smaller_img->data[indexSmall];
                indexBig++;
            }
            else
            {
                edgeCheck = 1;
                indexBig = indexBig + cc*3;
                img->data[indexBig] = smaller_img->data[indexSmall];
                indexBig++;
            }
        }
        
        return img;
    }
    else
    {
        //optimal path im bild bekommen
        int* array = local_energy(img);
        accumulated_energy(array, img->w, img->w*img->h);
        int* array2 = optimal_path(array, img->w, img->h);
        free(array);
        
        //bild ohne änderungen printen
//         printf("\n");
//         printArray3(array2, 1, img->h);
        
        //pixel rechts vom path nach links verschieben
        int width = img->w;
        int height = img->h;
        for(int i=0; i < img->h; i++)
        {
            int x_3 = array2[i] * 3;
            for(int j=0; j < width*3-x_3-3; j++)
            {
                img->data[(width*(height-1-i))*3+x_3+j] = img->data[(width*(height-i-1))*3+x_3+3+j];
            }
        }
        free(array2);
        
        //rechteste spalte auf schwarz setzen
        int check = 0;
        int integer = width*3-3;
        for(int i=0; i < img->w*img->h*3; i++)
        {
            if(check == integer)
            {
                for(int j=0; j<3; j++)
                {
                    img->data[i+j] = 0;
                }
                i += 2;
                check = 0;
                continue;
            }
            check++;
        }
        
        return img;
    }
}


void image_write_to_file(image_t* img)
{
    FILE *f;
    f = fopen("out.ppm", "w");
    
    fprintf(f, "P3\n");
    fprintf(f, "%i %i\n", img->w, img->h);
    fprintf(f, "255\n");
    for(int j=0; j<img->h; j++)
    {
        for(int i=0; i<img->w; i++)
        {
            fprintf(f,"%i ", img->data[i]);
        }
        fprintf(f,"\n");
    }
    fclose(f);
}
