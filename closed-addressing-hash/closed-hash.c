#include "closed-hash.h"

int mod = 0;
student *pointers = NULL;

void printTable() {
    int i = 0;
    student *p1 = NULL;

    printf("\n\n**Individual visualization per enrollment**\nHASH CODE\tENROLLMENT\tNAME\n");
    for (i = 0; i < mod; i++) {
        p1 = pointers[i].next;
        while (p1 != NULL) {
            printf("%d\t\t%d\t\t%s\n", i, p1->enrollment, p1->name);
            p1 = p1->next;
        }
    }

    printf("\n\n**Group visualization per hash code**\nHASH CODE\tCLOSED ADDRESS LIST (NAME)\n");
    for (i = 0; i < mod; i++) {
        printf("%d\t\t", i);
        p1 = pointers[i].next;
        while (p1 != NULL) {
            printf("%s ", p1->name);
            p1 = p1->next;
        }
        printf("\n");
    }
}

void insertStudent(char *name, int age, char gender, int enrollment) {
    int hashCode = 0;
    student *newStudent = (student *) malloc(sizeof(student));

    newStudent->name = (char *) malloc(strlen(name) * sizeof(char));

    strcpy(newStudent->name, name);
    newStudent->age = age;
    newStudent->gender = gender;
    newStudent->enrollment = enrollment;

    hashCode = (enrollment % mod);
    if (pointers[hashCode].next == NULL) {
        pointers[hashCode].next = newStudent;
        newStudent->next = NULL;
    } else {
        student *p1 = pointers[hashCode].next;

        while (p1->next != NULL) {
            p1 = p1->next;
        }

        p1->next = newStudent;
        newStudent->next = NULL;
    }
}

bool deleteStudent(int enrollment) {
    int hashCode = (enrollment % mod);
    student *p1 = pointers[hashCode].next;
    student *p2 = NULL;

    while (p1 != NULL) {
        if (p1->enrollment == enrollment) {
            if (p1 == pointers[hashCode].next) {
                pointers[hashCode].next = p1->next;
                free(p1);
            } else if (p1->next == NULL) {
                p2->next = NULL;
                free(p1);
            } else {
                p2->next = p1->next;
                free(p1);
            }

            printf("\nSUCCESS!\nEnrollment %d removed.\n", enrollment);

            return true;
        } else {
            p2 = p1;
            p1 = p1->next;
        }
    }

    printf("\nERROR!\nEnrollment %d not found.\n", enrollment);

    return false;
}

bool findStudent(int enrollment) {
    int hashCode = (enrollment % mod);
    student *p1 = pointers[hashCode].next;

    while (p1 != NULL && p1->enrollment != enrollment) {
        p1 = p1->next;
    }

    if (p1 == NULL)
        return false;

    return true;
}

/* This function checks if the int 'n' is a prime number. */
bool isPrime(int n) {
    int i = 0, w = 0;

    if (n == 2)
        return 1;
    if (n == 3)
        return 1;
    if ((n % 2) == 0)
        return 0;
    if ((n % 3) == 0)
        return 0;

    i = 5;
    w = 2;

    while ((i * i) <= n) {
        if ((n % i) == 0)
            return false;

        i += w;
        w = 6 - w;
    }

    return true;
}

/* This function needs to be called after the number of records
 * has settled. It returns the mod value to the hash code functions. */
int setMod(int tableSize) {
    mod = tableSize;

    while (!isPrime(mod))
        mod--;
}
