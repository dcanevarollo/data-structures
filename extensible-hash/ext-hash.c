#include "ext-hash.h"

int globalDepth = 2, idCtrl = 0, numberDir = 0;
bool pageOverflow = false;
directory *dir = NULL;

/* Compare the binary keys. */
bool compareKeys(char *key1, char *key2, int depth) {
    int i = 0, count = 0;
    bool result = true;

    count = 0;
    i = (strlen(key1)-1);
    while (count < depth) {
        if (key1[i] != key2[i]) {
            result = false;
            break;
        }

        count++;
        i--;
    }

    return result;
}

/* Help function to cast a decimal number to binary. */
char *toBin(int decimal, int depth) {
    int i = 0;
    char *bin = (char *) malloc(globalDepth+1 * sizeof(char));

    for (i = depth; i >= 0; i--) {
        if (i == depth) {
            bin[i] = '\0';
            continue;
        } else if (decimal%2 == 1)
            bin[i] = '1';
        else
            bin[i] = '0';

        decimal /= 2;
    }

    return bin;
}

/* Turn the binary char array in a decimal; returns an integer. */
int toDecimal(char *bin) {
    int i = 0, dec = 0, binIndex = 0;

    binIndex = strlen(bin);
    while (binIndex--)
        dec += (int) pow(2, (double) i++) * (bin[binIndex]-48);

    return dec;
}

/* Hash function: returns the directory index. */
int hash(int accountNumber, int depth) {
    int index = 0;
    char *key;

    key = toBin(accountNumber, depth);
    index = toDecimal(key);

    return index;
}

bucket *createBucket(int id, int localDepth) {
    int i = 0;
    bucket *bkt = (bucket *) malloc(sizeof(bucket));

    bkt->id = id;
    bkt->localDepth = localDepth;
    bkt->rec = (record *) malloc(4 * sizeof(record));

    for (i = 0; i < 4; i++) {
        bkt->rec[i].accountNumber = -1;
        bkt->rec[i].recordId = -1;
    }

    return bkt;
}

void createDirectory() {
    int i = 0;

    numberDir = (int) pow(2, (double) globalDepth);
    dir = (directory *) malloc(numberDir * sizeof(directory));

    for (i = 0; i < numberDir; i++) {
        dir[i].keys = toBin(idCtrl, globalDepth);
        dir[i].bkt = createBucket(idCtrl++, globalDepth);
    }
}

/* Finds the corresponding directory for the 'target'.
 * For example, if the target is the '100' directory then this
 * function returns a pointer to the '000' directory. */
directory *getCorrespondingDir(directory *target) {
    int i = 0, initialIndex = 0;
    directory *corresponding = NULL;

    do {
        for (i = initialIndex; i < numberDir; i++)
            if (compareKeys(target->keys, dir[i].keys, globalDepth - 1)) {
                corresponding = &dir[i];
                break;
            }

        initialIndex = i+1;
    } while (corresponding == target);

    return corresponding;
}

void duplicateDirectories(int midIndex) {
    int i = 0;
    directory *corresponding = NULL;

    numberDir = (int) pow(2, (double) globalDepth);

    for (i = 0; i < midIndex; i++) // Add a less significant digit in existing directories.
        dir[i].keys = toBin(i, globalDepth);

    dir = (directory *) realloc(dir, numberDir * sizeof(directory));

    for (i = midIndex; i < numberDir; i++) { // Create the new directories.
        dir[i].keys = toBin(idCtrl++, globalDepth);

        corresponding = getCorrespondingDir(&dir[i]); // Finds the corresponding directory to remake the pointers.
        dir[i].bkt = corresponding == NULL ? NULL : corresponding->bkt;
    }
}

/* Overflow treatment: bucket division. */
void divideBuckets(directory *target) {
    int i = 0, count = 0;
    bool temp = false;
    bucket *newBucket = NULL, *auxBucket = NULL;
    directory *corresponding = NULL;

    newBucket = createBucket(target->bkt->id + 10, target->bkt->localDepth);

    /* If the keys of the new bucket are equal to those of the full bucket - considering
     * local depth -1 less significant digits - and the new bucket is not equal to the
     * full bucket, we find the new bucket directory. */
    corresponding = getCorrespondingDir(target);
    if (corresponding != NULL) {
        corresponding->bkt = newBucket;
        auxBucket = createBucket(-1, -1); // Help bucket to store the keys of the full bucket.

        for (i = 0; i < 4; i++) {
            auxBucket->rec[i].recordId = target->bkt->rec[i].recordId;
            auxBucket->rec[i].accountNumber = target->bkt->rec[i].accountNumber;

            target->bkt->rec[i].recordId = -1;
            target->bkt->rec[i].accountNumber = -1;
        }

        count = 0;
        for (i = 0; i < 4; i++) // Check if the correct division of the full bucket is possible.
            if (strcmp(target->keys, toBin(auxBucket->rec[i].accountNumber, globalDepth)) == 0)
                count++;

        if (count == 4) {
            temp = true;

            /* Flag that controls whether there was persistence of overflow (indivision of the full bucket) */
            pageOverflow = 1;
        }

        /* If overflow persists, half of the full bucket stays and the other half goes to the new bucket. */
        if (temp) {
            for (i = 0; i < 2; i++) {
                target->bkt->rec[i].recordId = auxBucket->rec[i].recordId;
                target->bkt->rec[i].accountNumber = auxBucket->rec[i].accountNumber;
            }

            for (i = 2; i < 4; i++) {
                corresponding->bkt->rec[i].recordId = auxBucket->rec[i].recordId;
                corresponding->bkt->rec[i].accountNumber = auxBucket->rec[i].accountNumber;
            }

        } else
            for (i = 0; i < 4; i++) // Inserts into the correct bucket each element of the bucket filled.
                insert(auxBucket->rec[i].recordId, auxBucket->rec[i].accountNumber);

        free(auxBucket);
    }
}

