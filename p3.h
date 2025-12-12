#ifndef P3_H
#define P3_H
#include "list.h"

int Cmd_uid(char *trozos[], int ntrozos, Listas L, char *env[]);
int Cmd_envvar(char *trozos[], int ntrozos, Listas L, char *env[]);
int Cmd_showenv(char *trozos[],int ntrozos, Listas L, char *env[]);
int Cmd_fork (char *trozos[], int ntrozos, Listas L, char *env[]);
int Cmd_exec(char *trozos[], int ntrozos, Listas L, char *env[]);
#endif // P3_H


