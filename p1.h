#ifndef P1_H
#define P1_H
#include "list.h"

typedef enum { SHORT_FORMAT, LONG_FORMAT } DirFormat;
typedef enum { NO_LINK, LINK } LinkOption;
typedef enum { NO_HID, HID } HiddenOption;
typedef enum { NO_REC, RECA, RECB } RecursionOption;

int Cmd_create(char *trozos[],int ntrozos,Listas L);
int Cmd_erase(char *trozos[], int ntoken, Listas L);
int Cmd_delrec(char *trozos[], int ntrozos, Listas L);
int Cmd_writestr(char *trozos[], int ntrozos, Listas L);
int Cmd_lseek(char *trozos[], int ntrozos, Listas L) ;
int setdirparams(char *trozos[], int ntrozos, Listas L) ;
int cmd_dir(char *trozos[], int ntrozos, Listas L) ;


#endif // P1_H


