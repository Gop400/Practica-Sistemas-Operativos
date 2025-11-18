#include "p1.h"
#include "aux.h"

DirFormat g_format = SHORT_FORMAT;
LinkOption g_link = NO_LINK;
HiddenOption g_hidden = NO_HID;
RecursionOption g_recursion = NO_REC;

int Cmd_create(char *trozos[],int ntrozos, Listas L) {
    int fd;
    if (ntrozos ==3) {
        if (strcmp(trozos[0],"-f")==0) {
            if(trozos[1]!=NULL) {
                fd = open(trozos[1], O_CREAT | O_WRONLY | O_TRUNC, 0666);
                if (fd == -1) {
                    perror("Error al crear el fichero");
                    return 1;
                }
                printf("Fichero '%s' creado con exito\n", trozos[1]);
                AnadirAFicherosAbiertos(&L->OpenFilesList,fd,O_RDWR,trozos[1]);
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

    struct stat st;
    if (fstat(file->df, &st) == -1) {
        perror("Error al obtener información del fichero");
        return 1;
    }

    off_t nuevo_off = lseek(file->df, off, ref);
    if (nuevo_off == (off_t)-1) {
        perror("Error al cambiar el offset con lseek");
        return 1;
    }

    file->offset = nuevo_off;

    char permisos[12];
    ConvierteModo(st.st_mode, permisos); // Usa tu función auxiliar

    printf("Descriptor: %d, nombre: '%s'\n", file->df, file->name);
    printf("Tipo: %c, Permisos: %s\n", LetraTF(st.st_mode), permisos);
    printf("Nuevo offset: %lld bytes\n", (long long)file->offset);

    return 0;
}



int setdirparams(char *trozos[], int ntrozos, Listas L) {
    if (ntrozos <2) {
        char *result = GetDirParamsString(g_format, g_link, g_hidden, g_recursion);
        printf("%s\n", result);
        free(result);
        return 0;
    }

    for (int i = 0; i < ntrozos-1; i++) {
        if (strcmp(trozos[i], "long") == 0) {
            g_format = LONG_FORMAT;
            continue;
        }if (strcmp(trozos[i], "short") == 0) {
            g_format = SHORT_FORMAT;
            continue;
        }if (strcmp(trozos[i], "link") == 0) {
            g_link = LINK;
            continue;
        } if (strcmp(trozos[i], "nolink") == 0) {
            g_link = NO_LINK;
            continue;
        } if (strcmp(trozos[i], "hid") == 0) {
            g_hidden = HID;
            continue;
        } if (strcmp(trozos[i], "nohid") == 0) {
            g_hidden = NO_HID;
            continue;
        } if (strcmp(trozos[i], "reca") == 0) {
            g_recursion = RECA;
            continue;
        } if (strcmp(trozos[i], "recb") == 0) {
            g_recursion = RECB;
            continue;
        } if (strcmp(trozos[i], "norec") == 0) {
            g_recursion = NO_REC;
            continue;
        } else {
            fprintf(stderr,"Parámetro desconocido: %s %s\n", trozos[i],strerror(errno));
        }
    }
    printf("Parámetros actualizados correctamente.\n");
    return 0;
}
static void print_file_info(const char *dirpath, const char *filename) {
    char fullpath[4096], linkdest[4096], permisos[12];
    snprintf(fullpath, sizeof(fullpath), "%s/%s", dirpath, filename);// Construir la ruta completa

    struct stat s;
    if (lstat(fullpath, &s) == -1) {
        fprintf(stderr, "%s imposible de acceder: %s\n", fullpath, strerror(errno));
        return;
    }

    if (g_hidden == NO_HID && filename[0] == '.') return;

    if (g_format == SHORT_FORMAT) {
        printf("%s\t%ld bytes\n", filename, (long)s.st_size);
    } else {
        ConvierteModo(s.st_mode, permisos);
        struct passwd *pw = getpwuid(s.st_uid);
        struct group *gr = getgrgid(s.st_gid);

        char fecha[32];
        strftime(fecha, sizeof(fecha), "%Y-%m-%d %H:%M", localtime(&s.st_mtime));

        printf("%s %2lu %s %s %8ld %s %s",
            permisos,
            (unsigned long)s.st_nlink,
            pw ? pw->pw_name : "?",
            gr ? gr->gr_name : "?",
            (long)s.st_size,
            fecha,
            filename);

        if (S_ISLNK(s.st_mode) && g_link == LINK) {
            ssize_t len = readlink(fullpath, linkdest, sizeof(linkdest) - 1);
            if (len != -1) {
                linkdest[len] = '\0';
                printf(" -> %s", linkdest);
            }
        }
        printf("\n");
    }
}

static int list_directory_recursive(const char *path, int list_contents) {
    DIR *dirp = opendir(path);
    if (!dirp) {
        perror(path);
        return 1;
    }

    struct dirent *dp;
    struct stat s;
    char fullpath[4096];

    // RECB: primero recursión en subdirectorios
    if (g_recursion == RECB && list_contents) {
        rewinddir(dirp);
        while ((dp = readdir(dirp)) != NULL) {
            if (g_hidden == NO_HID && dp->d_name[0] == '.') continue;
            if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0) continue;

            snprintf(fullpath, sizeof(fullpath), "%s/%s", path, dp->d_name);
            if (lstat(fullpath, &s) == -1) continue;

            if (S_ISDIR(s.st_mode)) {
                list_directory_recursive(fullpath, list_contents);
            }
        }
        rewinddir(dirp);
    }

    // Listado de archivos/directorios actuales
    printf("************%s\n", path);
    while ((dp = readdir(dirp)) != NULL) {
        if (g_hidden == NO_HID && dp->d_name[0] == '.') continue;
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0) continue;

        print_file_info(path, dp->d_name);
    }

    // RECA: primero listar archivos y luego recursión en subdirectorios
    if (g_recursion == RECA && list_contents) {
        rewinddir(dirp);
        while ((dp = readdir(dirp)) != NULL) {
            if (g_hidden == NO_HID && dp->d_name[0] == '.') continue;
            if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0) continue;

            snprintf(fullpath, sizeof(fullpath), "%s/%s", path, dp->d_name);
            if (lstat(fullpath, &s) == -1) continue;

            if (S_ISDIR(s.st_mode)) {
                list_directory_recursive(fullpath, list_contents);
            }
        }
    }

    closedir(dirp);
    return 0;
}

