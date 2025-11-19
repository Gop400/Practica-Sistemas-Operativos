#ifndef P2_H
#define P2_H
#include "list.h"


int Cmd_shared(char *trozos[],int ntrozos,  Listas );
int Cmd_Free(char *trozos[], int ntrozos, Listas L);

int Cmd_Memory(char *trozos[], int ntrozos, Listas L) ;
int Cmd_ReadFile (char *trozos[],int ntrozos,Listas L);

int Cmd_WriteFile(char *trozos[], int ntrozos, Listas L) ;

int Cmd_Read(char *trozos[], int ntrozos, Listas L);

int Cmd_Write(char *trozos[], int ntrozos, Listas L);

#endif // P2_H


