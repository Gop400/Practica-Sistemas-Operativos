
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

#define Max_OpenFiles 20
#define LNULL NULL

enum tAllocL { MALLOC, SHARED, MAPPED };
enum tStatusL { FINISHED, STOPPED, SIGNALED, ACTIVE };

typedef char tFNameL[1024];


struct structHist {
    char *comando;
    int id;
};
struct structMem {
    void * address;
    int size; 
    time_t time;
    enum tAllocL alloc; 
    key_t smb_key; 
    tFNameL file_name; 
    int file_desc;
};
struct structOpenFile {
    char *name; 
    int df;
    int modos;
    off_t offset;
};
struct structProc {
    pid_t pid; // PID
    time_t time; // Fecha de lanzamiento
    enum tStatusL status; // Estado
    int wstatus; // WStatus
    char * command; // Command Line
};
typedef void* tItem;
typedef struct structProc* tItemP;
typedef struct structHist* tItemH;
typedef struct structOpenFile* tItemF;
typedef struct structMem* tItemM;

typedef struct tNode* tPos;
struct tNode{
    tItem item;
    tPos next;
};
typedef tPos tList;
struct structListas{
    tList HistoricList;
    tList OpenFilesList;
    tList MemList;
    tList ProcList;

};
typedef struct structListas* Listas;

int sizeList(tList l1);
void createNode(tPos *p);
void deleteList(tList *l1,char type);
tItem getItem(tList l1,tPos p);
tPos first(tList l1);
tPos last(tList l1);
void CreateEmptyList(tList *l1);
bool InsertItem(tList *l1,tItem item,tPos p);
void RemoveElement(tList *l1,tPos p);
bool isEmptyList(tList l1);
tPos previous(tList l1,tPos p);
tPos next(tList l1,tPos p);

void RemoveElement(tList *l1, tPos p);

void RemoveProcElement(tList *l, tPos p);

void deleteList(tList *l1,char type) ;
// RemoveElement y deleteList para HistoricList
// ================================================
void RemoveHistoricElement(tList *l, tPos p) ;
void DeleteHistoricList(tList *l);
void DeleteProcList(tList *l) ;
// ================================================
// RemoveElement y deleteList para OpenFilesList
// ================================================
void RemoveOpenFileElement(tList *l, tPos p);
void DeleteOpenFilesList(tList *l) ;

void RemoveMemElement(tList *l, tPos p) ;

#endif //LIST_H

