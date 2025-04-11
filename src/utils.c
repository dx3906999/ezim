#include <stdlib.h>
#include <stdint.h>
#include "utils.h"

Vector* newVector(size_t len,size_t type_size){
    Vector* vector=(Vector*)malloc(sizeof(Vector));
    if (!vector)
    {
        return NULL;
    }
    vector->len=len;
    vector->capacity=(INIT_CAPACITY>len)?INIT_CAPACITY:len;
    vector->type_size=type_size;
    vector->data=calloc(vector->capacity,type_size);
    if (!vector->data)
    {
        free(vector);
        return NULL;
    }
    
    return vector;
    
}


void freeVector(Vector* vector){
    free(vector->data);
    free(vector);
}

void* getFromVector(Vector* vector,size_t index){
    if (index>=vector->capacity)
    {
        vector->capacity=(vector->capacity*2>(index+1))?(vector->capacity*2):(index+1);
        vector->data=realloc(vector->data,(vector->type_size)*(vector->capacity));
        //* actually need checking
    }
    
    return (char*)(vector->data)+index*(vector->type_size);
}

