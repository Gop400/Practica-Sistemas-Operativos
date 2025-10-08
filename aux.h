#include "list.h"
#include "p0.h"
#include "p1.h"
struct cmd {
    char *name;
    int (*func)(char *trozos[],int ntoken,Listas L);
};
extern struct cmd cmds[];

int TrocearCadena(char * cadena, char * trozos[]);

void Addhistoric(char cadena[],tList *l);

int ProcesarEntrada(char *trozos[],int ntoken,Listas L);

void CrearCharModos(char *modos,int filemodo);
void PrintDefaultOpen();
char *NombreDescriptor(int df,Listas L);
void AnadirAFicherosAbiertos(tList *F,int fd, int modo, const char *nombre);
void PrintOpenFiles(tList F);
int aux_remove_rec(const char *path);