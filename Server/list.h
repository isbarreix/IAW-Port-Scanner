
typedef struct celda {
    int elto;
    struct celda* sig;
    struct celda* ant;
} *tLista;

typedef struct celda tCelda;

int estaLista(tLista listap, int x);
	
void insertarPpio(tLista *lista, int x);

void insertarFinal(tLista *lista, int x);
	
void insertarElemento(tLista * lista, int x);
	
void eliminar(tLista* listap, int x);
	
void insertarAtras(tLista *lista, int pos ,int x);
	
void insertarOrdenado(tLista *listaOrd, int x);
	
void ordenar(tLista *lista);
	
void mostrarLista(tLista lista);
	
void destruirLista(tLista *plista);
