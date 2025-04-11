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

StrChunk* addStrChunk(String* s){
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

void appendString(String* dst, char* src, long len){
    if (dst->chunk_num==0)
    {
        addStrChunk(dst);
    }

    while (dst->tail->len+len>CHUNKSIZE)
    {
        memcpy(dst->tail->str+dst->tail->len,src,CHUNKSIZE-dst->tail->len);
        len-=CHUNKSIZE-dst->tail->len;
        src+=CHUNKSIZE-dst->tail->len;
        dst->len+=CHUNKSIZE-dst->tail->len;
        dst->tail->len=CHUNKSIZE;
        addStrChunk(dst);
    }
    
    if (len!=0)
    {
        memcpy(dst->tail->str+dst->tail->len,src,len);
        dst->len+=len;
        dst->tail->len+=len;
    }
    
}
