#define TAMANO 1024

#include "aux.h"
#include "p0.h"
#include "p1.h"
#include "p2.h"

struct cmd cmds[]={{"dir",cmd_dir},{"setdirparams",setdirparams},{"lseek",Cmd_lseek},{"writestr",Cmd_writestr},{"delrec",Cmd_delrec},{"erase",Cmd_erase},{"dup",Cmd_dup},{"listopen",listopen},{"create",Cmd_create},{"close",Cmd_close},{"open",Cmd_open},{"historic",historic},{"help",help},{"date",date},{"authors",authors},{"pid",pid},{"infosys",infosys},{"getcwd",cmd_getcwd},{"cd",cmd_cd},{"hour",hour}};

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
char * ConvierteModo (mode_t m, char *permisos)
{
    strcpy (permisos,"---------- ");
    
    permisos[0]=LetraTF(m);
    if (m&S_IRUSR) permisos[1]='r';    /*propietario*/
    if (m&S_IWUSR) permisos[2]='w';
    if (m&S_IXUSR) permisos[3]='x';
    if (m&S_IRGRP) permisos[4]='r';    /*grupo*/
    if (m&S_IWGRP) permisos[5]='w';
    if (m&S_IXGRP) permisos[6]='x';
    if (m&S_IROTH) permisos[7]='r';    /*resto*/
    if (m&S_IWOTH) permisos[8]='w';
    if (m&S_IXOTH) permisos[9]='x';
    if (m&S_ISUID) permisos[3]='s';    /*setuid, setgid y stickybit*/
    if (m&S_ISGID) permisos[6]='s';
    if (m&S_ISVTX) permisos[9]='t';
    
    return permisos;
}

char LetraTF (mode_t m)
{
     switch (m&S_IFMT) { /*and bit a bit con los bits de formato,0170000 */
        case S_IFSOCK: return 's'; /*socket */
        case S_IFLNK: return 'l'; /*symbolic link*/
        case S_IFREG: return '-'; /* fichero normal*/
        case S_IFBLK: return 'b'; /*block device*/
        case S_IFDIR: return 'd'; /*directorio */ 
        case S_IFCHR: return 'c'; /*char device*/
        case S_IFIFO: return 'p'; /*pipe*/
        default: return '?'; /*desconocido, no deberia aparecer*/
     }
}
void initOpenList(Listas L){
    AnadirAFicherosAbiertos(&L->OpenFilesList,0,O_RDWR,"entrada estandar");
    AnadirAFicherosAbiertos(&L->OpenFilesList,1,O_RDWR,"salida estandar");
    AnadirAFicherosAbiertos(&L->OpenFilesList,2,O_RDWR,"error estandar");
}


char* GetDirParamsString(DirFormat f, LinkOption l, HiddenOption h, RecursionOption r) {
    const char *format, *link, *hid, *rec;
    

    // Convertir cada enum a texto (en la misma función)
    switch (f) {
        case SHORT_FORMAT: format = "corto"; break;
        case LONG_FORMAT:  format = "largo"; break;
        default: format = "unknown";
    }

    switch (l) {
        case NO_LINK: link = "sin link"; break;
        case LINK:    link = "con link"; break;
        default: link = "unknown";
    }

    switch (h) {
        case NO_HID: hid = "con archivos ocultos"; break;
        case HID:    hid = "sin archivos ocultos"; break;
        default: hid = "unknown";
    }

    switch (r) {
        case NO_REC: rec = "no recursivo"; break;
        case RECA:   rec = "recursivo(despues)"; break;
        case RECB:   rec = "recursivo(antes)"; break;
        default: rec = "unknown";
    }

    // Reservar memoria para la cadena final
    char *result = malloc(128);
    if (!result) return NULL;

    snprintf(result, 128, "Listado %s %s %s %s", format, link, hid, rec);
    return result; // llamador debe hacer free(result)
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


void * MapearFichero (char * fichero, int protection)
{
    int df, map=MAP_PRIVATE,modo=O_RDONLY;
    struct stat s;
    void *p;

    if (protection&PROT_WRITE)
          modo=O_RDWR;
    if (stat(fichero,&s)==-1 || (df=open(fichero, modo))==-1)
          return NULL;
    if ((p=mmap (NULL,s.st_size, protection,map,df,0))==MAP_FAILED)
           return NULL;
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
     if ((p=MapearFichero(arg[0],protection))==NULL)
             perror ("Imposible mapear fichero");
     else
             printf ("fichero %s mapeado en %p\n", arg[0], p);
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

    // Escribir cont bytes desde memoria hacia el descriptor
    if ((n = write(df, p, cont)) == -1) {
        aux = errno;
        // No cerramos df porque ya está abierto fuera
        errno = aux;
        return -1;
    }

    return n;
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
        // no cerramos df porque ya estaba abierto fuera
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

    // Abrir archivo en modo escritura, crear si no existe, truncar si existe
    if ((df = open(f, O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1)
        return -1;

    // Escribir cont bytes desde memoria
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

            // Eliminar nodo usando tu función
            RemoveMemElement(L->MemList, p);

            return 0;  // éxito
        }

        p = next(L->MemList, p);
    }

    errno = ENOENT;   // no existe la dirección
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

    return NULL;    // no encontrado
}


void MList_print(enum tAllocL tipo,Listas L) {
    tPos p = first(L->MemList);

    if (isEmptyList(L->MemList)) {
        printf("No hay bloques de memoria.\n");
        return;
    }

    printf("%-18s %-10s %-20s %-10s %-10s\n",
           "ADDRESS", "SIZE", "TIME", "ALLOC", "KEY");

    printf("---------------------------------------------------------------\n");

    while (p != NULL) {

        tItemM item = (tItemM) getItem(L->MemList, p);

        // Solo imprime si coincide el tipo
        if (item->alloc == tipo) {

            char fecha[64];
            struct tm *tminfo = localtime(&item->time);
            strftime(fecha, sizeof(fecha), "%Y-%m-%d %H:%M:%S", tminfo);

            printf("%-18p %-10d %-20s %-10s %-10d\n",
                   item->address,
                   item->size,
                   fecha,
                   (item->alloc == SHARED ? "SHARED" :
                   item->alloc == MALLOC ? "MALLOC" : 
                                           "MAPPED"),
                   item->smb_key
            );
        }

        p = next(L->MemList, p);
    }
}


