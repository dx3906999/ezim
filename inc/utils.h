#ifndef UTILS_H
#define UTILS_H
#include <stdlib.h>


#define INIT_CAPACITY 1024

#define GETV(vector,type,index) (*((type*)getFromVector((vector),(index))))

typedef struct Vector{
    void* data;
    size_t len;
    size_t type_size;
    size_t capacity;
}Vector;

Vector* newVector(size_t len,size_t type_size);
void freeVector(Vector* vector);
void* getFromVector(Vector* vector,size_t index);


#endif