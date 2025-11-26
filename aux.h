#ifndef AUX_H
#define AUX_H
#include "list.h"
#include "p0.h"
#include "p2.h"

struct cmd {
    char *name;
    int (*func)(char *trozos[],int ntoken,Listas L);
};
extern struct cmd cmds[];

int TrocearCadena(char * cadena, char * trozos[]);

void Addhistoric(char cadena[],tList *l);

int ProcesarEntrada(char *trozos[],int ntoken,Listas L);

void CrearCharModos(char *modos,int filemodo);
void PrintOpenFiles(tList F);
char *NombreDescriptor(int df,Listas L);
void AnadirAFicherosAbiertos(tList *F,int fd, int modo, const char *nombre);
void initOpenList(Listas L);
void Recursiva (int n);
void LlenarMemoria (void *p, size_t cont, unsigned char byte);
void * ObtenerMemoriaShmget (key_t clave, size_t tam, Listas L);
void do_SharedCreate (char *tr[], Listas L);
void do_Shared (char *tr[], Listas L);
void doSharedFree (key_t cl, Listas L);
void * MapearFichero (char * fichero, int protection,Listas L);

void do_Mmap(char *arg[],Listas L);
void RemoveFileFromMmap(Listas L, char *filename);

void do_SharedDelkey (char *args[]);
void * CadenatoPointer (char * s);
ssize_t LeerFichero (char *f, void *p, size_t cont);
ssize_t EscribirDesdeDescriptor(int df, void *p, size_t cont);
ssize_t LeerDesdeDescriptor(int df, void *p, size_t cont);
ssize_t EscribirFichero (char *f, void *p, size_t);
void MList_print(enum tAllocL tipo,Listas L);
void Aux_add_malloc_block(void *addr, size_t size, Listas L) ;
void Remove_malloc(size_t size, Listas L) ;


int EliminarNodoDireccion(Listas L, void *dir);


void* DireccionNodoShared(Listas L, key_t cl);

void MList_print(enum tAllocL tipo,Listas L);
void Aux_mem_funcs();

void Aux_mem_vars();
void Aux_mem_blocks(Listas L) ;


void Do_pmap (void);
bool PerteneceMemList(void *addr, Listas L);

char LetraTF (mode_t m);
#endif //AUX_H