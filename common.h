#ifndef COMMON_H
#define COMMON_H

typedef enum {
    STATUS_FAILURE = 0,   // operation or function failed
    STATUS_SUCCESS = 1    // operation or function completed correctly
} Status;

typedef enum {
    OP_NONE,              // no proper option given by user
    OP_V,                 // -v : view mp3 tag info
    OP_E,                 // -e : edit mp3 tag info
    OP_H                  // -h / --help : print help menu
} Op;

#endif // COMMON_H