int cmd_dir(char *trozos[], int ntrozos, Listas L) {
    int actual_args = 0;
    //se calcula el numero de trozos ,sin contar la funcion dir hay en la setencia
    for (int i = 0; i < ntrozos && trozos[i] != NULL; i++) actual_args++;

    int list_dir_contents = 0;
    int start_index = 0;

    // Si el primer argumento es -d
    if (actual_args > 0 && strcmp(trozos[0], "-d") == 0) {
        list_dir_contents = 1;
        start_index = 1;
        //no permite usar solo el -d sin mas argumentos
        if (actual_args == 1) {
            fprintf(stderr, "Error: se requiere al menos un directorio después de -d\n");
            return 1;
        }
    }
    if (start_index >= actual_args) {
        fprintf(stderr, "Error: se requiere al menos un directorio para listar\n");
        return 1;
    }

    for (int i = start_index; i < actual_args; i++) {
        const char *name = trozos[i];
        struct stat s;
        if (!name) continue;//
        //devuelve 0 si pudo obtener la informacion del archivo o directorio y guardarla en s
        if (lstat(name, &s) == -1) {
            fprintf(stderr, "%s imposible de acceder: %s\n", name, strerror(errno));    
            continue;
        }

        if (g_hidden == NO_HID) {//comprobar si es archivo oculto
            
            const char *base = strrchr(name, '/');//buscar la ultima aparicion de / en la cadena name,base apunta a esa posicion
            base = base ? base + 1 : name;//si base no es NULL, apunta al caracter despues de /, si es NULL apunta al inicio de name
            if (base[0] == '.') continue;//si el primer caracter de base es ., se omite el archivo o directorio(noHID)
        }

        if (S_ISDIR(s.st_mode) && list_dir_contents) {
            list_directory_recursive(name, list_dir_contents);
        } else {
            const char *slash = strrchr(name, '/');//buscar la ultima aparicion de / en la cadena name
            char dirpath[4096];
            if (slash) {//si se encontro /
                size_t len = slash - name;//calcular la longitud del path del directorio hasta /
                if (len >= sizeof(dirpath)) len = sizeof(dirpath) - 1;//asegurar que no se desborde el buffer
                strncpy(dirpath, name, len);//copiar el path del directorio en dirpath
                dirpath[len] = '\0';//agregar el terminador nulo
                print_file_info(dirpath, slash + 1);//imprimir la informacion del archivo usando el path del directorio y el nombre del archivo
            } else {
                print_file_info(".", name);//si no hay /, usar el directorio actual
            }
        }
    }

    return 0;
}
