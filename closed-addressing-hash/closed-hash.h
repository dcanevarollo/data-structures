#ifndef CLOSED_HASH_H
#define CLOSED_HASH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define bool char
#define true 1
#define false 0

/* This is an example of a student registration system
 * using extensible hash to store the values by the
 * enrollment key. */

/* Each record (student) will be a node of a list. This list
 * are created on a collision of hash index (mod function). */
typedef struct _student {
    int enrollment;
    int age;
    char *name;
    char gender;
    struct _student *next;
} student;

void printTable();

void insertStudent(char *name, int age, char gender, int enrollment);

bool deleteStudent(int enrollment);

bool findStudent(int enrollment);

#endif
