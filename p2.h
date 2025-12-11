#ifndef P2_H
#define P2_H
#include "list.h"


int Cmd_shared(char *trozos[],int ntrozos,  Listas ,char *env[]);
int Cmd_Free(char *trozos[], int ntrozos, Listas L,char *env[]);

int Cmd_Memory(char *trozos[], int ntrozos, Listas L, char *env[]); ;
int Cmd_ReadFile (char *trozos[],int ntrozos,Listas L, char *env[]);

int Cmd_WriteFile(char *trozos[], int ntrozos, Listas L, char *env[]); ;

int Cmd_Read(char *trozos[], int ntrozos, Listas L, char *env[]); ;

int Cmd_Write(char *trozos[], int ntrozos, Listas L, char *env[]);
int Cmd_recurse(char *trozos[], int ntrozos, Listas L, char *env[]);
int Cmd_Mmap(char *trozos[], int ntrozos, Listas L, char *env[]);
int Cmd_Memfill(char *trozos[], int ntrozos, Listas L, char *env[]);
int Cmd_memdump(char *trozos[], int ntrozos, Listas L, char *env[]);
int Cmd_malloc(char *trozos[], int ntrozos, Listas L, char *env[]) ;

#endif // P2_H


