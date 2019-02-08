/* Balanced interleaving through multiple paths
 * Authors: Douglas Canevarollo, Douglas Brandão and Gabriel Andrey
 * Date: 22/01/2019 */

#ifndef BITMP_H
#define BITMP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#define tape01 1
#define tape02 2
#define tape03 3

/* Representation struct of the record to be sorted on the external file.
 * Each record have a key that is used to the sorting. */
typedef struct _record {
    int key;
} record;


/* Representation struct of the system's internal memory.
 * The size is dynamic and defined by the user. */
typedef struct _memory {
    int size;  // Number of records available in memory.
    record *rec;  // Record array.
} memory;

/* Main sort function. */
void sort(FILE *clutteredFile, int memorySize);

#endif
