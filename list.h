
#ifndef LIST_H
#define LIST_H
#include <grp.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/utsname.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <pwd.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>


#define LNULL NULL

struct structHist {
    char *comando;
    int id;
};

struct structOpenFile {
    char *name; 
    int df;
    int modos;
};
typedef void* tItem;
typedef struct structHist* tItemH;
typedef struct structOpenFile* tItemF;

typedef struct tNode* tPos;
struct tNode{
    tItem item;
    tPos next;
};
typedef tPos tList;
struct structListas{
    tList HistoricList;
    tList OpenFilesList;

};
typedef struct structListas* Listas;

int sizeList(tList l1);
void createNode(tPos *p);
void deleteList(tList *l1);
tItem getItem(tList l1,tPos p);
tPos first(tList l1);
tPos last(tList l1);
void CreateEmptyList(tList *l1);
bool InsertItem(tList *l1,tItem item,tPos p);
void RemoveElement(tList *l1,tPos p);
bool isEmptyList(tList l1);
tPos previous(tList l1,tPos p);
tPos next(tList l1,tPos p);

#endif //LIST_H

