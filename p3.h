#ifndef P3_H
#define P3_H
#include "list.h"

int Cmd_uid(char *trozos[], int ntrozos, Listas L, char *env[]);
int Cmd_envvar(char *trozos[], int ntrozos, Listas L, char *env[]);
int Cmd_showenv(char *trozos[],int ntrozos, Listas L, char *env[]);
int Cmd_fork (char *trozos[], int ntrozos, Listas L, char *env[]);
int Cmd_exec(char *trozos[], int ntrozos, Listas L, char *env[]);
int jobs(char *trozos[], int ntrozos, Listas L, char *env[]);
int deljobs(char *trozos[], int ntrozos, Listas L, char *env[]);


void ejecutarComando(char *trozos[], int ntrozos, Listas L) ;

#endif // P3_H


