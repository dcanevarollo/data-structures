#ifndef EXT_HASH_H
#define EXT_HASH_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#define bool char
#define true 1
#define false 0

typedef struct _record {
    int recordId;
    int accountNumber;
} record;

typedef struct _bucket {
    int id;
    int localDepth;
    record *rec;
} bucket;

typedef struct directory {
    char *keys;
    bucket *bkt;
} directory;

void insert(int recordId, int accountNumber);

void purge(int accountNumber);

void printHash();

#endif
