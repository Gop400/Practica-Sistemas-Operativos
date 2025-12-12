#define TAMANO 128
#define MAXVAR 1024
#include "aux.h"



int externia, externia2, externia3;
int externa=1,externb=2, externc=3;
struct cmd cmds[]={{"jobs", jobs},{"deljobs", deljobs},{"exec",Cmd_exec},{"fork",Cmd_fork},{"showenv",Cmd_showenv},{"envvar",Cmd_envvar},{"uid",Cmd_uid},{NULL,NULL}};

int TrocearCadena(char * cadena, char * trozos[])
{ int i=1;
    if ((trozos[0]=strtok(cadena," \n\t"))==NULL)
        return 0;

    while ((trozos[i]=strtok(NULL," \n\t"))!=NULL)
        i++;
    return i;
}



int ProcesarEntrada(char *trozos[],int ntrozos,Listas L, char *env[]) {
    int i;
    for(i=0;cmds[i].name!=NULL;i++) {
        if((strcmp(cmds[i].name,trozos[0]))==0) {
            cmds[i].func(trozos+1,ntrozos,L, env);
            return 0;
        }
    }if(strcmp(trozos[0],"quit")==0 || strcmp(trozos[0],"exit")==0 ||strcmp(trozos[0],"bye")==0) {
        return 1;
    }
    ejecutarComando(trozos,ntrozos,L);
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
        fprintf(stderr,"No hay ficheros abiertos:%su\n",strerror(errno));
        return;
    }
    tPos p = first(F);
    tItemF file;
    while (p != LNULL) {
        modos[0] = '\0';               // vaciar el buffer al inicio de cada iteración
        file = (tItemF)getItem(F, p);   // obtener el archivo actual
        CrearCharModos(modos, file->modos); // convertir los modos a cadena
        printf("Descriptor: %d ,offset :(%s)-> %s %s\n",file->df,(file->offset == -1) ? "" : ({ static char buf[32]; sprintf(buf, "%ld", file->offset); buf; }),file->name,modos);

        p = next(F, p);                // pasar al siguiente elemento
    }
}

void initOpenList(Listas L){
    AnadirAFicherosAbiertos(&L->OpenFilesList,0,O_RDWR,"entrada estandar");
    AnadirAFicherosAbiertos(&L->OpenFilesList,1,O_RDWR,"salida estandar");
    AnadirAFicherosAbiertos(&L->OpenFilesList,2,O_RDWR,"error estandar");
}

void Recursiva (int n)
{
  char automatico[TAMANO];
  static char estatico[TAMANO];

  printf ("parametro:%3d(%p) array %p, arr estatico %p\n",n,&n,automatico, estatico);

  if (n>0)
    Recursiva(n-1);
}


void LlenarMemoria (void *p, size_t cont, unsigned char byte)
{
  unsigned char *arr=(unsigned char *) p;
  size_t i;

  for (i=0; i<cont;i++)
		arr[i]=byte;
}

void * ObtenerMemoriaShmget (key_t clave, size_t tam, Listas L)
{
    void * p;
    int aux,id,flags=0777; /*los 9 bits menos significativos de los flags:permisos*/
    struct shmid_ds s;

    if (tam)     /*tam distito de 0 indica crear */
        flags=flags | IPC_CREAT | IPC_EXCL; /*cuando no es crear pasamos de tamano 0*/
    if (clave==IPC_PRIVATE)  /*no nos vale*/
        {errno=EINVAL; return NULL;}
    if ((id=shmget(clave, tam, flags))==-1)
        return (NULL);
    if ((p=shmat(id,NULL,0))==(void*) -1){
        aux=errno;
        if (tam)
             shmctl(id,IPC_RMID,NULL);
        errno=aux;
        return (NULL);
    }
    shmctl (id,IPC_STAT,&s); 
    tItemM m=(tItem) malloc (sizeof (struct structMem));
    m->address=p;
    m->size=s.shm_segsz;
    m->time=time(NULL);
    m->alloc=SHARED;
    m->smb_key=clave;
    strcpy (m->file_name,"-");
    m->file_desc=-1;
    InsertItem (&L->MemList,m,NULL);
    return (p);
}
void do_SharedCreate (char *tr[], Listas L)
{
   key_t cl;
   size_t tam;
   void *p;

   if (tr[0]==NULL || tr[1]==NULL) {
		MList_print(SHARED,L);
		return;
   }
  
   cl=(key_t)  strtoul(tr[0],NULL,10);
   tam=(size_t) strtoul(tr[1],NULL,10);
   if (tam==0) {
	printf ("No se asignan bloques de 0 bytes\n");
	return;
   }
   if ((p=ObtenerMemoriaShmget(cl,tam,L))!=NULL)
		printf ("Asignados %lu bytes en %p\n",(unsigned long) tam, p);
   else
		printf ("Imposible asignar memoria compartida clave %lu:%s\n",(unsigned long) cl,strerror(errno));
}

