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


int Cmd_writestr(char *trozos[], int ntrozos, Listas L) {
    if (ntrozos < 3) {
        printf("Uso: writestr <df> <texto>\n");
        return 1;
    }

    int df = atoi(trozos[0]); // descriptor del fichero

    // --- 1️⃣ Unir todos los argumentos desde args[1] en una sola cadena ---
    size_t len_total = 0;
    for (int i = 1; i < ntrozos-1; i++){
        len_total += strlen(trozos[i]); 
    } 
    printf("len_total: %zu\n",len_total);
    char *str = malloc(len_total+1 + (ntrozos -2)); //1 para \0 y otro para espacios
    if (!str) {
        perror("malloc");
        return 1;
    }
    str[0] = '\0';

    for (int i = 1; i < ntrozos-1; i++) {
        strcat(str, trozos[i]);
        if (i < ntrozos - 2) strcat(str, " "); // añadir espacio entre palabras
    }

    // --- 2️⃣ Buscar el fichero por su descriptor ---
    tPos pos = first(L->OpenFilesList);
    tItemF file = NULL;

    while (pos != NULL) {
        tItemF current = (tItemF)getItem(L->OpenFilesList, pos);
        if (current->df == df) {
            file = current;
            break;
        }
        pos = next(L->OpenFilesList, pos);
    }

    if (file == NULL) {
        printf("Error: descriptor de fichero %d no encontrado.\n", df);
        free(str);
        return 1;
    }

    // --- 3️⃣ Escribir el texto ---
    ssize_t bytes = write(file->df, str, strlen(str));
    if (bytes < 0) {
        perror("Error al escribir en el fichero");
        free(str);
        return 1;
    }
    printf("%zd bytes\n",bytes);

    // --- 4️⃣ Actualizar offset y mostrar mensaje ---
    file->offset += bytes;
    printf("Escritos %zd bytes en el fichero '%s' (df=%d)\n", bytes, file->name, file->df);
    free(str);
    return 0;

}



int Cmd_lseek(char *trozos[], int ntrozos, Listas L) {
    if (ntrozos < 4) {
        printf("Uso: lseek <df> <offset> <SEEK_SET|SEEK_CUR|SEEK_END>\n");
        return 1;
    }

    int df = atoi(trozos[0]);
    off_t off = atoll(trozos[1]);
    int ref;

    // --- 1️⃣ Interpretar el argumento de referencia ---
    if (strcmp(trozos[2], "SEEK_SET") == 0)
        ref = SEEK_SET;
    else if (strcmp(trozos[2], "SEEK_CUR") == 0)
        ref = SEEK_CUR;
    else if (strcmp(trozos[2], "SEEK_END") == 0)
        ref = SEEK_END;
    else {
        fprintf(stderr, "Referencia no válida: %s (use SEEK_SET, SEEK_CUR o SEEK_END)\n", trozos[2]);
        return 1;
    }

    // --- 2️⃣ Buscar el fichero en la lista ---
    if (isEmptyList(L->OpenFilesList)) {
        fprintf(stderr, "Error: lista de ficheros no inicializada.\n");
        return 1;
    }

    tPos pos = first(L->OpenFilesList);
    tItemF file = NULL;

    while (pos != NULL) {
        tItemF actual = (tItemF)getItem(L->OpenFilesList, pos);
        if (actual->df == df) {
            file = actual;
            break;
        }
        pos = next(L->OpenFilesList, pos);
    }

    if (file == NULL) {
        fprintf(stderr, "Error: descriptor de fichero %d no encontrado.\n", df);
        return 1;
    }

    // --- 3️⃣ Obtener información del fichero ---
    struct stat st;
    if (fstat(file->df, &st) == -1) {
        perror("Error al obtener información del fichero");
        return 1;
    }

    // --- 4️⃣ Mover el offset ---
    off_t nuevo_off = lseek(file->df, off, ref);
    if (nuevo_off == (off_t)-1) {
        perror("Error al cambiar el offset con lseek");
        return 1;
    }

    // --- 5️⃣ Actualizar offset en la estructura ---
    file->offset = nuevo_off;

    // --- 6️⃣ Mostrar información detallada con tus funciones auxiliares ---
    char permisos[12];
    ConvierteModo(st.st_mode, permisos); // Usa tu función auxiliar

    printf("Descriptor: %d, nombre: '%s'\n", file->df, file->name);
    printf("Tipo: %c, Permisos: %s\n", LetraTF(st.st_mode), permisos);
    printf("Nuevo offset: %lld bytes\n", (long long)file->offset);

    return 0;
}
