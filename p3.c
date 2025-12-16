#include "p3.h"
#include "aux.h"
extern char **environ;


int Cmd_uid(char *trozos[], int ntrozos, Listas L, char *env[]) {
    if(ntrozos == 2 && !strcmp(trozos[0], "-get")) {
        printf("Credencial real: %d, (%s)\n", getuid(), getpwuid(getuid())->pw_name);
        printf("Credencial efectiva: %d, (%s)\n", geteuid(), getpwuid(geteuid())->pw_name);
        return 0;
    }
    if(ntrozos == 3 && !strcmp(trozos[0], "-set")) {
        uid_t uid = (uid_t) atoi(trozos[1]);

        if(setuid(uid) == -1) {
            fprintf(stderr, "Imposible cambiar credencial a uid: %s\n", trozos[1]);

            return 1;
        }
        printf("Credencial cambiada a: %d, (%s)\n", getuid(), getpwuid(getuid())->pw_name);
        return 0;
    }
    if(ntrozos== 4 && !strcmp(trozos[0], "-set") && !strcmp(trozos[1], "-l")) {
        struct passwd *pwd = getpwnam(trozos[2]);

        if(!pwd) {
            fprintf(stderr, "Usuario no encontrado: %s\n", trozos[2]);
            return 1;
        }
        if(setuid(pwd->pw_uid) == -1) {
            fprintf(stderr, "Imposible cambiar credencial a uid: %s\n", trozos[2]);
            return 1;
        }
        printf("Credencial cambiada a: %d, (%s)\n", getuid(), getpwuid(getuid())->pw_name);
        return 0;
    }
    fprintf(stderr, "Uso: uid -get | uid -set <uid> | uid -set -l <login>\n");
    return 1;    
}

int Cmd_envvar(char *trozos[],int ntrozos,Listas L, char *env[]) {
    if (ntrozos == 2 && (strcmp(trozos[0], "-show"))==0) {
        Aux_processos_show(env, "main arg3");
        return 0;
    }
    if(ntrozos== 3 && (strcmp(trozos[0], "-show")==0)) {

        char *var = trozos[1];
        char *val = getenv(var);
         if (val == NULL) {
            fprintf(stderr, "Variable de entorno %s no encontrada.\n", var);

            return 1;
        } 
        printf("Variable de entorno %s encontrada:\n", var);
        int pos_main = BuscarVariable(var, env);
        if(pos_main>=0) { 
            printf("  Con main arg3 %s(%p) @%p\n",env[pos_main],(void*) env[pos_main], (void*)&env);

        }else{
            fprintf(stderr, "Variable de entorno %s no encontrada en arg3 main.\n", trozos[1]);
        }
        
        int pos_environ = BuscarVariable(var, environ);
        if(pos_environ>=0) {
            printf("  Con environ %s(%p) @%p\n", environ[pos_environ],(void*) environ[pos_environ], (void*)&environ);
        }else{
            fprintf(stderr, "Variable de entorno %s no encontrada en environ.\n", var);
        }
        printf("Con getenv %s(%p)\n", var, val);
        
        return 0;
    }
    if(ntrozos == 5 && strcmp(trozos[0], "-change")==0) {
        char *mode = trozos[1];
        char *var = trozos[2];
        char *val = trozos[3];
        if(strcmp(trozos[1], "-a") == 0) {
            int pos=BuscarVariable(var, env);
            if(pos<0) {
                fprintf(stderr, "Variable de entorno %s no encontrada en arg3 main.\n", var);
                return 1;
            }
            if(CambiarVariable(var, val, env) <0){
                fprintf(stderr, "Error al cambiar la variable de entorno %s en arg3 main.\n", var);
                return 1;
            } 
            printf("Variable de entorno %s cambiada a %s en arg3 main.\n", var, val);
            return 0;
        }
        if(strcmp(mode, "-e") == 0){
            int pos=BuscarVariable(var, environ);
            if(pos<0) {
                fprintf(stderr, "Variable de entorno %s no encontrada en environ.\n", var);
                return 1;
            }
            if(CambiarVariable(var, val, environ) <0){
                fprintf(stderr, "Error al cambiar la variable de entorno %s en environ.\n", var);
                return 1;
            } 
            printf("Variable de entorno %s cambiada a %s en environ.\n", var, val);
            return 0;
        }
        if(strcmp(mode,"-p")==0){
            char *entrada=malloc(strlen(var)+strlen(val)+2);
            sprintf(entrada,"%s=%s",var,val);
            if(putenv(entrada)!=0){
                fprintf(stderr, "Error al cambiar la variable de entorno %s con putenv.\n", var);
                return 1;
            }
            printf("Variable de entorno %s cambiada a %s con putenv.\n", var, val);
            return 0;
        }
        fprintf(stderr, "Modo de cambio desconocido: %s\n", mode);
        return 1;
    }
    fprintf(stderr, "Uso: envvar -show [var] | envvar -change <-a|-e|-p> <var> <value>\n");
    return 1;
}


