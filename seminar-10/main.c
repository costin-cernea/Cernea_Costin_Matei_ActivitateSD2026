

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct StructuraMasina {
    int id;
    int nrUsi;
    float pret;
    char* model;
    char* numeSofer;
    unsigned char serie;
};
typedef struct StructuraMasina Masina;

/* Nod comun pentru ambele structuri */
struct Nod {
    Masina info;
    struct Nod* next;
};
typedef struct Nod Nod;

/* ------------------------------------------------------------------- */
/* Stack                                                                */
/* ------------------------------------------------------------------- */

/*
 * top   - varful stivei (ultimul element adaugat)
 * size  - numar de elemente curente
 */
struct Stack {
    Nod* top;
    int  size;
};
typedef struct Stack Stack;

/* ------------------------------------------------------------------- */
/* Queue                                                                */
/* ------------------------------------------------------------------- */

/*
 * head  - primul element (urmatorul de extras)
 * tail  - ultimul element (unde adaugam)
 * Pastram pointerul la tail ca sa evitam parcurgerea listei la enqueue.
 */
struct Queue {
    Nod* head;
    Nod* tail;
    int  size;
};
typedef struct Queue Queue;

/* ------------------------------------------------------------------- */

Masina citireMasinaDinFisier(FILE* file) {
    char buffer[100];
    char sep[3] = ",\n";
    Masina m1;
    m1.id = -1;
    m1.model = NULL;
    m1.numeSofer = NULL;

    if (!fgets(buffer, 100, file)) return m1;

    char* aux = strtok(buffer, sep);
    if (!aux) return m1;

    m1.id    = atoi(aux);
    m1.nrUsi = atoi(strtok(NULL, sep));
    m1.pret  = atof(strtok(NULL, sep));

    aux = strtok(NULL, sep);
    m1.model = malloc(strlen(aux) + 1);
    strcpy(m1.model, aux);

    aux = strtok(NULL, sep);
    m1.numeSofer = malloc(strlen(aux) + 1);
    strcpy(m1.numeSofer, aux);

    m1.serie = *strtok(NULL, sep);
    return m1;
}

void afisareMasina(Masina masina) {
    printf("Id: %d\n", masina.id);
    printf("Nr. usi : %d\n", masina.nrUsi);
    printf("Pret: %.2f\n", masina.pret);
    printf("Model: %s\n", masina.model);
    printf("Nume sofer: %s\n", masina.numeSofer);
    printf("Serie: %c\n\n", masina.serie);
}

/* Aloca un nod nou cu masina data */
static Nod* creeazaNod(Masina masina) {
    Nod* nou   = malloc(sizeof(Nod));
    nou->info  = masina;
    nou->next  = NULL;
    return nou;
}

/* ------------------------------------------------------------------- */
/* Operatii Stack                                                       */
/* ------------------------------------------------------------------- */

Stack initStack() {
    Stack s;
    s.top  = NULL;
    s.size = 0;
    return s;
}

/* Adauga la capul listei - devine noul top */
void pushStack(Stack* stiva, Masina masina) {
    Nod* nou  = creeazaNod(masina);
    nou->next = stiva->top;
    stiva->top = nou;
    stiva->size++;
}

/*
 * Scoate si returneaza elementul din varful stivei.
 * Apeleaza emptyStack() inainte ca sa eviti UB pe stiva goala.
 */
Masina popStack(Stack* stiva) {
    Masina m  = stiva->top->info;
    Nod* temp = stiva->top;
    stiva->top = stiva->top->next;
    free(temp);  /* eliberam nodul, nu si campurile char* din Masina */
    stiva->size--;
    return m;
}

int emptyStack(Stack* stiva) {
    return stiva->top == NULL;
}

int sizeStack(Stack* stiva) {
    return stiva->size;
}

Stack* citireStackMasiniDinFisier(const char* numeFisier) {
    FILE* file = fopen(numeFisier, "r");
    Stack* stiva = malloc(sizeof(Stack));
    *stiva = initStack();

    if (!file) {
        printf("Eroare: fisierul '%s' nu a putut fi deschis.\n", numeFisier);
        return stiva;
    }

    while (!feof(file)) {
        Masina m = citireMasinaDinFisier(file);
        if (m.id != -1) {
            pushStack(stiva, m);
        }
    }
    fclose(file);
    return stiva;
}

void dezalocareStivaDeMasini(Stack* stiva) {
    while (!emptyStack(stiva)) {
        Masina m = popStack(stiva);
        free(m.model);
        free(m.numeSofer);
    }
    free(stiva);
}

/* ------------------------------------------------------------------- */
/* Operatii Queue                                                       */
/* ------------------------------------------------------------------- */

Queue initQueue() {
    Queue q;
    q.head = NULL;
    q.tail = NULL;
    q.size = 0;
    return q;
}

