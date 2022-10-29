// generate FNL implementation
#define FNL_IMPL
#define M_PI 3.14159265358979323846

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <assert.h>
#include "FastNoiseLite.h"
#include "tgeom.h"
#include "geom.h"
#include "fbuf.h"
#include "render.h"
#include "cglm/cglm.h"

void genplane(geom* g, int len, int height){
    assert(g->npts == len*height);

    vec3 norm = {.0f, 1.f, .0f};

    int index = 0;
    for(int i=0; i<height; i++){
        for(int j=0; j<len; j++){
            /* memcpy(g->pts[index], ((vec3){i, 0.f, j}), sizeof(vec3)); */
            g->pts[index][0] = (float)i/len;
            g->pts[index][1] = (float)j/height;
            g->pts[index][2] = 0.f;

            memcpy(g->norm[index], norm, sizeof(norm));

            index++;
        }
    }
    transform(g, g, NULL, (vec3){-0.5,-0.5,0.f}, NULL);

}


static volatile int run = 1;

void run_handler(int _){
    run = 0;
}

int main(){

    signal(SIGINT, run_handler);

    // hide cursor
    printf("\x1b[?25l");
    
    // terminal geometry
    int tgeom[2];
    termgeom(tgeom);
    tgeom[0]--;
    
    // frame buffer
    // geom[1]+1 because of extra \n at each line end
    char** fbuf = createfbuf(tgeom);

    // z buffer
    float * zbuf = createzbuf(tgeom);

    renderargs rargs = {};

    default_rargs(&rargs);
    int len = 40;

    struct geom* g = creategeom(len*len);
    struct geom* tg = creategeom(len*len);
    genplane(g, len, len);
    vec3 rot = {0,0,0};
    vec3 tr = {0,0,2};
    while(run){
        transform(g, tg, rot, tr, NULL);
        

        clearfbuf(fbuf, tgeom);
        clearzbuf(zbuf, tgeom);

        render(tg, fbuf, zbuf, tgeom, &rargs);

        transform(g, tg, (vec3){0,rot[1],rot[0]}, (vec3){0,0,2}, NULL);

        render(tg, fbuf, zbuf, tgeom, &rargs);

        printfbuf(fbuf, tgeom);

        rot[0]+=0.02;
        rot[1]+=0.02;
    }

    destroygeom(g);
    destroygeom(tg);
    destroyfbuf(fbuf, tgeom);
    destroyzbuf(zbuf);
    
    // show cursor
    printf("\x1b[?25h");
}


