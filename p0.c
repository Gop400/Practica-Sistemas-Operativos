
#include "p0.h"
#include "aux.h"

int authors(char *trozos[],int ntrozos,Listas L) {
    if(ntrozos==2 && strcmp(trozos[0],"-l")==0) {
        printf("login del autor1:g.oliveira@udc.es\n");
        printf("login del autor2:samuel.mouriño@udc.es\n");
        return 0;
    }if(ntrozos==2 && strcmp(trozos[0],"-n")==0) {
        printf("Nombre del autor1:Gabriel Oliveira Pais\n");
        printf("Nombre del autor2:Samuel Mouriño\n");
        return 0;
    }if(ntrozos==1){
        printf("Nombre del autor1:Gabriel Oliveira Pais\nLogin del autor1:g.oliveira@udc.es\n");
        printf("Nombre del autor2:Samuel Mouriño\nLogin del autor2:samuel.mouriño@udc.es\n");
        return 0;
    }
    perror("Argumento invalido");
    return 1;
}

int pid(char*trozos[],int ntrozos,Listas L) {
    if(ntrozos==1) {
        printf("%d\n",getpid());
        return 0;
    }if(ntrozos==2 && strcmp(trozos[0],"-p")==0){
        printf("%d\n",getppid());
        return 0;
    }
    perror("Argumento invalido");
    return 1;
}


int infosys(char *trozos[],int ntrozos,Listas L) {
    struct utsname infoS;
    if(uname(&infoS)==0 && ntrozos==1) {
        printf("Nombre del sistema: %s\n", infoS.sysname);
        printf("Nombre del nodo: %s\n", infoS.nodename);
        printf("Versión del sistema: %s\n", infoS.release);
        printf("Versión de la máquina: %s\n", infoS.version);
        printf("Nombre de la máquina: %s\n", infoS.machine);
        return 0;
    }
    perror("Argumento invalido o imposible obtener info del sistema");
    return 1;
}

int cmd_getcwd(char*trozos[],int ntrozos,Listas L) {
    if(ntrozos==1) {
        char cwd[4096];
        if(getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("Directorio actual:%s\n",cwd);
            return 0;
        }
        perror("Error al obtener el directorio actual");

        return 1;
    }
    perror("Argumento invalido");
    return 1;
}

int cmd_cd(char*trozos[],int ntrozos,Listas L) {
    if(ntrozos==2) {
        if(trozos[0]!=NULL && chdir(trozos[0])==0){
            printf("Directorio actualizado\n");
            return 0;
        }
        perror("Directorio invalido");
        return 1;
    }if(ntrozos==1) {
        char cwd[4096];
        if(getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("Directorio actual:%s\n",cwd);
            return 0;
        }
        perror("Error al obtener el directorio actual");

        return 1;
    }
    perror("Argumentos invalidos");
    return 1;
}

int date(char *trozos[],int ntrozos,Listas L) {
    time_t t;
    struct tm *tm;
    time(&t);
    tm=localtime(&t);

    if(ntrozos==2) {
        if(strcmp(trozos[0],"-d")==0) {
            printf("%02d/%02d/%d\n",tm->tm_mday,tm->tm_mon+1,tm->tm_year+1900);
            return 0;
        }if(strcmp(trozos[0],"-t")==0) {
            printf("%02d:%02d:%02d\n",tm->tm_hour,tm->tm_min,tm->tm_sec);
            return 0;
        }
        perror("Argumento invalido");
        return 1;
    }if(ntrozos==1) {
        printf("%02d/%02d/%d\n%02d:%02d:%02d\n",tm->tm_mday,tm->tm_mon+1,tm->tm_year+1900,tm->tm_hour,tm->tm_min,tm->tm_sec);
        return 0;
    }
    perror("Argumentos invalidos");
    return 1;
}


int hour(char *trozos[],int ntrozos,Listas L){
    time_t t;
    struct tm *tm;
    time(&t);
    tm=localtime(&t);
    if(ntrozos==1){
        printf("%02d:%02d:%02d\n",tm->tm_hour,tm->tm_min,tm->tm_sec);
        return 0;
    }
    perror("Argumento invalidos");
    return 1;
}


