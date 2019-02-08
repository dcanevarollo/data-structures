/* Balanced interleaving through multiple paths
 * Authors: Douglas Canevarollo, Douglas Brandão and Gabriel Andrey
 * Date: 22/01/2019 */

#include "bitmp.h"

/* Global variables. */
memory *mem = NULL;  // Memory pointer for the internal sort.
FILE **tapes = NULL;  // Tapes array available to the sorting.
int recursions = 0;  // Recursion counter.
int complexity = 0;  // Number of needed recursions for sorting.

/* Sort method for the internal memory. */
void insertionSort(record *rec, int size) {
    int i = 0, j = 0, key = 0;

    for (i = 1; i < size; i++) {
        key = rec[i].key;
        j = i-1;

        while (j >= 0 && rec[j].key > key) {
            rec[j+1].key = rec[j].key;
            j--;
        }

        rec[j+1].key = key;
    }
}

/* Returns the complexity of the sorting.
 * In C we don't have a base 3 log function, so we have to
 * create help functions to change log base.
 * We need base 3 cause it corresponds to (tapes available/2). */
int getComplexity(int memorySize) {
    return (int) ceil(log10(1000/memorySize)/log10(3));
}

/* Opens the tapes in the interval [start, final] with the op file mode. */
void openTapes(int start, int final, char *op) {
    int i = 0;

    for (i = start; i < final; i++) {
        if (tapes[i] != NULL)  // Close the file if it is already open.
            fclose(tapes[i]);

        char fileName[10] = "tape", extension[6] = "";

        strcat(fileName, strcat(itoa(i+1, extension, 10), ".txt"));

        tapes[i] = fopen(fileName, op);
        if (tapes[i] == NULL) {
            printf("Error creating the tape %d: %s\n", i+1, strerror(errno));

            exit(EXIT_FAILURE);
        }
    }
}

/* Function that reverses the read and write tapes. For example,
 * if tapes 1, 2 and 3 were opened for reading, here they
 * will be reopened as written. The same applies to tapes 4, 5 and 6. */
void exchangeTapes() {
    /* If the passage is even the first 3 tapes are opened as
     * reading and the other 3 as writing. Analogously for odd passing. */
    if (recursions%2 == 0) {
        openTapes(0, 3, "r");
        openTapes(3, 6, "w");
    } else {
        openTapes(0, 3, "w");
        openTapes(3, 6, "r");
    }
}

/* Writes the block contained in memory to the correct tape.
 * This function returns an indicator of the next tape that should
 * be filled in the next pass. */
int writeBlock(int tapeIndex, int numRecords) {
    FILE *chosenTape = NULL;
    int i = 0, nextTape = 0;

    if (tapeIndex == tape01) {
        chosenTape = tapes[0];
        nextTape = tape02;
    } else if (tapeIndex == tape02) {
        chosenTape = tapes[1];
        nextTape = tape03;
    } else {
        chosenTape = tapes[2];
        nextTape = tape01;
    }

    for (i = 0; i < numRecords; i++)
        fprintf(chosenTape, "%d ", mem->rec[i].key);

    return nextTape;
}

/* Write the key on the corresponding tape.
 * This function returns an integer to count
 * how many keys were inserted in the block */
int writeKey(FILE *tape1, FILE *tape2, FILE *tape3, int tapeIndex, int key) {
    if (tapeIndex == tape01)
        fprintf(tape1, "%d ", key);
    else if (tapeIndex == tape02)
        fprintf(tape2, "%d ", key);
    else
        fprintf(tape3, "%d ", key);

    return 1;
}

/* Interleaves the blocks inserted in the tape.
 * It receives as parameters the reading tapes (readTi)
 * and writing tapes (writeTi). */
