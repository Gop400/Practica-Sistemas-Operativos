#ifndef P0_H
#define P0_H

#include "list.h"
int historic(char *trozos[],int ntrozos,Listas L, char *env[]);
int Cmd_open (char * trozos[],int ntrozos,Listas L, char *env[]);
int Cmd_close (char *trozos[],int ntrozos,Listas L, char *env[]);
int Cmd_dup(char *trozos[],int ntrozos,Listas L, char *env[]);

#endif // P0_H


