#ifndef IMAGE_H
#define IMAGE_H

#include <stdio.h>


typedef struct image
{
	int w, h;
	int* data;
} image_t;
 
image_t* image_init(FILE* f);
void image_destroy(image_t* img);
void image_write_to_file(image_t* img);
image_t* resize_picture(image_t* img, int cc);

#endif