void merge(FILE *readT1, FILE *readT2, FILE *readT3, FILE *writeT1, FILE *writeT2, FILE *writeT3, int blockSize) {
    int i = 0, j = 0, k = 0, tape = 0, keyCount = 0, temp = 0;

    recursions++;  // Increments the recursion counter at each call of this function.

    if (fscanf(readT1, "%d ", &mem->rec[0].key) == EOF)
        i = blockSize;

    if (fscanf(readT2, "%d ", &mem->rec[1].key) == EOF)
        j = blockSize;

    if (fscanf(readT3, "%d ", &mem->rec[2].key) == EOF)
        k = blockSize;

    tape = tape01;
    while (tape <= tape03) {
        keyCount = 0;

        /* The indexes i, j and k will control the records of the blocks,
         * that is, with each reading and insertion in the tapes the corresponding index is increased.
         * When some "bursts" (reaches the size of the memory / block),
         * it is necessary to disregard the block in future interleavings;
         * This is done by setting -1 to the correponding tape memory position. */
        if (mem->rec[0].key == -1)
            i = blockSize;

        if (mem->rec[1].key == -1)
            j = blockSize;

        if (mem->rec[2].key == -1)
            k = blockSize;

        /* If none of the tapes have keys, then it does not make sense to read them. Like this,
         * decrement the tape indicated (to use as a stop condition
         * of the recursion) and interrupt the current loop. */
        if (i == blockSize && j == blockSize && k == blockSize)
            break;

        /* If the block does not contain records it is automatically ignored,
         * so that if this occurs its index (i, j or k) will not be zeroed
         * and will start the loop by using the block size. */

        /* Inserts the smallest key contained in the memory in writing tape 1, 2, or 3
         * and increments the tape index in which the block was read,
         * as is done in the Merge Sort algorithm. */
        while (i < blockSize && j < blockSize && k < blockSize) {
            if (mem->rec[0].key <= mem->rec[1].key && mem->rec[0].key <= mem->rec[2].key) {
                keyCount += writeKey(writeT1, writeT2, writeT3, tape, mem->rec[0].key);

                if (fscanf(readT1, "%d ", &mem->rec[0].key) == EOF) {
                    mem->rec[0].key = -1;
                    i = blockSize;
                    break;
                }

                i++;
            } else if (mem->rec[1].key <= mem->rec[0].key && mem->rec[1].key <= mem->rec[2].key) {
                keyCount += writeKey(writeT1, writeT2, writeT3, tape, mem->rec[1].key);

                if (fscanf(readT2, "%d ", &mem->rec[1].key) == EOF) {
                    mem->rec[1].key = -1;
                    j = blockSize;
                    break;
                }

                j++;
            } else if (mem->rec[2].key <= mem->rec[0].key && mem->rec[2].key <= mem->rec[1].key) {
                keyCount += writeKey(writeT1, writeT2, writeT3, tape, mem->rec[2].key);

                if (fscanf(readT3, "%d ", &mem->rec[2].key) == EOF) {
                    mem->rec[2].key = -1;
                    k = blockSize;
                    break;
                }

                k++;
            }
        }

        /* Ignores in the interleaving the tape referring to the block that was read in full,
         * for example, if the first block (tape 1) is completely read and passed
         * for writing tape 1, 2 or 3 the index i will be worth the block size,
         * so we need to disregard it in the next intersperses. */
        while (j < blockSize && k < blockSize) {
            if (mem->rec[1].key <= mem->rec[2].key) {
                keyCount += writeKey(writeT1, writeT2, writeT3, tape, mem->rec[1].key);

                if (fscanf(readT2, "%d ", &mem->rec[1].key) == EOF) {
                    mem->rec[1].key = -1;
                    j = blockSize;
                    break;
                }

                j++;
            } else {
                keyCount += writeKey(writeT1, writeT2, writeT3, tape, mem->rec[2].key);

                if (fscanf(readT3, "%d ", &mem->rec[2].key) == EOF) {
                    mem->rec[2].key = -1;
                    k = blockSize;
                    break;
                }

                k++;
            }
        }

        while (i < blockSize && k < blockSize) {
            if (mem->rec[0].key <= mem->rec[2].key) {
                keyCount += writeKey(writeT1, writeT2, writeT3, tape, mem->rec[0].key);

                if (fscanf(readT1, "%d ", &mem->rec[0].key) == EOF) {
                    mem->rec[0].key = -1;
                    i = blockSize;
                    break;
                }

                i++;
            } else {
                keyCount += writeKey(writeT1, writeT2, writeT3, tape, mem->rec[2].key);

                if (fscanf(readT3, "%d ", &mem->rec[2].key) == EOF) {
                    mem->rec[2].key = -1;
                    k = blockSize;
                    break;
                }

                k++;
            }
        }

        while (i < blockSize && j < blockSize) {
            if (mem->rec[0].key <= mem->rec[1].key) {
                keyCount += writeKey(writeT1, writeT2, writeT3, tape, mem->rec[0].key);

                if (fscanf(readT1, "%d ", &mem->rec[0].key) == EOF) {
                    mem->rec[0].key = -1;
                    i = blockSize;
                    break;
                }

                i++;
            } else {
                keyCount += writeKey(writeT1, writeT2, writeT3, tape, mem->rec[1].key);

                if (fscanf(readT2, "%d ", &mem->rec[1].key) == EOF) {
                    mem->rec[1].key = -1;
                    j = blockSize;
                    break;
                }

                j++;
            }
        }

        /* Finally, in case there is some block of some tape left exclusively
         * the same is inserted in the current tape of the loop. */
        while (i++ < blockSize) {
            keyCount += writeKey(writeT1, writeT2, writeT3, tape, mem->rec[0].key);

            if (fscanf(readT1, "%d ", &mem->rec[0].key) == EOF) {
                mem->rec[0].key = -1;
                break;
            }
        }

        while (j++ < blockSize) {
            keyCount += writeKey(writeT1, writeT2, writeT3, tape, mem->rec[1].key);

            if (fscanf(readT2, "%d ", &mem->rec[1].key) == EOF) {
                mem->rec[1].key = -1;
                break;
            }
        }

        while (k++ < blockSize) {
            keyCount += writeKey(writeT1, writeT2, writeT3, tape, mem->rec[2].key);

            if (fscanf(readT3, "%d ", &mem->rec[2].key) == EOF) {
                mem->rec[2].key = -1;
                break;
            }
        }

        i = j = k = 0;

        /* Replaces the largest value of stored records if necessary.
         * This greater value will be passed to the recursion of this function at the moment
         * to merge the new blocks that can be created. */
        if (temp < keyCount)
            temp = keyCount;


        /* If the read tape is the last of the recursion and there are still records
         * to be interleaved the function returns to insert the blocks on the first tape. */
        if (tape == tape03 && (mem->rec[0].key != -1 || mem->rec[1].key != -1 || mem->rec[2].key != -1)) {
            tape = tape01;
            continue;
        }

        /* When the writing tape 1, for example, is filled with all the blocks
         * the tape controller is incremented, being considered the
         * tape that follows it: tape 2. */
        tape++;
    }

    if (recursions < complexity) {
        exchangeTapes();

        if (recursions%2 == 0)
            merge(tapes[0], tapes[1], tapes[2], tapes[3], tapes[4], tapes[5], temp);
        else
            merge(tapes[3], tapes[4], tapes[5], tapes[0], tapes[1], tapes[2], temp);
    }
}

