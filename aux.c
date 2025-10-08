
#include "aux.h"
#include "p0.h"
#include "p1.h"

struct cmd cmds[]={{"dup",Cmd_dup},{"listopen",listopen},{"create",Cmd_create},{"close",Cmd_close},{"open",Cmd_open},{"historic",historic},{"help",help},{"date",date},{"authors",authors},{"pid",pid},{"infosys",infosys},{"getcwd",cmd_getcwd},{"cd",cmd_cd},{"hour",hour}};

int TrocearCadena(char * cadena, char * trozos[])
{ int i=1;
    if ((trozos[0]=strtok(cadena," \n\t"))==NULL)
        return 0;

    while ((trozos[i]=strtok(NULL," \n\t"))!=NULL)
        i++;
    return i;
}



int ProcesarEntrada(char *trozos[],int ntrozos,Listas L) {
    int i;
    for(i=0;cmds[i].name!=NULL;i++) {
        if((strcmp(cmds[i].name,trozos[0]))==0) {
            cmds[i].func(trozos+1,ntrozos,L);
            return 0;
        }
    }if(strcmp(trozos[0],"quit")==0 || strcmp(trozos[0],"exit")==0 ||strcmp(trozos[0],"bye")==0) {
        return 1;
    }
    perror("comando invalido");
    return 0;
}

void Addhistoric(char cadena[],tList *l) {
    tItemH d=(tItem)malloc(sizeof(struct structHist));
    d->comando=strdup(cadena);
    if(isEmptyList(*l)) {
        d->id=0;
    }else{
        d->id=((((tItemH) getItem(*l, last(*l)))->id)+1);
    }
    InsertItem(l,d,NULL);
    return;
}

void CrearCharModos(char *modos,int filemodo){
    if ((filemodo & (O_RDONLY | O_WRONLY | O_RDWR)) == O_RDONLY) {
        strcat(modos, "O_RDONLY,");
    }

    if (filemodo & O_WRONLY) {
        strcat(modos, "O_WRONLY,");
    }
    if (filemodo & O_RDWR) {
        strcat(modos, "O_RDWR,");
    }
    if (filemodo & O_CREAT) {
        strcat(modos, "O_CREAT,");
    }
    if (filemodo & O_EXCL) {
        strcat(modos, "O_EXCL,");
    }
    if (filemodo & O_APPEND) {
        strcat(modos, "O_APPEND,");
    }
    if (filemodo & O_TRUNC) {
        strcat(modos, "O_TRUNC,");
    }
    size_t len = strlen(modos);
    if(len > 0 && modos[len-1] == ',') {
        modos[len-1] = '\0';
    }
    
}
void PrintDefaultOpen(){
    printf("Descriptor: 0 ,offset :( )-> entrada estandar O_RDWR\n");
    printf("Descriptor: 1 ,offset :( )-> salida estandar O_RDWR\n");
    printf("Descriptor: 2 ,offset :( )-> error estandar O_RDWR\n");
}

char *NombreDescriptor(int df,Listas L){
    tPos p=first(L->OpenFilesList);
    tItemF file;
    while(p!=LNULL){
        file=(tItemF)getItem(L->OpenFilesList,p);
        if(file->df==df){
            return file->name;
        }
        p=next(L->OpenFilesList,p);
    }
    return "Not Found";
}

void AnadirAFicherosAbiertos(tList *F,int fd, int modo, const char *nombre) {
    if (sizeList(*F)>= Max_OpenFiles) {
        printf("Tabla de ficheros abiertos llena.\n");
        return;
    }
    tItemF file=(tItem)malloc(sizeof(struct structOpenFile));
    file->df=fd;
    file->modos=modo;
    file->name=strdup(nombre);
    file->offset=lseek(fd,0,SEEK_CUR);
    InsertItem(F,file,NULL);
    return;
}

void PrintOpenFiles(tList F) {
    char modos[64]; // buffer suficiente para los flags
    if (isEmptyList(F)) {
        PrintDefaultOpen();
        return;
    }
    PrintDefaultOpen();
    tPos p = first(F);
    tItemF file;
    while (p != LNULL) {
        modos[0] = '\0';               // vaciar el buffer al inicio de cada iteración
        file = (tItemF)getItem(F, p);   // obtener el archivo actual
        CrearCharModos(modos, file->modos); // convertir los modos a cadena
        printf("Descriptor: %d ,offset :(%ld)-> %s %s\n", file->df,file->offset,file->name,modos);
        p = next(F, p);                // pasar al siguiente elemento
    }
}