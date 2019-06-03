#include <stdlib.h>
#include <string.h>

#include "image.h"
#include "calculate.h" 

int calculateBrightness(image_t* img)
{
    int w = img->w;
    int h = img->h;
    int size = w*h;
    int ergebnis = 0;
    int currentpixel = 0;
    
    while(currentpixel < size*3)
    {
        int teilergebnis = 0;
        for(int i=0; i<3; ++i)
        {
            teilergebnis += img->data[currentpixel];
            ++currentpixel;
        }
        ergebnis += teilergebnis / 3;
    }
    return ergebnis / size;
}

int* local_energy(image_t* img)
{
    int w = img->w;
    int h = img->h;
    int* array = calloc(w*h, sizeof(int));
    int spot = 0;
    int currentpixel = 0;
    int check = w;
    int check2 = 1;
    
    while(spot < w*h)
    {
        int blue2 = 0;
        int blue2_oben = 0;
        
        if(check2 % w == 1)
        {
            check2++;
        }
        else 
        {
            int red = (img->data[currentpixel-3]) - (img->data[currentpixel]);
            int red2 = red * red;
            int green = (img->data[currentpixel-2]) - (img->data[currentpixel+1]);
            int green2 = green * green;
            int blue = (img->data[currentpixel-1]) - (img->data[currentpixel+2]);
            blue2 = red2 + green2 + blue * blue;
            check2++;
        }
        if (check > 0)
        {
            check--;
        }
        else
        {
            int red_oben = (img->data[currentpixel-(w*3)]) - (img->data[currentpixel]);
            int red2_oben = red_oben * red_oben;
            int green_oben = (img->data[currentpixel-((w*3)-1)]) - (img->data[currentpixel+1]);
            int green2_oben = green_oben * green_oben;
            int blue_oben = (img->data[currentpixel-((w*3)-2)]) - (img->data[currentpixel+2]);
            blue2_oben = red2_oben + green2_oben + blue_oben * blue_oben;
            
        }
        
        int result = blue2 + blue2_oben;
        array[spot] = result;
        spot++;
        currentpixel = currentpixel +3;
    }
    return array;
}

void accumulated_energy(int* local_energy_array, int width, int arraysize)
{
    int spot = 0;
    int check = width;
    int check2 = 1;
    
    while(spot < arraysize)
    {
        if(check > 0)
        {
            check--;
        }
        else 
        {
            if(check2 % width != 1 && check2 % width == 0)
            {
                check2++;
                int place = local_energy_array[spot-width]; 
                if(place <= local_energy_array[spot-width+1] && place <= local_energy_array[spot-width-1])
                {
                    int add = local_energy_array[spot];
                    local_energy_array[spot] = add + place;
                }
                else
                {   
                    int place = local_energy_array[spot-(width+1)]; 
                    if(place <= local_energy_array[spot-(width-1)])
                    {
                        int add = local_energy_array[spot];
                        local_energy_array[spot] = add + place;
                    }
                    else
                    {
                        int place = local_energy_array[spot-(width-1)];
                        int add = local_energy_array[spot];
                        local_energy_array[spot] = add + place;
                    }
                }
            }
            else
            {
                check2++;
                if(check2 != width)
                {
                    
                    int place = local_energy_array[spot-width];
                    if(place <= local_energy_array[spot-(width-1)])
                    {
                        int add = local_energy_array[spot];
                        local_energy_array[spot] = add + place;
                    }
                    else
                    {   
                        int place = local_energy_array[spot-(width-1)];
                        int add = local_energy_array[spot];
                        local_energy_array[spot] = add + place;
                    }
                }
                else
                {
                    
                    int place = local_energy_array[spot-width];
                    if(place <= local_energy_array[spot-width+1])
                    {
                        int add = local_energy_array[spot];
                        local_energy_array[spot] = add + place;
                    }
                    else
                    {      
                        int place = local_energy_array[spot-(width+1)];
                        int add = local_energy_array[spot];
                        local_energy_array[spot] = add + place;
                    }
                }
            }
        }
        spot++;
    }
}

int* optimal_path(int* ae_array, int width, int height)
{
    //first element of path_array is the x-coord of the minimal element of the last row of ae_array
    //in other words: bottom first
    int* path_array = malloc(height*sizeof(int));
    int arraysize = width * height;
    int min = ae_array[arraysize-1];
    int endIndex = arraysize-1;
    int x = 0;
    int spot = 0;
    
    for(int i = 0; i < width; i++)
    {
        if (min >= ae_array[endIndex])
        {
            min = ae_array[endIndex];
            x = endIndex % width;
            spot = endIndex;
        }
        endIndex--;
    }
    path_array[0] = x;
    
    //int check = 1;
    for(int i=1; i < height; i++)
    {
        if(x % width == 0)
        {
            //check++;
            int above = ae_array[spot-width];
            if(above <= ae_array[spot-(width-1)])
            {
                spot = spot - width;
                x = spot % width;
                path_array[i] = x;
            }
            else
            {
                spot = spot-(width-1);
                x = spot % width;
                path_array[i] = x;
            }
        }
        else 
        {
            if(x % width == width-1)
            {
                //check++;
                int above = ae_array[spot-width];
                if(above <= ae_array[spot-(width+1)])
                {
                    spot = spot - width;
                    x = spot % width;
                    path_array[i] = x;
                }
                else
                {
                    spot = spot - (width+1);
                    x = spot % width;
                    path_array[i] = x;
                }
            }
            else
            {
                //check++;
                int above = ae_array[spot-width];
                if(above <= ae_array[spot-(width-1)] && above <= ae_array[spot-(width+1)])
                {
                    spot = spot - width;
                    x = spot % width;
                    path_array[i] = x;
                }
                else 
                {
                    int left = ae_array[spot-(width+1)];
                    if(left <= ae_array[spot-(width-1)])
                    {
                        spot = spot - width+1;
                        x = spot % width;
                        path_array[i] = x;
                    }
                    else
                    {
                        spot = spot-(width-1);
                        x = spot % width;
                        path_array[i] = x;
                    }
                }
            }
        }
    }
    return path_array;
}
