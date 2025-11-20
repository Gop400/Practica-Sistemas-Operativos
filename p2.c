#include "p2.h"
#include "aux.h"

int Cmd_shared(char *trozos[], int ntoken, Listas L) {
    if (ntoken == 1) { // solo "shared"
        MList_print(SHARED,L);
        return 0;
    }

    // shared -create cl n
    if (strcmp(trozos[0], "-create") == 0) {
        do_SharedCreate(trozos + 1, L);
        return 0;
    }

    // shared -free cl
    if (strcmp(trozos[0], "-free") == 0) {
        doSharedFree((key_t) strtoul(trozos[1], NULL, 10), L);
        return 0;
    }

    // shared -delkey cl
    if (strcmp(trozos[0], "-delkey") == 0) {
        do_SharedDelkey(trozos + 1);
        return 0;
    }
    // shared cl   → attach
    do_Shared(trozos, L);
    return 0;
}

int Cmd_Free(char *trozos[], int ntrozos, Listas L) {
    tList lista = L->MemList;
    tPos p = first(lista);
    if(ntrozos != 2) {
        printf("Uso: free <dirección>\n");
        return 1;
    }
    if (trozos[0] == NULL) {
        printf("Dirección NULL no válida\n");
        return 1;
    }
    /* Buscar el nodo con esa dirección */
    while (p != LNULL) {
        tItemM item = (tItemM) getItem(lista, p);
        if (item->address == CadenatoPointer(trozos[0])) {
            /* Según tipo de asignación */
            switch (item->alloc) {
                case MALLOC:
                    free(CadenatoPointer(trozos[0]));
                    printf("Bloque malloc liberado en %p\n", CadenatoPointer(trozos[0]));
                    break;
                case SHARED:
                    if (shmdt(CadenatoPointer(trozos[0])) == -1)
                        perror("shmdt");
                    else
                        printf("Bloque shared detach en %p (clave=%d)\n",
                               CadenatoPointer(trozos[0]), item->smb_key);
                    break;
                case MAPPED:
                    if (munmap(CadenatoPointer(trozos[0]), item->size) == -1)
                        perror("munmap");
                    else
                        printf("Bloque mapped desmapeado en %p archivo=%s\n",
                               CadenatoPointer(trozos[0]), item->file_name);
                    break;
                default:
                    printf("Tipo de bloque desconocido\n");
                    break;
            }
            /* Quitar nodo de la lista */
            RemoveMemElement(&(L->MemList), p);
            return 0;
        }
        p = next(lista, p);
    }
    /* Si llegamos aquí, no lo encontró */
    fprintf(stderr, "No existe ningún bloque con dirección %p\n", CadenatoPointer(trozos[0]));
    return 1;
}

int Cmd_Memory(char *trozos[], int ntrozos, Listas L) {
    if (ntrozos != 2) {
        printf("Uso: memory -funcs|-vars|-blocks|-all|-pmap\n");
        return 1;
    }
    if(strcmp(trozos[0], "-funcs") == 0){
        Aux_mem_funcs();
        return 0;
    }
    if(strcmp(trozos[0], "-vars") == 0){
        Aux_mem_vars();
        return 0;
    }
    if(strcmp(trozos[0], "-blocks") == 0){
        Aux_mem_blocks(L);
        return 0;
    }
    if(strcmp(trozos[0], "-all") == 0){
        Aux_mem_funcs();
        Aux_mem_vars();
        Aux_mem_blocks(L);
        return 0;
    }if(strcmp(trozos[0], "-pmap") == 0){
        Do_pmap();
        return 0;
    }
    return 1;
}

int Cmd_ReadFile (char *trozos[],int ntrozos,Listas L){
    void *p;
    size_t cont=-1;  /*si no pasamos tamano se lee entero */
    ssize_t n;
    if (ntrozos<4){
        printf("Uso: readfile <df> <puntero> <num_bytes>\n");
        return 1;
    }
    p=CadenatoPointer(trozos[1]);  /*convertimos de cadena a puntero*/
    if(p==NULL){
        fprintf(stderr,"Dirección inválida: %s\n", trozos[1]);
        return 1;
    }
    cont=(size_t) atoll(trozos[2]);
    if(cont==0){
        fprintf(stderr,"Número de bytes inválido: %s\n", trozos[2]);
        return 1;
    }

    if ((n=LeerFichero(trozos[0],p,cont))==-1){
        fprintf (stderr,"Imposible leer fichero: %s\n",strerror(errno));
        return 1;
    }
    printf ("leidos %lld bytes de %s en %p\n",(long long) n,trozos[0],p);
    return 0;
}

