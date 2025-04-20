#ifndef UTILS_H
#define UTILS_H
#include <stdlib.h>


#define INIT_CAPACITY 1024

#define GETV(vector,type,index) (*((type*)getFromVector((vector),(index))))
#define PUSHBACKV(vector,type,data) {GETV(vector,type,(vector)->len)=(type)(data);(vector)->len++;}
#define POPV(vector) {if ((vector)->len>0){(vector)->len--;}}

typedef struct Vector{
    void* data;
    long len;
    size_t type_size;
    size_t capacity;
}Vector;

Vector* newVector(long len,size_t type_size);
Vector* newVectorCapa(long len,size_t type_size,size_t capacity);
void freeVector(Vector* vector);
void* getFromVector(Vector* vector,size_t index);
void resizeVector(Vector* vector,size_t len);

#endif