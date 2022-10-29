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

void terrain_normals_ln(geom* g, int len, int height, int i){
    vec3 h = {0.f, 0.f, 1.f} , l = {1.f, 0.f, 0.f};
    int id = i*len;
    int hi[2] = {id-len, id+len}, li[2] = {id-1, id+1};
    if(i < 2){
        hi[0] = 0;
    }
    if(i == height - 1){
        hi[1] = id;
    }
    for(int j=0; j<len; j++){
        if(j < 2){
            li[0] = i*len;
        }
        if(j == len - 1){
            li[1] = id;
        }
        h[1] = g->pts[hi[1]++][1] - g->pts[hi[0]++][1];
        l[1] = g->pts[li[1]++][1] - g->pts[li[0]++][1];
        glm_vec3_cross(l, h, g->norm[id]);
        glm_vec3_normalize(g->norm[id++]);
    }
}

void genterrain_ln(geom* g, int len, int height, int x, float scale){

    // Create and configure noise state
    fnl_state noise = fnlCreateState();
    noise.frequency = 0.05;
    noise.noise_type = FNL_NOISE_OPENSIMPLEX2;

    assert(g->npts == len*height);

    int i = (x%height);
    int index = i * len;
    float off = -len/2.f;
    for(int j=0; j<len; j++){
        /* memcpy(g->pts[index], ((vec3){i, 0.f, j}), sizeof(vec3)); */
        g->pts[index][0] = ((float)j+off)*scale;
        g->pts[index][1] = (fnlGetNoise2D(&noise, x*scale, j*scale)+1)*5;
        g->pts[index][2] = ((float)i+off)*scale;

        index++;
    }
    /* transform(g, g, NULL, (vec3){-len/2.f,0.f,-len/2.f}, (vec3){scale,scale,scale}); */

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
    cleargeom(g);
    struct geom* tg = creategeom(len*len);

    vec3 rot = {0,0,0};
    vec3 tr = {0,-20,len/2/4};
    float scale = 1.f/4;

    rargs.lightdir[0] = -1.f;
    rargs.lightdir[1] = -0.3f;
    rargs.lightdir[2] = 0.f;

    glm_vec3_normalize(rargs.lightdir);

    int ms = 40/2;
    struct timespec ts = { .tv_sec = 0, .tv_nsec = ms * 1000000};

    int i = 0;

    while(run){
    
        clearfbuf(fbuf, tgeom);
        clearzbuf(zbuf, tgeom);

        genterrain_ln(g, len, len, i++, scale);
        genterrain_ln(g, len, len, i, scale);
        terrain_normals_ln(g, len, len, (i-1)%len);

        transform(g, tg, rot, tr, NULL);

        render(tg, fbuf, zbuf, tgeom, &rargs);

        printfbuf(fbuf, tgeom);
        
        i++;
        /* rot[1] += 0.1; */
        nanosleep(&ts, NULL);
    }

    destroygeom(g);
    destroygeom(tg);
    destroyfbuf(fbuf, tgeom);
    destroyzbuf(zbuf);
    
    // show cursor
    /* printf("\x1b[?25h"); */
}