int Cmd_showenv(char *trozos[],int ntrozos, Listas L, char *env[]) {
    if (ntrozos ==1){
        Aux_processos_show(env, "main arg3");
        return 0;
    } 
    if (ntrozos ==2){
        if (strcmp(trozos[0], "-environ")==0){
            Aux_processos_show(environ, "environ");
            return 0;

        } 
        if (strcmp(trozos[0], "-addr")==0){
            printf("Direcciones de env[]: %p\n", (void*)env);
            printf("Direcciones de environ: %p\n", (void*)environ);
            Aux_processos_show(env, "main arg3");     // muestra punteros y valores
            Aux_processos_show(environ, "environ");   // idem
            return 0;
        }
        return 1;
    }
    fprintf(stderr, "Uso: showenv [-environ|-addr]\n");
    return 1;
}

int Cmd_fork (char *trozos[], int ntrozos, Listas L, char *env[])
{
	pid_t pid;
    pid=fork();
    if(pid== -1){
        perror("Error en fork");
        return 1;
    }
	if (pid==0){
		printf ("ejecutando proceso %d\n", getpid());
        exit(0);
	}
    if(waitpid (pid,NULL,0)==-1){
        perror ("Error en waitpid");
        return 1;
    }
    return 0;
}
int Cmd_exec(char *trozos[], int ntrozos, Listas L, char *env[])
{
    int i, priority = -99999; 
    int has_priority = 0;
    char *args[256];
    int nargs = 0;

    if (ntrozos < 2) {
        fprintf(stderr, "Uso: exec progspec\n");
        return 1;
    }

    /* --- 1. Parsear los argumentos y detectar @pri --- */
    for (i = 0; i < ntrozos && trozos[i] != NULL; i++) {

        /* detectar @pri */
        if (trozos[i][0] == '@') {
            priority = atoi(trozos[i] + 1);
            has_priority = 1;
            continue;
        }

        /* detectar & → EN exec NO debería usarse */
        if (strcmp(trozos[i], "&") == 0) {
            fprintf(stderr, "exec no permite ejecución en background (&)\n");
            return 1;
        }

        /* guardar argumento normal */
        args[nargs++] = trozos[i];
    }

    args[nargs] = NULL;  // execvp necesita NULL

    if (nargs == 0) {
        fprintf(stderr, "exec: no hay ejecutable\n");
        return 1;
    }

    /* --- 2. Cambiar prioridad si se pidió --- */
    if (has_priority) {
        if (setpriority(PRIO_PROCESS, getpid(), priority) == -1) {
            perror("setpriority");
            return 1;
        }
    }

    /* --- 3. Ejecutar el programa SIN fork() → reemplaza el shell --- */
    execvp(args[0], args);

    /* --- 4. Si llega aquí, exec ha fallado --- */
    fprintf(stderr, "exec: error al ejecutar %s: %s\n", args[0], strerror(errno));

    return 1;
}
int jobs(char *trozos[], int ntrozos, Listas L, char *env[]) {
    if(ntrozos > 1) {
        printf("Usage: jobs\n");
        return 1;
    }

    if (isEmptyList(L->ProcList)) {
        printf("No background processes.\n");
        return 1;
    }

    tPos p = first(L->ProcList);
    while (p != NULL) {
        tItemP proc = (tItemP)getItem(L->ProcList, p);
        int status;
        pid_t ret = waitpid(proc->pid, &status, WNOHANG | WUNTRACED | WCONTINUED);
        if (ret > 0) {
            if (WIFEXITED(status)) {
                proc->status = FINISHED;
                proc->wstatus = WEXITSTATUS(status);
            } else if (WIFSIGNALED(status)) {
                proc->status = SIGNALED;
                proc->wstatus = WTERMSIG(status);
            } else if (WIFSTOPPED(status)) {
                proc->status = STOPPED;
                proc->wstatus = WSTOPSIG(status);
            } else if (WIFCONTINUED(status)) {
                if(proc->status == STOPPED) {
                    proc->status = ACTIVE;
                }
                    // remains ACTIVE
            }
        }

        char timebuf[64];
        strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", localtime(&proc->time));
        int priority = (proc->status == ACTIVE) ? getpriority(PRIO_PROCESS, proc->pid) : -1;

        printf("PID: %d | Launched: %s | Status: ", proc->pid, timebuf);
        switch(proc->status) {
            case ACTIVE:   printf("ACTIVE"); break;
            case FINISHED: printf("FINISHED (exit=%d)", proc->wstatus); break;
            case SIGNALED: printf("SIGNALED (%s)", NombreSenal(proc->wstatus)); break;
            case STOPPED:  printf("STOPPED (%s)", NombreSenal(proc->wstatus))  ; break;
        }
        printf(" | Priority: %d | Cmd: %s\n", priority, proc->command);

        p = next(L->ProcList, p);
    }

    return 1;
}
int deljobs(char *trozos[], int ntrozos, Listas L, char *env[]) {
    int eliminarTerm = 0;
    int eliminarSig = 0;
    if (ntrozos != 2) {
        printf("Usage: deljobs -term|-sig\n");
        return 1;
    }
    if (strcmp(trozos[0], "-term") == 0) eliminarTerm = 1;
    else if (strcmp(trozos[0], "-sig") == 0) eliminarSig = 1;
    else {
        printf("Usage: deljobs -term|-sig\n");
        return 1;
    }
    if(isEmptyList(L->ProcList)) {
        printf("No background processes to delete.\n");
        return 1;
    }
    tPos p = first(L->ProcList);
    while (p != NULL) {
        tItemP proc = (tItemP)getItem(L->ProcList, p);
        tPos nextPos = next(L->ProcList, p); // Guardar la siguiente posición
        if ((eliminarTerm && proc->status == FINISHED) ||
            (eliminarSig && proc->status == SIGNALED)) {
            printf("Removing process PID: %d, Cmd: %s\n", proc->pid, proc->command);
            RemoveProcElement(&L->ProcList, p);
        }
        p = nextPos;
    }
    return 0;

}




