#ifndef STATUS_HEADER
#define STATUS_HEADER

typedef struct status{
    int status: 1;
    char *message;
} Status;

#endif