void do_Shared (char *tr[], Listas L)
{
   key_t cl;
   void *p;

   if (tr[0]==NULL) {
		MList_print(SHARED,L);
		return;
   }
  
   cl=(key_t)  strtoul(tr[0],NULL,10);

   if ((p=ObtenerMemoriaShmget(cl,0,L))!=NULL)
		printf ("Asignada memoria compartida de clave %lu en %p\n",(unsigned long) cl, p);
   else
		printf ("Imposible asignar memoria compartida clave %lu:%s\n",(unsigned long) cl,strerror(errno));
}

void doSharedFree (key_t cl, Listas L)
{
    void *p;                                 /*llamo a la funcion de la lista*/
                                             /*que devuelve la direccion donde*/
    if ((p=DireccionNodoShared (L,cl))==NULL){/*esta la memoria de clave cl*/
                printf ("No hay bloque de esa clave mapeado en el proceso\n");
                return;
    }
   shmdt(p);
   if (EliminarNodoDireccion (L,p)==-1)  /*elimino el nodo */
        printf ("Imposible quitar de lista: %s\n",strerror(errno));
}


void * MapearFichero (char * fichero, int protection,Listas L)
{
    int df, map=MAP_PRIVATE,modo=O_RDONLY;
    struct stat s;
    void *p;

    if (protection&PROT_WRITE)
          modo=O_RDWR;
    if (stat(fichero,&s)==-1 || (df=open(fichero, modo))==-1)
          return NULL;
    if(s.st_size==0){ /*no se puede mapear un fichero de tamaño 0 */
        close (df);
        fprintf(stderr, "No se puede mapear un fichero vacío\n");
        return NULL;
    }
    if ((p=mmap (NULL,s.st_size, protection,map,df,0))==MAP_FAILED)
           return NULL;
    tItemM m=(tItem) malloc(sizeof(struct structMem));
    m->address =p;
    m->size = s.st_size;
    m->time = time(NULL);
    m->alloc = MAPPED;
    m->file_desc = df;
    strncpy(m->file_name, fichero, 1024);
    InsertItem (&L->MemList,m,NULL);
/* Guardar en la lista    InsertarNodoMmap (&L,p, s.st_size,df,fichero); */
/* Gurdas en la lista de descriptores usados df, fichero*/
    return p;
}

void do_Mmap(char *arg[],Listas L)
{ 
     char *perm;
     void *p;
     int protection=0;
     
     if (arg[0]==NULL)
            {MList_print(MAPPED,L); return;}
     if ((perm=arg[1])!=NULL && strlen(perm)<4) {
            if (strchr(perm,'r')!=NULL) protection|=PROT_READ;
            if (strchr(perm,'w')!=NULL) protection|=PROT_WRITE;
            if (strchr(perm,'x')!=NULL) protection|=PROT_EXEC;
     }
     if ((p=MapearFichero(arg[0],protection,L))==NULL)
             perror ("Imposible mapear fichero");
     else
             printf ("fichero %s mapeado en %p\n", arg[0], p);
}
void RemoveFileFromMmap(Listas L, char *filename)
{
   if(isEmptyList(L->MemList)) {
        perror("No hay archivos mapeados.\n");
        return;
    }
    tPos p = first(L->MemList);
    while (p != LNULL) {
        tItemM item = (tItemM) getItem(L->MemList, p);
        if (strcmp(item->file_name, filename) == 0) {
            // Desmapear el archivo
            if (munmap(item->address, item->size) == -1) {
                perror("Imposible desmapear el archivo");
                return;
            }
            // Cerrar el descriptor de archivo
            close(item->file_desc);
            // Eliminar el nodo de la lista
            RemoveMemElement(&L->MemList, p);
            printf("Archivo mapeado %s desmapeado y eliminado de la lista.\n", filename);
            return;
        }
        p = next(L->MemList, p);
    }
    return;
}

