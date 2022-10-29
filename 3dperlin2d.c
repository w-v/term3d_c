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
    vec3 h = {0.f, 0.f, 1.f} , l = {1.f, 0.f, 0.f};
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
                li[0] = id;
            }
            if(j == len - 1){
                li[1] = id;
            }
            h[1] = g->pts[hi[1]++][1] - g->pts[hi[0]++][1];
            l[1] = g->pts[li[1]++][1] - g->pts[li[0]++][1];
            glm_vec3_cross(l, h, g->norm[id]);
            glm_vec3_normalize(g->norm[id++]);
        }
        li[1]++;
    }
}

void genterrain(geom* g, int len, int height){

    // Create and configure noise state
    fnl_state noise = fnlCreateState();
    noise.frequency = 0.005;
    noise.noise_type = FNL_NOISE_OPENSIMPLEX2;

    assert(g->npts == len*height);

    float scale = 1.f;

    int index = 0;
    for(int i=0; i<height; i++){
        for(int j=0; j<len; j++){
            /* memcpy(g->pts[index], ((vec3){i, 0.f, j}), sizeof(vec3)); */
            g->pts[index][0] = (float)i;
            g->pts[index][1] = (fnlGetNoise2D(&noise, i, j)+1)*20;
            g->pts[index][2] = (float)j;

            index++;
        }
    }
    terrain_normals(g, len, height);
    transform(g, g, NULL, (vec3){-len/2.f,0.f,-len/2.f}, (vec3){scale,scale,scale});

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
    int len = 40*16;

    struct geom* g = creategeom(len*len);
    genterrain(g, len, len);

    vec3 rot = {-40,0,0};
    vec3 tr = {0,80,len/2};

    transform(g, g, rot, tr, NULL);
    float r = 0.001f;
    rargs.lightdir[0] = -1.f;
    rargs.lightdir[1] = -1.f;
    rargs.lightdir[2] = -1.f;
    rargs.K = 20;

    glm_vec3_normalize(rargs.lightdir);


    while(run){
    

        clearfbuf(fbuf, tgeom);
        clearzbuf(zbuf, tgeom);

        render(g, fbuf, zbuf, tgeom, &rargs);
        /* transform(g, g, NULL, (vec3){0,0,r}, NULL); */

        glm_vec3_rotate(rargs.lightdir, r, (vec3){0.f, 1.f, 0.f});

        printfbuf(fbuf, tgeom);

    }

    destroygeom(g);
    destroyfbuf(fbuf, tgeom);
    destroyzbuf(zbuf);
    
    // show cursor
    /* printf("\x1b[?25h"); */
}
