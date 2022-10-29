#ifndef GEOM_H
#define GEOM_H

#include "cglm/cglm.h"
#include <string.h>

typedef struct geom {
    vec3* pts; 
    vec3* norm;
    int npts;
} geom;

static inline 
struct geom* creategeom(int npts){
    struct geom* g = (struct geom*) malloc(sizeof(geom));
    g->pts = (vec3*) malloc(npts * sizeof(vec3));
    g->norm = (vec3*) malloc(npts * sizeof(vec3));
    g->npts = npts;
    return g;
}

static inline 
void destroygeom(geom* g){
    free(g->pts);
    free(g->norm);
}

static inline
void cleargeom(geom* g){
    memset(g->pts, 0.f, g->npts * sizeof(vec3));
    memset(g->norm, 0.f, g->npts * sizeof(vec3));
}


static inline
void transform(geom* sg, geom* tg, vec3 eulerdeg, vec3 tr, vec3 sca){
    mat4 rot;

    if(eulerdeg != NULL){
        vec3 eulerrad;
        for(int i = 0; i<3; i++){
            eulerrad[i] = eulerdeg[i] * M_PI / 180.f;
        }

        glm_euler(eulerrad, rot);
    }
    
    for(int i = 0; i<sg->npts; i++){
        if(sca != NULL){
            glm_vec3_mul(sg->pts[i], sca, tg->pts[i]);
        }
        if(eulerdeg != NULL){
            glm_mat4_mulv3(rot, sg->pts[i], 1, tg->pts[i]);
            glm_mat4_mulv3(rot, sg->norm[i], 1, tg->norm[i]);
        }
        if(tr != NULL){
            glm_vec3_add(tg->pts[i], tr, tg->pts[i]);
        }
    }
}

#endif


