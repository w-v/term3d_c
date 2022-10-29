#ifndef RENDER_H
#define RENDER_H

#include <math.h>
#include <string.h>
#include "geom.h"
#include "cglm/cglm.h"

typedef struct renderargs {
    char* greyscale;
    vec3 lightdir;
    float K;
} renderargs;

static inline
void default_rargs(renderargs* rargs){
    rargs->greyscale = " .,-~:;+=#$@";
    vec3 ld = {-0.6, -1, 1};
    glm_vec3_normalize(ld);
    memcpy(rargs->lightdir, ld, sizeof(ld));
    rargs->K = 50;
}

static inline
void render(geom* g, char** fbuf, float* zbuf, int tgeom[2], renderargs* rargs){
    int mid[2] = {tgeom[0]/2, tgeom[1]/2};
    ivec2 pt;
    float L;
    int scalen = strlen(rargs->greyscale);
    int zptr;
    for(int i = 0; i<g->npts; i++){
        float oz = ((float)1)/g->pts[i][2];
        pt[0] = mid[0] - rargs->K * g->pts[i][1] * oz * 0.55;
        pt[1] = mid[1] + rargs->K * g->pts[i][0] * oz;
        
        // clip
        if(pt[0] < tgeom[0] && pt[1] < tgeom[1] && pt[0] > 0 && pt[1] > 0){
            zptr = pt[0] * tgeom[1] + pt[1];
            /* L = glm_vec3_dot(g->norm[i], rargs->lightdir); */
            /* if(L > 0){ */
            /*     if(oz > zbuf[zptr]){ */
            /*         // z buffer check */
            /*         zbuf[zptr] = oz; */
            /*         fbuf[pt[0]][pt[1]] = rargs->greyscale[(int) floor(L*scalen)]; */
            /*     } */
            /* } */

            if(oz > zbuf[zptr]){
                // z buffer check
                zbuf[zptr] = oz;
                L = glm_vec3_dot(g->norm[i], rargs->lightdir);
                L = L < 0 ? 0 : L;
                // luminance
                fbuf[pt[0]][pt[1]] = rargs->greyscale[(int) floor(L*scalen)];
            }
        }
    }
}

#endif
