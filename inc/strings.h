#ifndef STRINGS_H
#define STRINGS_H

#define CHUNKSIZE 80

typedef struct StrChunk {
    char str[CHUNKSIZE];// [CHUNKSIZE-1] is reserved
    struct StrChunk* bk;
    long len;
}StrChunk;

typedef struct String {
    StrChunk* head;
    StrChunk* tail;
    long len;
    long chunk_num;
}String;

String* newString();
void freeString(String* s);
StrChunk* addStrChunk(String* s);
void appendString(String* dst, char* src, long len);

#endif