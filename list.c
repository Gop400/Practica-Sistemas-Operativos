
#include "list.h"

void createNode(tPos *p) {
    *p=malloc(sizeof(struct tNode));
}
void CreateEmptyList(tList *l1) {
    *l1=LNULL;

}
bool isEmptyList(tList l1) {
    return l1==LNULL;
}
tPos first(tList l1) {
    if(!isEmptyList(l1)) {
        return l1;
    }
    return NULL;
}

tPos last(tList l1){
    //Variable last con valor inicial nulo
    tPos last;
    //Comprueba que la lista no este vacia
    if(!isEmptyList(l1)){
        last=first(l1);
        //Bucle que recorre toda la lista hasta la ultima posicion,modificando el valor last hasta llegar al ultimo en la lista
        while(last->next!=LNULL){
            last=last->next;
        }
    }
    return last;//Devuelve el ultimo nodo si la lista no esta vacia o LNULL si lo esta
}
tPos next(tList l1,tPos p) {
    tPos next=LNULL;
    if(!isEmptyList(l1)) {
        next=p->next;
    }
    return next;
}
tPos previous(tList l1,tPos p) {
    tPos previous=LNULL;
    tPos actual=first(l1);
    if (!isEmptyList(l1)) {
        while(actual!=LNULL&& actual!=p) {
            previous=actual;
            actual=next(l1,actual);
        }
    }
    return previous;

}

bool InsertItem(tList *l1,tItem item,tPos p) {
    tPos aux;
    createNode(&aux);
    if(aux==NULL) {
        return false;;
    }
    aux->item=item;
    aux->next=LNULL;
    if(isEmptyList(*l1)) {
        *l1=aux;
        return true;
    }if(p==LNULL) {
        last(*l1)->next=aux;
        return true;
    }if(p==*l1){
        aux->next=*l1;
        *l1=aux;
        return true;

    }else {
        aux-> item = p-> item;//Se inserta el item ubicado en pos a Nnodo
        p-> item =item;//t1 se almacena en el nodo pos
        aux -> next = p -> next;
        p -> next = aux;
        return true;
    }
    return false;
}
void RemoveElement(tList *l1, tPos p) {
    if (isEmptyList(*l1) || p == LNULL) return;

    if (p == *l1) {
        // Caso: Eliminar el primer nodo
        *l1 = p->next; // Mover el inicio de la lista al siguiente nodo
        free(p);// Liberar el nodo actual
        return ;
    }
    // Caso: Eliminar cualquier nodo que no sea el primero
    tPos prev = previous(*l1, p); // Obtener el nodo anterior a 'p'
    if (prev == LNULL) {
        return; // Nodo previo no encontrado, error en la estructura
    }
    prev->next = p->next; // Saltar el nodo actual
    free(p); // Liberar el nodo eliminado

}

tItem getItem(tList l1,tPos p) {
    if (p==LNULL) {
        return NULL;
    }
    return p->item;
}

void deleteList(tList *l1,char type) {

    if(!isEmptyList(*l1)) {
        switch(type) {
            case 'H':
                while (!isEmptyList(*l1)) {
                    RemoveHistoricElement(l1, *l1);
                }    
            break;
            case 'O':
                while (!isEmptyList(*l1)) {
                    RemoveOpenFileElement(l1, *l1);
                }
            break;
            case 'M':
                while (!isEmptyList(*l1)) {
                    RemoveMemElement(l1, *l1);
                }
            default:
                while (!isEmptyList(*l1)) {
                RemoveElement(l1, *l1);
                }
            break;
        }
    }
}

int sizeList(tList l1) {
    int size=0;
    if(!isEmptyList(l1)) {
        tPos p=first(l1);
        while(p!=LNULL) {
            size++;
            p=p->next;
        }
    }
    return size;
}
// ================================================
// RemoveElement y deleteList para HistoricList
// ================================================
void RemoveHistoricElement(tList *l, tPos p) {
    if (isEmptyList(*l) || p == LNULL) return;

    tItemH item = (tItemH)getItem(*l, p);
    if (item != NULL) {
        free(item->comando); // liberar cadena strdup
        free(item);          // liberar estructura
    }
    RemoveElement(l, p);
}

void DeleteHistoricList(tList *l) {
    while (!isEmptyList(*l)) {
        RemoveHistoricElement(l, *l);
    }
}

// ================================================
// RemoveElement y deleteList para OpenFilesList
// ================================================
void RemoveOpenFileElement(tList *l, tPos p) {
    if (isEmptyList(*l) || p == LNULL) return;

    tItemF item = (tItemF)p->item;
    if (item != NULL) {
        free(item->name); // liberar cadena strdup
        free(item);       // liberar estructura
    }
    RemoveElement(l, p);
}

void DeleteOpenFilesList(tList *l) {
    while (!isEmptyList(*l)) {
        RemoveOpenFileElement(l, *l);
    }
}

void RemoveMemElement(tList *l, tPos p) {
    if (isEmptyList(*l) || p == LNULL) return;

    tItemM item = (tItemM)p->item;
    if (item != NULL) {
        free(item);       // liberar estructura
    }
    RemoveElement(l, p);
}
