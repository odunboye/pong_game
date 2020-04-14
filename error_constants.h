#ifndef ERROR_CONSTANTS_H
#define ERROR_CONSTANTS_H

typedef const char *(*ErrorFn)(void);

extern ErrorFn errorFn;

#endif
