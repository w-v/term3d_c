
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


    char * greyscale = " .,-~:;=!#$@";
    int scalen = strlen(greyscale)/2;

    // Gather noise data
    int geom[2];
    termgeom(geom);
    geom[0]--;  // new prompt will eat 1 line
                //
    int fsize = geom[0] * (geom[1]+1);
    size_t fbufsize = fsize * sizeof(char);
    char* noiseData = malloc(fbufsize);
    memset(noiseData, greyscale[0], fbufsize);
    unsigned int index = 0, nindex = 0;

    int ms = 40;
    struct timespec ts = { .tv_sec = 0, .tv_nsec = ms * 1000000};

    int z = 0, y = 0;
    while(z < 500){
        for (int x = 0; x < geom[1]; x++) 
        {
            noiseData[index++] = greyscale[(int)((fnlGetNoise2D(&noise, x, y)+1)*scalen)];
        }
        z++;
        y++;
        noiseData[index] = '\0';
        nindex = (index+1)%fsize;
        printf("\x1b[H");
        printf("%s\n", noiseData+nindex);
        if(nindex != 0){
            printf("%s", noiseData);
        }
        noiseData[index] = '\n';
        index = nindex;
        nanosleep(&ts, NULL);
    }




    // Free data later
    free(noiseData);
}
