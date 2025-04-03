#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strings.h"

String* newString(){
    String* s = (String*)malloc(sizeof(String));
    if (!s)
    {
        return NULL;
    }

    s->head=NULL;
    s->tail=NULL;
    s->len=0;
    s->chunk_num=0;
    return s;
}

void freeString(String* s){
    StrChunk* temp=NULL;
    while (s->head!=NULL)
    {
        temp=s->head;
        s->head=s->head->bk;
        free(temp);
    }
    free(s);
}

String* addStrChunk(String* s){
    StrChunk* sc = (StrChunk*)malloc(sizeof(StrChunk));
    if (!sc)
    {
        return NULL;
    }

    if (s->tail)
    {
        s->tail->bk=sc;
    }
    else
    {
        s->head=sc;
        s->tail=sc;
    }
    
    
    sc->bk=NULL;
    sc->len=0;
    s->tail=sc;
    s->chunk_num++;
    return sc;
}

void clearString(String* s){
    StrChunk* sc=s->head;
    while (sc)
    {
        sc->len=0;
        sc=sc->bk;
    }
    s->len=0;
    
}

void appendString(String* dst, char* src, long len){
    if (dst->chunk_num==0)
    {
        addStrChunk(dst);
    }

    while (dst->tail->len+len>CHUNKSIZE-1)
    {
        memcpy(dst->tail->str+dst->tail->len,src,CHUNKSIZE-dst->tail->len-1);
        len-=CHUNKSIZE-dst->tail->len-1;
        src+=CHUNKSIZE-dst->tail->len-1;
        dst->len+=CHUNKSIZE-dst->tail->len-1;
        dst->tail->len=CHUNKSIZE-1;
        addStrChunk(dst);
    }
    
    if (len!=0)
    {
        memcpy(dst->tail->str+dst->tail->len,src,len);
        dst->len+=len;
        dst->tail->len+=len;
    }
    
}
