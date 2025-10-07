#include "list.h"
#include "p0.h"
struct cmd {
    char *name;
    int (*func)(char *trozos[],int ntoken,Listas L);
};
extern struct cmd cmds[];

int TrocearCadena(char * cadena, char * trozos[]);

void Addhistoric(char cadena[],tList *l);

int ProcesarEntrada(char *trozos[],int ntoken,Listas L);