int Cmd_WriteFile(char *trozos[], int ntrozos, Listas L) {
    void *p;
    size_t cont;  /*si no pasamos tamano se lee entero */
    ssize_t n;
    if (ntrozos<4){
        printf("Uso: writefile <df> <puntero> <num_bytes>\n");
        return 1;
    }
    p=CadenatoPointer(trozos[1]);
    if(p==NULL){
        fprintf(stderr,"Dirección inválida: %s\n", trozos[1]);
        return 1;
    }  /*convertimos de cadena a puntero*/
    cont=(size_t) atoll(trozos[2]);
    if(cont==0){
        fprintf(stderr,"Número de bytes inválido: %s\n", trozos[2]);
        return 1;
    }

    if ((n=EscribirFichero (trozos[0],p,cont))==-1){
        fprintf (stderr,"Imposible escribir fichero: %s\n",strerror(errno));
        return 1;
    }
    printf ("escritos %lld bytes de %s en %p\n",(long long) n,trozos[0],p);
    return 0;
}

int Cmd_Read(char *trozos[], int ntrozos, Listas L)
{
    int df;
    if (ntrozos < 3) {
        fprintf(stderr, "Uso: read <df> <addr> <num_bytes>\n");
        return 1;
    }

    // Convertir descriptor a int
    if ((df=atoi(trozos[0]))<0) {
        fprintf(stderr, "Descriptor de archivo inválido: %s\n", trozos[0]);
        return 1;
    }
    // Convertir dirección de string a puntero
    void *p = CadenatoPointer(trozos[1]);
    if (p == NULL) {
        fprintf(stderr, "Dirección inválida: %s\n", trozos[1]);
        return 1;
    }

    // Convertir número de bytes
    size_t cont = (size_t) atoll(trozos[2]);
    if (cont == 0) {
        fprintf(stderr, "Número de bytes inválido: %s\n", trozos[2]);
        return 1;
    }

    // Leer desde descriptor
    ssize_t n = LeerDesdeDescriptor(df, p, cont);
    if (n == -1) {
        fprintf(stderr, "Imposible leer del descriptor %d: %s\n", df, strerror(errno));
        return 1;
    }

    printf("Leídos %lld bytes del descriptor %d en %p\n", (long long)n, df, p);
    return 0;
}

int Cmd_Write(char *trozos[], int ntrozos, Listas L)
{
    if (ntrozos < 3) {
        fprintf(stderr, "Uso: write <df> <addr> <num_bytes>\n");
        return 1;
    }

    // Convertir descriptor a int
    int df = (int) strtol(trozos[0], NULL, 10);
    if (df < 0) {
        fprintf(stderr, "Descriptor de archivo inválido: %s\n", trozos[0]);
        return 1;
    }

    // Convertir dirección de string a puntero
    void *p = CadenatoPointer(trozos[1]);
    if (p == NULL) {
        fprintf(stderr, "Dirección inválida: %s\n", trozos[1]);
        return 1;
    }

    // Convertir número de bytes
    size_t cont = (size_t) atoll(trozos[2]);
    if (cont == 0) {
        fprintf(stderr, "Número de bytes inválido: %s\n", trozos[2]);
        return 1;
    }

    // Escribir desde memoria hacia descriptor
    ssize_t n = EscribirDesdeDescriptor(df, p, cont);
    if (n == -1) {
        fprintf(stderr, "Imposible escribir en descriptor %d: %s\n", df, strerror(errno));
        return 1;
    }

    printf("Escritos %lld bytes en descriptor %d desde %p\n", (long long)n, df, p);
    return 0;
}

int Cmd_recurse(char *trozos[], int ntrozos, Listas L) {
    if (ntrozos != 2) {
        printf("Uso: recurse <n>\n");
        return 1;
    }
    int n = atoi(trozos[0]);
    if (n <= 0) {
        printf("El número de recursiones debe ser mayor que 0.\n");
        return 1;
    }
    Recursiva(n);
    return 0;
}