/* Adauga la coada listei */
void enqueue(Queue* coada, Masina masina) {
    Nod* nou = creeazaNod(masina);
    if (coada->tail) {
        coada->tail->next = nou;
    } else {
        coada->head = nou;  /* coada era goala */
    }
    coada->tail = nou;
    coada->size++;
}

/* Scoate de la capul listei (FIFO) */
Masina dequeue(Queue* coada) {
    Masina m   = coada->head->info;
    Nod* temp  = coada->head;
    coada->head = coada->head->next;
    if (coada->head == NULL) {
        coada->tail = NULL;  /* coada a ramas goala */
    }
    free(temp);
    coada->size--;
    return m;
}

int emptyQueue(Queue* coada) {
    return coada->head == NULL;
}

int sizeQueue(Queue* coada) {
    return coada->size;
}

Queue* citireCoadaDeMasiniDinFisier(const char* numeFisier) {
    FILE* file = fopen(numeFisier, "r");
    Queue* coada = malloc(sizeof(Queue));
    *coada = initQueue();

    if (!file) {
        printf("Eroare: fisierul '%s' nu a putut fi deschis.\n", numeFisier);
        return coada;
    }

    while (!feof(file)) {
        Masina m = citireMasinaDinFisier(file);
        if (m.id != -1) {
            enqueue(coada, m);
        }
    }
    fclose(file);
    return coada;
}

void dezalocareCoadaDeMasini(Queue* coada) {
    while (!emptyQueue(coada)) {
        Masina m = dequeue(coada);
        free(m.model);
        free(m.numeSofer);
    }
    free(coada);
}

/* ------------------------------------------------------------------- */
/* Metode de procesare - lucreaza pe ambele structuri prin Nod*        */
/* ------------------------------------------------------------------- */

/*
 * getMasinaByID si calculeazaPretTotal primesc direct pointerul
 * la primul nod (top pentru stack, head pentru queue) ca sa nu
 * fie nevoie de doua versiuni ale aceleiasi functii.
 * Apelezi cu: getMasinaByID(stiva->top, id)
 *          sau getMasinaByID(coada->head, id)
 */
Masina getMasinaByID(Nod* lista, int id) {
    Masina m;
    m.id = -1;
    m.model = NULL;
    m.numeSofer = NULL;

    while (lista) {
        if (lista->info.id == id) {
            m = lista->info;
            m.model = malloc(strlen(lista->info.model) + 1);
            strcpy(m.model, lista->info.model);
            m.numeSofer = malloc(strlen(lista->info.numeSofer) + 1);
            strcpy(m.numeSofer, lista->info.numeSofer);
            return m;
        }
        lista = lista->next;
    }
    return m;
}

float calculeazaPretTotal(Nod* lista) {
    float total = 0;
    while (lista) {
        total += lista->info.pret;
        lista = lista->next;
    }
    return total;
}

/* ------------------------------------------------------------------- */

int main() {
    /* --- Stack --- */
    Stack* stiva = citireStackMasiniDinFisier("masini.txt");
    printf("=== Stack (LIFO) - %d elemente ===\n", sizeStack(stiva));
    printf("Pret total stiva: %.2f\n\n", calculeazaPretTotal(stiva->top));

    Masina dinStiva = getMasinaByID(stiva->top, 3);
    if (dinStiva.id != -1) {
        printf("Masina cu id=3 (din stiva):\n");
        afisareMasina(dinStiva);
        free(dinStiva.model);
        free(dinStiva.numeSofer);
    }

    printf("Extragere din stiva (LIFO):\n");
    while (!emptyStack(stiva)) {
        Masina m = popStack(stiva);
        printf("  extras: id=%d, model=%s\n", m.id, m.model);
        free(m.model);
        free(m.numeSofer);
    }
    free(stiva);

    /* --- Queue --- */
    printf("\n=== Queue (FIFO) - citire din fisier ===\n");
    Queue* coada = citireCoadaDeMasiniDinFisier("masini.txt");
    printf("Elemente in coada: %d\n", sizeQueue(coada));
    printf("Pret total coada: %.2f\n\n", calculeazaPretTotal(coada->head));

    Masina dinCoada = getMasinaByID(coada->head, 7);
    if (dinCoada.id != -1) {
        printf("Masina cu id=7 (din coada):\n");
        afisareMasina(dinCoada);
        free(dinCoada.model);
        free(dinCoada.numeSofer);
    }

    printf("Extragere din coada (FIFO):\n");
    while (!emptyQueue(coada)) {
        Masina m = dequeue(coada);
        printf("  extras: id=%d, model=%s\n", m.id, m.model);
        free(m.model);
        free(m.numeSofer);
    }
    free(coada);

    return 0;
}