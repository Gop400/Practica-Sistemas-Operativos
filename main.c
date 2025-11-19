
/*Autor1:Gabriel Oliveira Pais
 *Autor2:Samuel Mouriño
 *email1:g.oliveira@udc.es
 *email2:samuel.mouriño@udc.es
 */
#define size_cadena 1024
#include "p0.h"
#include "p1.h"
#include "p2.h"
#include "aux.h"

int main() {
    char cadena[1024];
    char *trozos[64];

    int ntokens;
    int terminado=0;

    Listas L;
    tList h;
    tList f;
    tList m;

    L=malloc(sizeof(struct structListas));
    CreateEmptyList(&h);
    CreateEmptyList(&f);
    CreateEmptyList(&m);
    L->OpenFilesList=f;
    L->HistoricList=h;
    L->MemList=m;
    initOpenList(L);
    while(1) {
        printf(":~$");
        fgets(cadena,sizeof(cadena),stdin);
        if (cadena[0] != ' ' && cadena[0] != '\n') {
            Addhistoric(cadena,&L->HistoricList);
            ntokens = TrocearCadena(cadena, trozos);
            terminado=ProcesarEntrada(trozos, ntokens, L);
            if (terminado==1) {
                break;
            }
        }else{
            printf("Escriba un comando\n");
        }
    }
    deleteList(&L->HistoricList,'H');
    deleteList(&L->OpenFilesList,'O');
    deleteList(&L->MemList,'M');
    free(L);
    return 0;
}
