#ifndef FBUF_H
#define FBUF_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static inline
void printfbuf(char** fbuf, int tgeom[2]){
    printf("\x1b[H");
    for(int i=0; i<tgeom[0]; i++){
        printf("%s\n", fbuf[i]);
    }
}

static inline
void clearfbuf(char** fbuf, int tgeom[2]){
    for(int i=0; i<tgeom[0]; i++){
        memset(fbuf[i], ' ', (tgeom[1]) * sizeof(char));
        /* fbuf[i][0] = '|'; */
        /* fbuf[i][tgeom[1]-1] = '|'; */
    }
    /* memset(fbuf[tgeom[0]-1], '-', (tgeom[1]-1) * sizeof(char)); */
    /* memset(fbuf[0], '-', (tgeom[1]-1) * sizeof(char)); */
}

static inline
char** createfbuf(int tgeom[2]){
    char** fbuf = (char**) malloc(tgeom[0] * sizeof(char*));
    for(int i=0; i<tgeom[0]; i++){
        fbuf[i] = (char*) malloc((tgeom[1]+1) * sizeof(char));
        fbuf[i][tgeom[1]] = '\0';
    }
    return fbuf;
}

static inline
void destroyfbuf(char** fbuf, int tgeom[2]){
    for(int i=0; i<tgeom[0]; i++){
        free(fbuf[i]);
    }
    free(fbuf);
}

static inline
float* createzbuf(int tgeom[2]){
    return  (float*) malloc(tgeom[0] * tgeom[1] * sizeof(float));
}

static inline
void clearzbuf(float* zbuf, int tgeom[2]){
    memset(zbuf, 0, tgeom[1] * tgeom[0] * sizeof(float));
}

static inline
void destroyzbuf(float* zbuf){
    free(zbuf);
}

#endif
