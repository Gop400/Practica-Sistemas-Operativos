#include "p2.h"
#include "aux.h"

int Cmd_shared(char *trozos[], int ntoken, Listas L) {
    if (ntoken == 1) { 
        MList_print(SHARED,L);
        return 0;
    }

    if (strcmp(trozos[0], "-create") == 0 && ntoken == 4) {
        do_SharedCreate(trozos + 1, L);
        return 0;
    }

    if (strcmp(trozos[0], "-free") == 0 && ntoken == 3) {
        doSharedFree((key_t) strtoul(trozos[1], NULL, 10), L);
        return 0;
    }

    if (strcmp(trozos[0], "-delkey") == 0 && ntoken == 3) {
        do_SharedDelkey(trozos + 1);
        return 0;
    }
    if(ntoken == 2) {
        do_Shared(trozos, L);
        return 0;
    }
    fprintf(stderr, "Uso: shared | shared -create <cl> <n> | shared -free <cl> | shared -delkey <cl> | shared <cl>\n");
    return 1;
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
    while (p != LNULL) {
        tItemM item = (tItemM) getItem(lista, p);
        if (item->address == CadenatoPointer(trozos[0])) {
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
            RemoveMemElement(&(L->MemList), p);
            return 0;
        }
        p = next(lista, p);
    }
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
    size_t cont;  
    ssize_t n;
    if (ntrozos<4){
        printf("Uso: readfile <df> <puntero> <num_bytes>\n");
        return 1;
    }
    p=CadenatoPointer(trozos[1]);  
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
    size_t cont;  
    ssize_t n;
    if (ntrozos<4){
        printf("Uso: writefile <df> <puntero> <num_bytes>\n");
        return 1;
    }
    p=CadenatoPointer(trozos[1]);
    if(p==NULL){
        fprintf(stderr,"Dirección inválida: %s\n", trozos[1]);
        return 1;
    }  
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
    if (ntrozos < 4){
        fprintf(stderr, "Uso: read <df> <addr> <num_bytes>\n");
        return 1;
    }

    if ((df=atoi(trozos[0]))<0) {
        fprintf(stderr, "Descriptor de archivo inválido: %s\n", trozos[0]);
        return 1;
    }
    void *p = CadenatoPointer(trozos[1]);
    if (p == NULL) {
        fprintf(stderr, "Dirección inválida: %s\n", trozos[1]);
        return 1;
    }

    size_t cont = (size_t) atoll(trozos[2]);
    if (cont == 0) {
        fprintf(stderr, "Número de bytes inválido: %s\n", trozos[2]);
        return 1;
    }

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
    if (ntrozos < 4) {
        fprintf(stderr, "Uso: write <df> <addr> <num_bytes>\n");
        return 1;
    }

    int df = (int) strtol(trozos[0], NULL, 10);
    if (df < 0) {
        fprintf(stderr, "Descriptor de archivo inválido: %s\n", trozos[0]);
        return 1;
    }

    void *p = CadenatoPointer(trozos[1]);
    if (p == NULL) {
        fprintf(stderr, "Dirección inválida: %s\n", trozos[1]);
        return 1;
    }

    size_t cont = (size_t) atoll(trozos[2]);
    if (cont == 0) {
        fprintf(stderr, "Número de bytes inválido: %s\n", trozos[2]);
        return 1;
    }

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

int Cmd_Mmap(char *trozos[], int ntrozos, Listas L)
{
   if(ntrozos==1){
         MList_print(MAPPED,L);
         return 0;  
   }
   if(ntrozos!=3){
        printf("Uso: mmap <fichero> <permisos>\n");
        return 1;
   }
   if(strcmp(trozos[0],"-free")==0){
        RemoveFileFromMmap(L,trozos[1]);
        return 0;
   }
   do_Mmap(trozos,L);
   return 0;
}

int Cmd_Memfill(char *trozos[], int ntrozos, Listas L) {
    if (ntrozos != 4) {
        printf("Uso: memfill <addr> <num_bytes> <byte_value>\n");
        return 1;
    }

    void *p = CadenatoPointer(trozos[0]);
    if (p == NULL) {
        fprintf(stderr, "Dirección inválida: %s\n", trozos[0]);
        return 1;
    }

    size_t cont = (size_t) atoll(trozos[1]);
    if (cont <=0) {
        fprintf(stderr, "Número de bytes inválido: %s\n", trozos[1]);
        return 1;
    }

    unsigned char byte = (unsigned char)(trozos[2][0]);

    LlenarMemoria(p, cont, byte);
    printf("Memoria en %p llenada con %zu bytes del valor 0x%02x\n", p, cont, byte);
    return 0;
}
int Cmd_memdump(char *trozos[], int NumTrozos, Listas L) {
    if (NumTrozos < 3) {
        printf("Uso: memdump <addr> <num_bytes>\n");
        return 1  ;
    }
    unsigned long long addr_val = strtoull(trozos[0], NULL, 16);
    if (addr_val == 0) {
        fprintf(stderr, "Dirección inválida: %s\n", trozos[0]);
        return 1;
    }

    unsigned char *address = (unsigned char *) addr_val;

    if(address == NULL) {
        fprintf(stderr, "Dirección inválida: %s\n", trozos[0]);
        return 1;
    }
    int bytes = atoi(trozos[1]);
    if (bytes < 0) {
        fprintf(stderr, "Número de bytes inválido: %s\n", trozos[1]);
        return 1;
    }
    int i = 0, j = 0;

    while (i <= bytes) {
        for (int k = 0; k < 16 && i <= bytes; i++, k++) {
            unsigned char c = address[i];
            switch (c) {
               case '\n': printf(" \\n");
                    break;
                case '\t': printf(" \\t");
                    break;
                case '\r': printf(" \\r");
                    break; 
                case '\\': printf(" \\\\");
                    break; 
                case '\'': printf(" \\\'");
                    break; 
                case '\"': printf(" \\\"");
                    break; 
                case 0: printf("   ");
                    break; 
                default:
                    if (c >= 32 && c <= 126) printf("%3c", c);
                    else{
                        printf("   ");
                    }
            }
        }
        printf("\n");
        for (int k = 0; k < 16 && j <= bytes; j++, k++) printf(" %02x", address[j]);
        printf("\n");
    }
    return 0;
}


int Cmd_malloc(char *trozos[], int ntrozos, Listas L) {
    if (ntrozos == 1) 
    {
        MList_print(MALLOC, L);
        return 0;
    }
    if(strcmp(trozos[0], "-free") == 0 && ntrozos == 3) {
        Remove_malloc((size_t) atoll(trozos[1]), L);
        return 0;
    }
    if(ntrozos ==2){
        size_t cont = (size_t) atoll(trozos[0]);
        if (cont == 0) {
            fprintf(stderr, "Número de bytes inválido: %s\n", trozos[0]);
            return 1;
        }
        void *p = malloc(cont);
        if (p == NULL) {
            fprintf(stderr, "Imposible asignar memoria malloc: %s\n", strerror(errno));
            return 1;
        }
        Aux_add_malloc_block(p,cont, L);
        printf("Memoria malloc asignada en %p (%zu bytes)\n", p, cont);
        return 0;
    }
    printf("Uso: malloc <num_bytes> | malloc -free <addr> | malloc\n");
    return 1;

}