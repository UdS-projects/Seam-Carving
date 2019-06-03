 #ifndef CALCULATE_H
#define CALCULATE_H

#include <stdio.h>

int calculateBrightness(image_t* img);

int* local_energy(image_t* img);

void accumulated_energy(int* local_energy_array, int width, int arraysize);

int* optimal_path(int* ae_array, int width, int height);

#endif

 