void do_SharedDelkey (char *args[])
{
   key_t clave;
   int id;
   char *key=args[0];

   if (key==NULL || (clave=(key_t) strtoul(key,NULL,10))==IPC_PRIVATE){
        printf ("      delkey necesita clave_valida\n");
        return;
   }
   if ((id=shmget(clave,0,0666))==-1){
        perror ("shmget: imposible obtener memoria compartida");
        return;
   }
   if (shmctl(id,IPC_RMID,NULL)==-1)
        perror ("shmctl: imposible eliminar memoria compartida\n");
}

void * CadenatoPointer (char * s)
{
  void *p;
  sscanf(s,"%p",&p);
  if (p==NULL)
      errno=EFAULT;
  return p;
}

ssize_t LeerFichero (char *f, void *p, size_t cont)
{
   struct stat s;
   ssize_t  n;  
   int df,aux;

   if (stat (f,&s)==-1 || (df=open(f,O_RDONLY))==-1)
	return -1;     
   if (cont==-1)   /* si pasamos -1 como bytes a leer lo leemos entero*/
	cont=s.st_size;
   if ((n=read(df,p,cont))==-1){
	aux=errno;
	close(df);
	errno=aux;
	return -1;
   }
   close (df);
   return n;
}

void Do_pmap (void) /*sin argumentos*/
 { pid_t pid;       /*hace el pmap (o equivalente) del proceso actual*/
   char elpid[32];
   char *argv[4]={"pmap",elpid,NULL};
   
   sprintf (elpid,"%d", (int) getpid());
   if ((pid=fork())==-1){
      perror ("Imposible crear proceso");
      return;
      }
   if (pid==0){
      if (execvp(argv[0],argv)==-1)
         perror("cannot execute pmap (linux, solaris)");
         
      argv[0]="procstat"; argv[1]="vm"; argv[2]=elpid; argv[3]=NULL;   
      if (execvp(argv[0],argv)==-1)/*No hay pmap, probamos procstat FreeBSD */
         perror("cannot execute procstat (FreeBSD)");
         
      argv[0]="procmap",argv[1]=elpid;argv[2]=NULL;    
            if (execvp(argv[0],argv)==-1)  /*probamos procmap OpenBSD*/
         perror("cannot execute procmap (OpenBSD)");
         
      argv[0]="vmmap"; argv[1]="-interleave"; argv[2]=elpid;argv[3]=NULL;
      if (execvp(argv[0],argv)==-1) /*probamos vmmap Mac-OS*/
         perror("cannot execute vmmap (Mac-OS)");      
      exit(1);
  }
  waitpid (pid,NULL,0);
}




ssize_t EscribirDesdeDescriptor(int df, void *p, size_t cont)
{
    if (df < 0 || p == NULL || cont == 0) {
        errno = EINVAL;
        return -1;
    }

    ssize_t n;
    int aux;

    if ((n = write(df, p, cont)) == -1) {
        aux = errno;
        errno = aux;
        return -1;
    }

    return n;
}

void Aux_add_malloc_block(void *p, size_t size, Listas L) {
    tItemM m = (tItem) malloc(sizeof(struct structMem));
    m->address = p;
    m->size = size;
    m->time = time(NULL);
    m->alloc = MALLOC;
    strcpy(m->file_name, "-");
    m->file_desc = -1;
    InsertItem(&L->MemList, m, NULL);
    return;
}
void Remove_malloc(size_t size, Listas L) {
    if(isEmptyList(L->MemList)) {
        fprintf(stderr, "No hay bloques malloc en la lista.\n");
        return;
    }
    tPos p = first(L->MemList);
    while (p != LNULL) {    
        tItemM item = (tItemM) getItem(L->MemList, p);
        if (item->size == size && item->alloc == MALLOC) {
            free(item->address);
            RemoveMemElement(&L->MemList, p);
            printf("Bloque malloc de tamaño %zu eliminado y liberado.\n", size);
            return;
        }
        p = next(L->MemList, p);
    }
    fprintf(stderr, "No se encontró un bloque malloc de tamaño %zu en la lista.\n", size);

    return;
}

ssize_t LeerDesdeDescriptor(int df, void *p, size_t cont)
{
    if (df < 0 || p == NULL || cont == 0) {
        errno = EINVAL;
        return -1;
    }

    ssize_t n;
    int aux;

    if ((n = read(df, p, cont)) == -1) {
        aux = errno;
        errno = aux;
        return -1;
    }

    return n;
}


