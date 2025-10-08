#include "p1.h"
#include "aux.h"

int Cmd_create(char *trozos[],int ntrozos, Listas L) {
    

    if (ntrozos ==3) {
        if (strcmp(trozos[0],"-f")==0) {
            if(trozos[1]!=NULL) {
                if (fopen(trozos[1], "w")==NULL) {
                    perror("Error al crear el fichero");
                    return 1;
                }
                printf("Fichero '%s' creado con exito\n", trozos[1]);
                return 0;
            }
            perror("Nombre de fichero invalido");
            return 1;
        }
        perror("Argumento invalido");
        return 1;
    }
    if(ntrozos==2){
        if(trozos[0]!=NULL) {
            if (mkdir(trozos[0], 0777) == -1) {
                perror("Error al crear el directorio");
                return 1;
            }
            printf("Directorio '%s' creado con exito\n", trozos[0]);
            return 0;
        }
        perror("Nombre de directorio invalido");
        return 1;
    }
    perror("Numero de parametros invalidos");
    return 1;
}


int Cmd_erase(char *trozos[], int ntrozos, Listas L) {
    int errorflag=0;
    if (ntrozos < 2) {
        perror("Numero de argumentos invalidos");
        return 1;
    }

    for (int i = 0; i < ntrozos-1; i++) {
        struct stat info;
        // Comprobamos si el archivo o directorio existe
        if (stat(trozos[i], &info) == -1) {
            fprintf(stderr, "No se ha encontrado el archivo o directorio: %s: %s\n", trozos[i], strerror(errno));
            errorflag=1;
            continue;
        }

        if (S_ISDIR(info.st_mode)) {
            if(rmdir(trozos[i]) == 0) {
                printf("Directorio eliminado: %s\n", trozos[i]);
            }else{
                fprintf(stderr, "Error al eliminar directorio %s: %s\n", trozos[i], strerror(errno));
                errorflag=1;
            }
        }else if(S_ISREG(info.st_mode) ) {
            if(unlink(trozos[i]) == 0) {
                printf("Fichero eliminado: %s\n", trozos[i]);
            }else{
                fprintf(stderr, "Error al eliminar fichero %s: %s\n", trozos[i], strerror(errno));
                errorflag=1;
            }
        }
    }
    return errorflag;
}
int Cmd_delrec(char *trozos[], int ntrozos, Listas L) {
    int errorflag=0;
    if (ntrozos < 2) {
        perror("Numero de argumentos invalidos");
        return 1;
    }
    for (int i = 0; i < ntrozos-1; i++) {
        if (aux_remove_rec(trozos[i]) != 0)
            errorflag = 1;  // Guardamos si hubo algún error
    }
    return errorflag;
}