int help(char *trozos[], int ntrozos, Listas L){
    if(ntrozos==2) {
        if(strcmp(trozos[0],"autores")==0) {
            printf("authors [-n|-l]	Muestra los nombres y/o logins de los autores");
            return 0;
        } if(strcmp(trozos[0],"pid")==0) {
            printf("pid [-p] Muestra el pid del shell o de su proceso padre\n");
            return 0;
        } if(strcmp(trozos[0],"historic")==0) {
            printf("historic [-clear|-count|-N|N] Muestra (o borra)el historico de comandos\n-clear: borra el historico\n-count: dimension del historico\n-N: muestra los N ultimos\nN: repite el comando N\n");
            return 0;
        } if(strcmp(trozos[0],"open")==0) {
            printf("open fich m1 m2... Abre el fichero fich y lo anade a la lista de ficheros abiertos del shell\nm1, m2..es el modo de apertura (or bit a bit de los siguientes)\ncr: O_CREAT     ap: O_APPEND\nex: O_EXCL      ro: O_RDONLY\nrw: O_RDWR      wo: O_WRONLY\ntr: O_TRUNC\n");
            return 0;
        } if(strcmp(trozos[0],"close")==0) {
            printf("close df        Cierra el descriptor df y elimina el correspondiente fichero de la lista de ficheros abiertos\n");
            return 0;
        } if(strcmp(trozos[0],"dup")==0) {
            printf("dup df  Duplica el descriptor de fichero df y anade una nueva entrada a la lista ficheros abiertos\n");
            return 0;
        } if(strcmp(trozos[0],"quit")==0) {
            printf("quit:Termina la ejecucion del shell\n");
            return 0;
        } if(strcmp(trozos[0],"exit")==0) {
            printf("exit: Termina la ejecucion del shell\n");
            return 0;
        } if(strcmp(trozos[0],"bye")==0) {
            printf("bye:Termina la ejecucion del shell\n");
            return 0;
        } if(strcmp(trozos[0],"infosys")==0) {
            printf("infosys :Muestra informacion de la maquina donde corre el shell\n");
            return 0;
        } if(strcmp(trozos[0],"help")==0) {
            printf("help [cmd]:Muestra y proporciona ayuda sobre los comandos disponibles\ncmd: info sobre el comando cmd\n");
            return 0;
        } if(strcmp(trozos[0],"date")==0) {
            printf("date [-d|-t]:Muestra la fecha y/o la hora actual\n");
            return 0;
        }
        perror("Error,comando no pertenece a la lista help\n");
        return 1;
    }else if(ntrozos==1) {
        printf("help [cmd] ayuda sobre comandos\nComandos disponibles:autores,pid,ppid,cd,date,historic,help,open,close,dup,infosys,quit,exit,bye\n");
        return 0;
    }
    return 1;
}


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
                deleteList(&L->HistoricList);
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
            RemoveElement(&L->HistoricList,last(L->HistoricList));
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
    if (ntrozos==1){
        perror("Introduzca un descriptor de fichero a cerrar");
        return 1;
    }
    if((df=atoi(trozos[0]))<0 && ntrozos==2){
        perror("Descriptor de fichero invalido");
        return 1;
    }
    if(ntrozos>2){
       perror("Demasiados argumentos");
       return 1; 
    }
    tPos p=first(L->OpenFilesList);
        tItemF file;
        while(p!=LNULL){
            file=(tItemF)getItem(L->OpenFilesList,p);
            if(file->df==df){
                if(close(df)==-1){
                    perror("Error al cerrar el fichero");
                    return 1;
                }
                RemoveElement(&L->OpenFilesList,p);
                printf("Descriptor %d cerrado y eliminado de la lista\n",df);
                return 0;
            }
            p=next(L->OpenFilesList,p);
        } 
    return 1;
}
int listopen(char *trozos[],int ntrozos,Listas L){
    if(ntrozos==1){
        PrintOpenFiles(L->OpenFilesList);
        return 0;
    }
    perror("Numero de Argumentos invalidos");
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