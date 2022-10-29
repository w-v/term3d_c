// generate FNL implementation
#define FNL_IMPL

#include <sys/ioctl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "FastNoiseLite.h"
#include "tgeom.h"


int main(){
    // Create and configure noise state
    fnl_state noise = fnlCreateState();
    noise.frequency = 0.04;
    noise.noise_type = FNL_NOISE_OPENSIMPLEX2;

    // Gather noise data
    int geom[2];
    termgeom(geom);
    geom[0]--;
    char* noiseData = malloc(geom[0] * (geom[1]+1) * sizeof(char));
    int index, z = 0;

    char * greyscale = " .,-~:;=!#$@";
    int scalen = strlen(greyscale)/2;
    int ms = 40;
    struct timespec ts = { .tv_sec = 0, .tv_nsec = ms * 1000000};

    while(z < 500){
        index = 0;
        z++;
        for (int y = 0; y < geom[0]; y++)
        {
            for (int x = 0; x < geom[1]; x++) 
            {
                noiseData[index++] = greyscale[(int)((fnlGetNoise3D(&noise, x, y, z)+1)*scalen)];
            }
            noiseData[index++] = '\n';
        }
        printf("\x1b[H");
        printf("%s", noiseData);
        nanosleep(&ts, NULL);
    }




    // Free data later
    free(noiseData);
}
