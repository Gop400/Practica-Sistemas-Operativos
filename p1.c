#include "p1.h"

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