ssize_t EscribirFichero( char *f, void *p, size_t cont)
{
    ssize_t n;
    int df, aux;

    if (f == NULL || p == NULL || cont == 0) {
        errno = EINVAL;
        return -1;
    }

    if ((df = open(f, O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1)
        return -1;

    if ((n = write(df, p, cont)) == -1) {
        aux = errno;
        close(df);
        errno = aux;
        return -1;
    }

    close(df);
    return n;
}


int EliminarNodoDireccion(Listas L, void *dir)
{
    tPos p = first(L->MemList);

    while (p != LNULL) {

        tItemM item = (tItemM) getItem(L->MemList, p);
        if (item->address == dir) {

            RemoveMemElement(&L->MemList, p);

            return 0;  // éxito
        }

        p = next(L->MemList, p);
    }

    errno = ENOENT;   
    return -1;
}


void* DireccionNodoShared(Listas L, key_t cl)
{
    ;
    tPos p = first(L->MemList);

    while (p != NULL) {

        tItemM item = (tItemM) getItem(L->MemList, p);

        if (item->alloc == SHARED && item->smb_key == cl)
            return item->address;

        p = next(L->MemList, p);
    }

    return NULL;
}


void MList_print(enum tAllocL tipo,Listas L) {
    tPos p = first(L->MemList);

    if (isEmptyList(L->MemList)) {
        printf("No hay bloques de memoria.\n");
        return;
    }

    printf("%-18s %-10s %-20s %-10s %-20s\n",
           "ADDRESS", "SIZE", "TIME", "ALLOC", "KEY/FICHERO");

    printf("-------------------------------------------------------------------------\n");

    while (p != NULL) {

        tItemM item = (tItemM) getItem(L->MemList, p);

        // Solo imprime si coincide el tipo
        if (item->alloc == tipo) {

            char fecha[64];
            struct tm *tminfo = localtime(&item->time);
            strftime(fecha, sizeof(fecha), "%Y-%m-%d %H:%M:%S", tminfo);
            char info[1024];

            if (item->alloc == MAPPED) {
                snprintf(info, sizeof(info), "%s", item->file_name);

            } else if (item->alloc == SHARED) {
                snprintf(info, sizeof(info), "%d", item->smb_key);

            } else {
                snprintf(info, sizeof(info), "--");
            }

            printf("%-18p %-10d %-20s %-10s %-20s\n",
                   item->address,
                   item->size,
                   fecha,
                   (item->alloc == SHARED ? "SHARED" :
                    item->alloc == MALLOC ? "MALLOC" : 
                                            "MAPPED"),
                   info
            );
        }

        p = next(L->MemList, p);
    }
}


void Aux_mem_funcs() {
    printf("Funciones programa\t\t%p,\t%p,\t%p\n", MList_print, do_Shared, Addhistoric);
    printf("Funciones libreria\t\t%p,\t%p,\t%p\n", printf, getpid, wait);
}

void Aux_mem_vars() {
    
    static int statica = 1, staticb = 2, staticc = 3;
    static int staticia, staticib, staticicc;
    auto int autoa = 1, autob = 2, autoc = 3;
    printf("Variables (N.I.) externas\t\t%p,\t%p,\t%p\n", &externia, &externia2, &externia3);
    printf("Variables externas\t%p,\t%p,\t%p\n", &externa, &externb, &externc);
    printf("Variables estáticas\t\t%p,\t%p,\t%p\n", &staticia, &staticib, &staticicc);
    printf("Variables (N.I.) estáticas\t%p,\t%p,\t%p\n", &statica, &staticb, &staticc);
    printf("Variables automáticas\t\t%p,\t%p,\t%p\n", &autoa, &autob, &autoc);
}

void Aux_mem_blocks(Listas L) {
    MList_print(MALLOC, L);
    printf("\n");
    MList_print(SHARED, L);
    printf("\n");
    MList_print(MAPPED, L);
}
bool PerteneceMemList(void *addr, Listas L) {
    tPos p = first(L->MemList);

    while (p != LNULL) {
        tItemM item = (tItemM) getItem(L->MemList, p);
        if (addr >= item->address && addr < (item->address + item->size)) {
            return true;  
        }
        p = next(L->MemList, p);
    }

    return false; 
}



int BuscarVariable (char * var, char *e[])  /*busca una variable en el entorno que se le pasa como parÃ¡metro*/
{                                           /*devuelve la posicion de la variable en el entorno, -1 si no existe*/
  int pos=0;
  char aux[MAXVAR];
  
  strcpy (aux,var);
  strcat (aux,"=");
  
  while (e[pos]!=NULL)
    if (!strncmp(e[pos],aux,strlen(aux)))
      return (pos);
    else 
      pos++;
  errno=ENOENT;   /*no hay tal variable*/
  return(-1);
}


int CambiarVariable(char * var, char * valor, char *e[]) /*cambia una variable en el entorno que se le pasa como parÃ¡metro*/
{                                                        /*lo hace directamente, no usa putenv*/
  int pos;
  char *aux;
   
  if ((pos=BuscarVariable(var,e))==-1)
    return(-1);
 
  if ((aux=(char *)malloc(strlen(var)+strlen(valor)+2))==NULL)
	return -1;
  strcpy(aux,var);
  strcat(aux,"=");
  strcat(aux,valor);
  e[pos]=aux;
  return (pos);
}

/*las siguientes funciones nos permiten obtener el nombre de una senal a partir
del nÃºmero y viceversa */
static struct SEN sigstrnum[]={   
	{"HUP", SIGHUP},
	{"INT", SIGINT},
	{"QUIT", SIGQUIT},
	{"ILL", SIGILL}, 
	{"TRAP", SIGTRAP},
	{"ABRT", SIGABRT},
	{"IOT", SIGIOT},
	{"BUS", SIGBUS},
	{"FPE", SIGFPE},
	{"KILL", SIGKILL},
	{"USR1", SIGUSR1},
	{"SEGV", SIGSEGV},
	{"USR2", SIGUSR2}, 
	{"PIPE", SIGPIPE},
	{"ALRM", SIGALRM},
	{"TERM", SIGTERM},
	{"CHLD", SIGCHLD},
	{"CONT", SIGCONT},
	{"STOP", SIGSTOP},
	{"TSTP", SIGTSTP}, 
	{"TTIN", SIGTTIN},
	{"TTOU", SIGTTOU},
	{"URG", SIGURG},
	{"XCPU", SIGXCPU},
	{"XFSZ", SIGXFSZ},
	{"VTALRM", SIGVTALRM},
	{"PROF", SIGPROF},
	{"WINCH", SIGWINCH}, 
	{"IO", SIGIO},
	{"SYS", SIGSYS},
/*senales que no hay en todas partes*/
#ifdef SIGPOLL
	{"POLL", SIGPOLL},
#endif
#ifdef SIGPWR
	{"PWR", SIGPWR},
#endif
#ifdef SIGEMT
	{"EMT", SIGEMT},
#endif
#ifdef SIGINFO
	{"INFO", SIGINFO},
#endif
#ifdef SIGSTKFLT
	{"STKFLT", SIGSTKFLT},
#endif
#ifdef SIGCLD
	{"CLD", SIGCLD},
#endif
#ifdef SIGLOST
	{"LOST", SIGLOST},
#endif
#ifdef SIGCANCEL
	{"CANCEL", SIGCANCEL},
#endif
#ifdef SIGTHAW
	{"THAW", SIGTHAW},
#endif
#ifdef SIGFREEZE
	{"FREEZE", SIGFREEZE},
#endif
#ifdef SIGLWP
	{"LWP", SIGLWP},
#endif
#ifdef SIGWAITING
	{"WAITING", SIGWAITING},
#endif
 	{NULL,-1},
	};    /*fin array sigstrnum */


int ValorSenal(char * sen)  /*devuelve el numero de senial a partir del nombre*/ 
{ 
  int i;
  for (i=0; sigstrnum[i].nombre!=NULL; i++)
  	if (!strcmp(sen, sigstrnum[i].nombre))
		return sigstrnum[i].senal;
  return -1;
}


char *NombreSenal(int sen)  /*devuelve el nombre senal a partir de la senal*/ 
{			/* para sitios donde no hay sig2str*/
 int i;
  for (i=0; sigstrnum[i].nombre!=NULL; i++)
  	if (sen==sigstrnum[i].senal)
		return sigstrnum[i].nombre;
 return ("SIGUNKNOWN");
}
void Aux_processos_show(char **env, char *nombre_entorno) {
    int i = 0;

    while (env[i] != NULL) {
        printf("%p->%s[%d]=(%p) %s\n", &env[i],
               nombre_entorno, i, env[i], env[i]);
        i++;
    }
}