void insert(int recordId, int accountNumber) {
    int i = 0, index;

    index = hash(accountNumber, globalDepth);
    for (i = 0; i < 4; i++) // Finds a available space in the bucket and insert the record in the same.
        if (dir[index].bkt->rec[i].accountNumber == -1) {
            dir[index].bkt->rec[i].recordId = recordId;
            dir[index].bkt->rec[i].accountNumber = accountNumber;
            break;
        }

    if (i == 4) { // Overflow

        /* If the global depth is equal to local depth of the full bucket
         * (increment of the full bucket location is already done). */
        if (globalDepth == dir[index].bkt->localDepth++) {
            if (globalDepth == 3) { // Max size of global depth.
                dir[index].bkt->localDepth--;

                system("cls");
                printf("ERROR!\nOverflow occurred.\nThe account %d cannot be registered.\n", accountNumber);
                sleep(2);

                return;
            }

            globalDepth++;
            duplicateDirectories(i);
        }

        divideBuckets(&dir[index]);

        /* Recursive call to insert function. Now the buckets are divided and
         * the key can be inserted on the correct index/directory. */
        insert(recordId, accountNumber);
    }
}

int removeKey(directory *target, int accountNumber) {
    int i = 0;

    for (i = 0; i < 4; i++)
        if (target->bkt->rec[i].accountNumber == accountNumber) {
            target->bkt->rec[i].recordId = -1;
            target->bkt->rec[i].accountNumber = -1;
            break;
        }

    return i;
}

void purge(int accountNumber) {
    int i = 0, index = 0;
    bool hasRecord = false;
    directory *target = NULL, *corresponding = NULL;
    bucket *auxBucket = NULL;

    index = hash(accountNumber, globalDepth);
    i = removeKey(&dir[index], accountNumber);
    corresponding = getCorrespondingDir(&dir[index]);
    target = &dir[index];

    /* If you have not found the record in your supposed correct index,
     * look in the corresponding directory and do the removal. */
    if (i == 4 && pageOverflow) {
        i = removeKey(corresponding, accountNumber);
        target = corresponding;
    }

    if (i == 4) {
        system("cls");
        printf("Account %d not found.\n", accountNumber);
        sleep(2);

        return;
    }

    for (i = 0; i < 4; i++) // Checks if the bucket whose record has been removed is empty.
        if (target->bkt->rec[i].accountNumber != -1) {
            hasRecord = true;
            break;
        }

    /* If it is empty, merge this bucket with its corresponding,
     * '100' with '000', for example, by decreasing local depth. */
    if (!hasRecord && target->bkt->localDepth > 2) {
        corresponding = getCorrespondingDir(target);

        if (corresponding != NULL) {
            auxBucket = target->bkt;
            corresponding->bkt->localDepth--;
            target->bkt = corresponding->bkt;

            free(auxBucket);
        }
    }

    /* If all local depths are smaller than the global depth,
     * it returns the directories to the previous state
     * (decrement of the global page). */
    for (i = 0; i < numberDir; i++)
        if (dir[i].bkt->localDepth == globalDepth)
            return;

    globalDepth--;
    numberDir = (int) pow(2, (double) globalDepth);
    dir = (directory *) realloc(dir, numberDir * sizeof(directory));

    for (i = 0; i < numberDir; i++)
        dir[i].keys = toBin(i, globalDepth);
}

void printHash() {
    int i = 0, j = 0;

    system("cls");
    printf("[%d]\n\n", globalDepth);

    for (i = 0; i < numberDir; i++) {
        printf("Directory %s -> Bucket %d:\t", dir[i].keys, dir[i].bkt->id);
        for (j = 0; j < 4; j++)
            dir[i].bkt->rec[j].accountNumber != -1 ? printf("|\t%d\t", dir[i].bkt->rec[j].accountNumber) : printf("|\t\t");
        printf("| [%d]\n\n", dir[i].bkt->localDepth);
    }

    printf("Press ENTER to return to main menu.\n\n");
    getchar();
}