/* Balanced interleaving through multiple paths. In this first part of the algorithm
 * the first 3 tapes with the ordered blocks are created and filled. Then,
 * merge() interleaves the tape elements according to the number of
 * required steps for correct file ordering. */
FILE *bitmp(FILE *clutteredFile) {
    int i = 0, tape = 0;

    /* The first tape to be filled in is the tape 1. */
    tape = tape01;

    /* Reads and inserts the key in the memory. */
    while (fscanf(clutteredFile, "%d ", &mem->rec[i].key) != EOF) {
        i++;

        /* If the memory is full, the ordering is done and the
         * is inserted into the chosen tape of the passage. */
        if (i == mem->size) {
            insertionSort(mem->rec, mem->size);

            /* After sorting the next ordered block is written on the subsequent tape. */
            tape = writeBlock(tape, mem->size);

            i = 0;
        }
    }

    /* If there are still records in memory to be inserted into the tape
     * the ordering of these is made and they are also inserted */
    if (i > 0) {
        insertionSort(mem->rec, i);
        writeBlock(tape, i);
    }

    openTapes(0, 3, "r");  // The first 3 tapes are read tapes.

    /* The first 3 parameters of the merge function are the reading tapes and
     * the other 3 are writing tapes. The last parameter is the size of the block
     * to be considered in the readings. */
    merge(tapes[0], tapes[1], tapes[2], tapes[3], tapes[4], tapes[5], mem->size);

    for (i = 0; i < 6; i++)  // Close all the tapes on our array.
        fclose(tapes[i]);

    /* If the number of passes is even, the resulting tape
     * ordered will be tape 1. Otherwise, it will be tape 4. */
    if (recursions%2 == 0)
        return fopen("tape1.txt", "r");
    else
        return fopen("tape4.txt", "r");
}

/* Main function of the external sort. Here it is also
 * set the size of the internal sort area (memory). */
void sort(FILE *clutteredFile, int memorySize) {
    FILE *sortedTape = NULL, *sortedFile = NULL;
    int i = 0, key = 0, op = 0;

    /* Initialization of the system's internal memory. */
    mem = (memory *) malloc(sizeof(memory));

    mem->size = memorySize;
    mem->rec = (record *) malloc(memorySize * sizeof(record));

    for (i = 0; i < memorySize; i++)  // Initializes the memory with null keys (-1).
        mem->rec[i].key = -1;

    /* Creates and saves the 6 available tapes in the array. */
    tapes = (FILE **) malloc(6 * sizeof(FILE *));

    for (i = 0; i < 6; i++)  // Initializes the tapes array with null pointers.
        tapes[i] = NULL;

    openTapes(0, 6, "w");

    /* Beginning of the balanced interleaving through multiple paths algorithm.
     * It returns the ordered final tape that will be copied to a final file. */
    complexity = getComplexity(memorySize);
    sortedTape = bitmp(clutteredFile);

    sortedFile = fopen("sorted.txt", "w");
    if (sortedFile == NULL) {
        printf("Error creating the sorted file: %s\n", strerror(errno));

        /* Even if the system can not create the sorted file, the cluttered file will already be sorted at this point. */
        exit(EXIT_SUCCESS);
    }

    while (fscanf(sortedTape, "%d ", &key) != EOF)
        fprintf(sortedFile, "%d\n", key);

    free(tapes);
    free(mem);

    fclose(sortedFile);
    fclose(clutteredFile);

    printf("\nSuccess!\nFile sorted using %d recursions.\n\n", recursions);
    printf("Open the sorted file?\n[1] - Yes\t[2] - No\n\n> ");
    scanf("%d", &op); fflush(stdin);

    switch (op) {
        case 1:
            system("sorted.txt");
            break;

        default:
            return;
    }
}