void ejecutarComando(char *trozos[], int ntrozos, Listas L) {
    int background = 0;
    int priority = -1;
    char *args[ntrozos +1];
    int i, j = 0;

    for (i = 0; i < ntrozos; i++) {
        if (strcmp(trozos[i], "&") == 0) {
            background = 1;
        } else if (trozos[i][0] == '@') {
            priority = atoi(trozos[i] + 1);
        } else {
            args[j++] = trozos[i];
        }
    }
    args[j] = NULL;

    pid_t pid = fork();
    if (pid == 0) { // hijo
        if (priority >= 0)
            if(setpriority(PRIO_PROCESS, 0, priority) == -1) {
                perror("setpriority failed");
            }
        execvp(args[0], args);
        perror("execvp failed");
        exit(1);
    } else if (pid > 0) { // padre
        if (background) {
            size_t len = 0;
            for (int i = 0; i < ntrozos; i++) {
                if (strcmp(trozos[i], "&") != 0 && trozos[i][0] != '@')
                    len += strlen(trozos[i]) + 1;
            }
            tItemP newProc = malloc(sizeof(struct structProc));
            newProc->pid = pid;
            newProc->time = time(NULL);
            newProc->status = ACTIVE;
            newProc->command = malloc(len + 1);
            newProc->command[0] = '\0';
            newProc->wstatus = 0;
            for (int i = 0; i < ntrozos; i++) {
                if (strcmp(trozos[i], "&") != 0 && trozos[i][0] != '@') {
                    strcat(newProc->command, trozos[i]);
                    if (i < ntrozos - 1) {
                        strcat(newProc->command, " ");
                    }
                }
            }
            
            InsertItem(&L->ProcList, newProc, NULL);
            printf("Process %d running in background\n", pid);
        } else {
            int status;
            waitpid(pid, &status, 0); // espera al hijo
        }
    } else {
        perror("fork failed");
    }
}
