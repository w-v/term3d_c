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

int clip(int i, int l, int h){
    i = i < l ? l : i;
    i = i > h ? h : i;
    return i;
}

void terrain_normals(geom* g, int len, int height){
    vec3 h = {1.f, 0.f, 0.f} , l = {0.f, 0.f, 1.f};
    int id = 0;
    int hi[2] = {0, 1}, li[2] = {0, 1};
    for(int i=0; i<height; i++){
        if(i < 2){
            hi[0] = 0;
        }
        if(i == height - 1){
            hi[1] = id;
        }
        for(int j=0; j<len; j++){
            if(j < 2){
                li[0] = i * len;
            }
            if(j == len - 1){
                li[1] = id;
            }
            h[1] = g->pts[hi[1]++][1] - g->pts[hi[0]++][1];
            l[1] = g->pts[li[1]++][1] - g->pts[li[0]++][1];
            glm_vec3_cross(h, l, g->norm[id]);
            glm_vec3_normalize(g->norm[id++]);
        }
        li[1]++;
    }
}

void genterrain(geom* g, int len, int height, float scale){

    // Create and configure noise state

    assert(g->npts == len*height);

    float a = 5, b = 0.1, c = 0.1, d = 0.1;

    int index = 0;
    for(int i=0; i<height; i++){
        for(int j=0; j<len; j++){
            /* memcpy(g->pts[index], ((vec3){i, 0.f, j}), sizeof(vec3)); */
            g->pts[index][0] = (float)i;
            g->pts[index][1] = a * sin(i * b + d) * cos(j * b + c);
            g->pts[index][2] = (float)j;

            index++;
        }
    }
    terrain_normals(g, len, height);
    transform(g, g, NULL, (vec3){-len/2.f,0.f,-len/2.f}, NULL);
    transform(g, g, NULL, NULL, (vec3){scale,scale,scale});

}



static volatile int run = 1;

void run_handler(int _){
    run = 0;
}

int main(){

    signal(SIGINT, run_handler);

    // hide cursor
    /* printf("\x1b[?25l"); */
    
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
    int len = 40*4;

    struct geom* g = creategeom(len*len);
    struct geom* tg = creategeom(len*len);
    genterrain(g, len, len, 0.25f);

    vec3 rot = {-90,0,0};
    vec3 tr = {0,0,20};

    /* transform(g, g, rot, tr, NULL); */
    float r = 0.1f;
    rargs.lightdir[0] = 0.f;
    rargs.lightdir[1] = 0.f;
    rargs.lightdir[2] = 1.f;
    /* rargs.K = 20; */

    glm_vec3_normalize(rargs.lightdir);


    while(run){
    
        clearfbuf(fbuf, tgeom);
        clearzbuf(zbuf, tgeom);

        transform(g, tg, rot, tr, NULL);
        render(tg, fbuf, zbuf, tgeom, &rargs);

        /* glm_vec3_rotate(rargs.lightdir, r, (vec3){0.f, 1.f, 0.f}); */

        rot[2] += r;

        printfbuf(fbuf, tgeom);

    }

    destroygeom(g);
    destroygeom(tg);
    destroyfbuf(fbuf, tgeom);
    destroyzbuf(zbuf);
    
    // show cursor
    /* printf("\x1b[?25h"); */
}
