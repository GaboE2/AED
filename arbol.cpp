#include <stdio.h>

typedef struct nodo {
    int valor;
    struct nodo* izq;
    struct nodo* der;
} nodo;

nodo* CreaArbol(int* ini, int* fin) {
    int n = fin - ini;
    if (n == 0) return NULL;

   
    static nodo arbol[64];
    
    nodo* cola[64];
    int frente = 0, final = 0;

    // Crear hojas
    for (int i = 0; i < n; i++) {
        arbol[i].valor = ini[i];
        arbol[i].izq = NULL;
        arbol[i].der = NULL;
        cola[final++] = &arbol[i];
    }

    int indice_actual = n; // nodo en arboles

    printf("Cola inicial: ");
    for (int i = frente; i < final; i++) {
        printf("%d ", cola[i]->valor);
    }
    printf("\n");

    while (frente < final - 1) {
        // sacar los 2 nodos a la cola
        nodo* izq = cola[frente++];
        nodo* der = cola[frente++];

        printf("Pop: %d y %d\n", izq->valor, der->valor);

        // nuevo padre
        arbol[indice_actual].valor = izq->valor + der->valor;
        arbol[indice_actual].izq = izq;
        arbol[indice_actual].der = der;

        // nuevo nodo
        cola[final++] = &arbol[indice_actual];
        printf("Push: %d\n", arbol[indice_actual].valor);

        printf("Cola actual: ");
        for (int i = frente; i < final; i++) {
            printf("%d ", cola[i]->valor);
        }
        printf("\n");

        indice_actual++;
    }

    return cola[frente]; // La raíz del árbol
}

void imprimirArbol(nodo* raiz, int nivel) {
    if (raiz == NULL) return;
    for (int i = 0; i < nivel; i++) printf("  ");
    printf("%d\n", raiz->valor);
    imprimirArbol(raiz->izq, nivel + 1);
    imprimirArbol(raiz->der, nivel + 1);
}

int main() {
    int arr[16] = { 2,4,7,9,10,15,3,25,38,74,1,5,10,13,11,35 };
    int n = sizeof(arr) / sizeof(arr[0]);

    nodo* raiz = CreaArbol(arr, arr + n);

    printf("\nArbol Final:\n");
    imprimirArbol(raiz, 0);

    return 0;
}