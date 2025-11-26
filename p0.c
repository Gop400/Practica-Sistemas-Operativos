
#include "p0.h"
#include "aux.h"


void PrintAllHistoric(tList l) {
    tPos p;
    tItemH d;
    if(isEmptyList(l)) {
        perror("error,la lista historic esta vacia");
        return ;
    }
    p=first(l);
    while(p!=NULL) {
        d=(tItemH)getItem(l,p);
        if(d!=NULL) {
            printf("%d.%s",d->id,d->comando);
            p=p->next;
            continue;
        }
        printf("error");
        
    }
    return ;
}
void PrintNHistoric(tList l,long n) {
    tPos p;
    if(isEmptyList(l)) {
        perror("Error la lista historic esta vacia\n");
        return;
    }
    long i=0;
    p=last(l);
    while(i<n) {
        if(p!=LNULL) {
            printf("%d->%s\n",((tItemH)getItem(l,p))->id,((tItemH)getItem(l,p))->comando);
            p=previous(l, p);
            i++;
        }else {
            break;
        }
    };
}
void Ncmd(long n,Listas L) {
    char comando[1024];
    int ntrozos_aux=0;
    char *args[64];
   
    if(isEmptyList(L->HistoricList)){
        fprintf(stderr,"lista historic vacia\n");
        return;
    }
    tPos p=first(L->HistoricList);
    tItemH item=getItem(L->HistoricList,p);
    while(p!=LNULL) {
        item=getItem(L->HistoricList,p);
        if(item->id==n){break;}
        if(p==last(L->HistoricList)){
            item=LNULL;
            break;
        }
        p=p->next;
    }if(item!=LNULL) {
        strcpy(comando,item->comando);
        comando[sizeof(comando) - 1] = '\0'; // asegurar terminador
        ntrozos_aux = TrocearCadena(comando, args);
        ProcesarEntrada(args,ntrozos_aux,L);
        return ;
    }
    perror("Elemento n mayor que ultimo en la lista");
    return;
}

int historic(char *trozos[],int ntrozos,Listas L) {
    char *endnptr;
    long n;
    if(ntrozos==2) {
        if(trozos[0][0]=='-') {
            if(strcmp(trozos[0]+1,"clear")==0){
                deleteList(&L->HistoricList,'H');
                if(isEmptyList(L->HistoricList)){
                    printf("lista vacia\n");
                    return 0;
                }
                perror("error al vaciar lista");
                return 1;
            }
            if(strcmp(trozos[0]+1,"count")==0){
                printf("el tamaño del historic list es de %d\n",sizeList(L->HistoricList));
                return 0;
            }
            n=strtol(trozos[0]+1,&endnptr,10);
            if(endnptr!=trozos[0]+1 && n>=0){
                PrintNHistoric(L->HistoricList,n);
                return 0;
            }
            perror("Error,valor -N invalido\n");
            return 1;
        }
        n=strtol(trozos[0],&endnptr,10);
        if(endnptr!=trozos[0] && n>=0) {
            RemoveHistoricElement(&L->HistoricList,last(L->HistoricList));
            Ncmd(n,L);
            return 0;
        }
        perror("Error,valor -N invalido");
        return 1;
    }if(ntrozos==1){
        PrintAllHistoric(L->HistoricList);
        return 0;
    }
    return 1;
}


int Cmd_open (char * trozos[],int ntrozos,Listas L){
    int i,df, mode=0;
    
    if (trozos[0]==NULL && ntrozos==1) { /*no hay parametro*/
        PrintOpenFiles(L->OpenFilesList);
        return 0;
    }
    for (i=1; trozos[i]!=NULL; i++){
        if (!strcmp(trozos[i],"cr")) mode|=O_CREAT;
        else if (!strcmp(trozos[i],"ex")) mode|=O_EXCL;
        else if (!strcmp(trozos[i],"ro")) mode|=O_RDONLY; 
        else if (!strcmp(trozos[i],"wo")) mode|=O_WRONLY;
        else if (!strcmp(trozos[i],"rw")) mode|=O_RDWR;
        else if (!strcmp(trozos[i],"ap")) mode|=O_APPEND;
        else if (!strcmp(trozos[i],"tr")) mode|=O_TRUNC; 
        else break;
        
    }
    if (!(mode & (O_RDONLY | O_WRONLY | O_RDWR))) {
        mode |= O_RDONLY;
    }
      
    if ((df=open(trozos[0],mode,0777))==-1){
            perror ("Imposible abrir fichero");
            return 1;
    }
    else{
        AnadirAFicherosAbiertos(&L->OpenFilesList,df,mode,trozos[0]);
        printf ("Anadida entrada a la tabla ficheros abiertos:fd=%d,nombre='%s'\n",df,trozos[0]);
        return 0;
    }
    return 1;
}


int Cmd_close (char *trozos[],int ntrozos,Listas L){ 
    int df;
    if (ntrozos<2){
        fprintf(stderr,"Introduzca un descriptor de fichero a cerrar\n");
        return 1;
    }
    if(ntrozos>2){
       perror("Demasiados argumentos");
       return 1; 
    }
    char *endptr;
    long df_long = strtol(trozos[0], &endptr, 10);
    if (*endptr != '\0' || df_long < 0) {
        fprintf(stderr, "Descriptor de fichero invalido\n");
        return 1;
    }
    df = (int)df_long;
   
    tPos p=first(L->OpenFilesList);
    tItemF file;
        while(p!=LNULL){
            file=(tItemF)getItem(L->OpenFilesList,p);
            if(file->df==df){
                if(close(df)==-1){
                    perror("Error al cerrar el fichero");
                    return 1;
                }
                RemoveOpenFileElement(&L->OpenFilesList,p);
                printf("Descriptor %d cerrado y eliminado de la lista\n",df);
                return 0;
            }
            p=next(L->OpenFilesList,p);
        } 
    fprintf(stderr, "Descriptor %d no encontrado en la lista\n", df);
    return 1;
}

int Cmd_dup (char *trozos[],int ntrozos,Listas L)
{ 
    int df, duplicado;
    char aux[64],*p;
    
    if (trozos[0]==NULL || (df=atoi(trozos[0]))<0) { /*no hay parametro*/
        PrintOpenFiles(L->OpenFilesList);   /*o el descriptor es menor que 0*/
        return 0;
    }

    p=NombreDescriptor(df,L);
    if(strcmp(p,"Not Found")==0){
        perror("Descriptor no encontrado en la lista de ficheros abiertos");
        return 1;
    }
     // Duplicar el descriptor con dup()
    if ((duplicado = dup(df)) == -1) {
        perror("Error al duplicar descriptor");
        return 1;
    }
    sprintf (aux,"dup %d (%s)",df, p);
    AnadirAFicherosAbiertos(&L->OpenFilesList,duplicado,fcntl(duplicado,F_GETFL),aux);
    printf ("Anadida entrada a la tabla ficheros abiertos:fd=%d,nombre='%s'\n",duplicado,aux);
    return 0;
}