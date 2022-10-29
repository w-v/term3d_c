#ifndef TGEOM_H
#define TGEOM_H

#include <sys/ioctl.h>
#include <unistd.h>

static inline
void termgeom( int geom[2]){
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    geom[0] = w.ws_row;
    geom[1] = w.ws_col;
}


#endif

