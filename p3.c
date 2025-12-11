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

