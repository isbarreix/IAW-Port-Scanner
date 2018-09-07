#include <stdlib.h>
#include <stdio.h>
#include "list.h"

int estaLista(tLista listap, int x) {
    if(listap== NULL)
        return 0;
    else
        if(listap->elto ==  x)
            return 1 ;
        else
            return estaLista(listap->sig, x);
}


void insertarPpio(tLista *lista, int x) {
        tLista nueva= (tLista) malloc(sizeof(tCelda));
        nueva->elto= x;
        nueva->ant= NULL;
        nueva->sig= *lista;
        if((*lista)!= NULL)
            (*lista)->ant= nueva;
        *lista= nueva;
}

void insertarFinal(tLista *lista, int x) {
        tLista aux=  *lista;
        tLista nueva= (tCelda*) malloc(sizeof(tCelda));
        nueva->elto= x;
        if(*lista== NULL)
            insertarPpio(lista,x);
        else{
            while(aux->sig != NULL)
                aux= aux->sig;
            nueva->sig= NULL;
            nueva->ant= aux;
            aux->sig= nueva;
        }
}

void insertarElemento(tLista * lista, int x) {
    if(*lista== NULL)  /* Si la lista por parametro es NULA,se inserta alppio.*/
            insertarPpio(lista, x);
    else              /*Caso contrario voy hasta final de la lista.*/
            insertarFinal(lista, x);
}

void eliminar(tLista* listap, int x) {
    if(*listap!= NULL) {
        tLista aux=  (*listap)->sig;
        if((*listap)->elto==  x) {  /*El primer elemento es el que se quiere borrar*/
                (*listap)->sig= NULL;
                (*listap)->ant= NULL;
                free(*listap);
                aux->ant= NULL;
                (*listap)= aux;
        }
        else{   /*Tiene mas de un elemento y no es el 1ero*/
                while(aux->elto !=  x)
                    aux= aux->sig;
                aux->ant->sig= aux->sig;
                if(aux->sig != NULL)
                    aux->sig->ant= aux->ant;
                aux->ant= NULL;
                aux->sig= NULL;
                free(aux);

        }
    }

}

void insertarAtras(tLista *lista, int pos ,int x) {
	if(*lista != NULL) {
		if((*lista)->elto==  pos)   /*tengo el primer elemento*/{
			 insertarPpio(lista, x);
		}
		else{
			tLista nuevo=  (tLista) malloc(sizeof(tCelda));
			tLista aux= *lista;
			while(aux->elto!=  pos) {
				aux= aux->sig;
			}
			nuevo->elto= x;
			nuevo->ant= aux->ant;
			nuevo->sig= aux;
			nuevo->ant->sig= nuevo;
			aux->ant= nuevo;
		}
	}
}

void insertarOrdenado(tLista *listaOrd, int x) {
    tLista cursor= *listaOrd;
    while(cursor!= NULL&& x>cursor->elto)
        cursor= cursor->sig;
    if(cursor== NULL)/* Llego al final y es mayor a todos*/
        insertarFinal(listaOrd, x);
    else/*Esta entre dos elementos y es menor*/
        insertarAtras(listaOrd, cursor->elto, x);
}

void ordenar(tLista *lista) {
    tLista aux= NULL;
    if(*lista != NULL && (*lista)->sig!= NULL) { /*La lista tiene mas de un elemento*/
            insertarElemento(&aux, (*lista)->elto);
            *lista= (*lista)->sig;
             while(*lista!= NULL) {
                 insertarOrdenado(&aux, (*lista)->elto);
                *lista=  (*lista)->sig;
            }
            *lista= aux;
    }
}

void mostrarLista(tLista lista){
    if(lista!=NULL){
        printf("Elemento: %d\n",lista->elto);
        mostrarLista(lista->sig);
    }
}

void destruirListaAux(tLista plista){
    if(plista!=NULL){
        destruirListaAux(plista->sig);
        plista->sig=NULL;
        free(plista);
        plista=NULL;
    }
}

void destruirLista(tLista *plista){
    if(*plista!=NULL){
        destruirListaAux((*plista)->sig);
        (*plista)->sig=NULL;
        free(*plista);
        *plista=NULL;
    }
}
