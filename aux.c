
#include "aux.h"
#include "p0.h"
#include "p1.h"

struct cmd cmds[]={{"writestr",Cmd_writestr},{"delrec",Cmd_delrec},{"erase",Cmd_erase},{"dup",Cmd_dup},{"listopen",listopen},{"create",Cmd_create},{"close",Cmd_close},{"open",Cmd_open},{"historic",historic},{"help",help},{"date",date},{"authors",authors},{"pid",pid},{"infosys",infosys},{"getcwd",cmd_getcwd},{"cd",cmd_cd},{"hour",hour}};

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


int aux_remove_rec(const char *path) {
    struct stat info;

    if (stat(path, &info) == -1) {
        fprintf(stderr, "No se ha encontrado el archivo o directorio %s: %s\n", path, strerror(errno));
        return 1;
    }

    if (S_ISDIR(info.st_mode)) {
        // Es un directorio: abrimos su contenido
        DIR *dir = opendir(path);
        if (!dir) {
            fprintf(stderr, "Error al abrir directorio %s: %s\n", path, strerror(errno));
            return 1;
        }

        struct dirent *entry;
        int ret = 0;

        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){
                continue;
            }//IGNORA LAS ENTRADAS . Y .. QUE EL SISTEMDA DE ARCHIVOS INCLUYE SIEMPRE


            char sub_path[1024];
            snprintf(sub_path, sizeof(sub_path), "%s/%s", path, entry->d_name);//Crea una cadena de texto pero con un límite de tamaño, para evitar overflow y queda almacenada en sub_path
            // Llamada recursiva
            if (aux_remove_rec(sub_path) != 0)//si da error en la llamada recursiva devuelve 1
                ret = 1;
        }
        closedir(dir);

        // Borramos el directorio vacío ahora
        if (rmdir(path) == -1) {
            fprintf(stderr, "Error al eliminar directorio %s: %s\n", path, strerror(errno));
            ret = 1;
        } else {
            printf("Directorio eliminado: %s\n", path);
        }

        return ret;
    } else if (S_ISREG(info.st_mode)) {
        // Es un archivo normal
        if (unlink(path) == -1) {
            fprintf(stderr, "Error al eliminar fichero %s: %s\n", path, strerror(errno));
            return 1;
        } else {
            printf("Fichero eliminado: %s\n", path);
            return 0;
        }
    }
    return 1;
}
