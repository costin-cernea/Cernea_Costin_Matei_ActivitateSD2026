

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

/*
 * nrElemente - cate elemente sunt "vizibile" (active) in heap
 * nrTotal    - cate elemente exista in total in vector (vizibile + ascunse)
 * lungime    - capacitatea maxima alocata
 */
struct Heap {
    Masina* vector;
    int lungime;
    int nrElemente;
    int nrTotal;
};
typedef struct Heap Heap;

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

/* ------------------------------------------------------------------- */

Heap initializareHeap(int lungime) {
    Heap h;
    h.lungime    = lungime;
    h.nrElemente = 0;
    h.nrTotal    = 0;
    h.vector     = (Masina*)malloc(sizeof(Masina) * lungime);
    return h;
}

/*
 * filtreazaHeap - Max-Heapify (sift-down)
 * Coboara nodul de la pozitia 'pozitieNod' pana cand e mai mare
 * decat ambii copii. Copilul stang al lui i = 2*i+1, drept = 2*i+2.
 */
void filtreazaHeap(Heap heap, int pozitieNod) {
    int stang  = 2 * pozitieNod + 1;
    int drept  = 2 * pozitieNod + 2;
    int maxIdx = pozitieNod;

    if (stang < heap.nrElemente &&
        heap.vector[stang].pret > heap.vector[maxIdx].pret)
        maxIdx = stang;

    if (drept < heap.nrElemente &&
        heap.vector[drept].pret > heap.vector[maxIdx].pret)
        maxIdx = drept;

    if (maxIdx != pozitieNod) {
        Masina tmp              = heap.vector[pozitieNod];
        heap.vector[pozitieNod] = heap.vector[maxIdx];
        heap.vector[maxIdx]     = tmp;
        filtreazaHeap(heap, maxIdx);
    }
}

/*
 * citireHeapDeMasiniDinFisier
 * Citim toate masinile in vector, apoi aplicam heapify bottom-up:
 * pornim de la ultimul nod non-frunza spre radacina.
 * Construieste heap-ul in O(n), mai eficient decat n insertii individuale.
 */
Heap citireHeapDeMasiniDinFisier(const char* numeFisier) {
    FILE* file = fopen(numeFisier, "r");
    Heap heap  = initializareHeap(100);

    if (!file) {
        printf("Eroare: fisierul '%s' nu a putut fi deschis.\n", numeFisier);
        return heap;
    }

    while (!feof(file)) {
        Masina m = citireMasinaDinFisier(file);
        if (m.id != -1) {
            heap.vector[heap.nrElemente++] = m;
        }
    }
    fclose(file);

    heap.nrTotal = heap.nrElemente;

    /* Heapify bottom-up: ultimul nod non-frunza e la (nrElemente/2 - 1) */
    for (int i = heap.nrElemente / 2 - 1; i >= 0; i--) {
        filtreazaHeap(heap, i);
    }

    return heap;
}

/* Afiseaza elementele active (vizibile) din heap */
void afisareHeap(Heap heap) {
    printf("=== Elemente vizibile (%d) ===\n", heap.nrElemente);
    for (int i = 0; i < heap.nrElemente; i++) {
        afisareMasina(heap.vector[i]);
    }
}

/* Afiseaza elementele "ascunse" - extrase dar inca in vector */
void afiseazaHeapAscuns(Heap heap) {
    int nrAscunse = heap.nrTotal - heap.nrElemente;
    printf("=== Elemente ascunse (%d) ===\n", nrAscunse);
    for (int i = heap.nrElemente; i < heap.nrTotal; i++) {
        afisareMasina(heap.vector[i]);
    }
}

/*
 * extrageMasina
 * Scoate radacina (maximul) si o muta la pozitia nrElemente (zona ascunsa).
 * Apoi pune ultimul element activ in radacina si reface heap-ul.
 * Primeste void* conform semnaturii din comentariile originale.
 */
Masina extrageMasina(void* ptr) {
    Heap* heap = (Heap*)ptr;
    Masina extras = heap->vector[0];

    heap->nrElemente--;

    /* Schimbam radacina cu ultimul element activ si refacem heap-ul */
    Masina tmp                      = heap->vector[0];
    heap->vector[0]                 = heap->vector[heap->nrElemente];
    heap->vector[heap->nrElemente]  = tmp;

    filtreazaHeap(*heap, 0);

    return extras;
}

void dezalocareHeap(Heap* heap) {
    for (int i = 0; i < heap->nrTotal; i++) {
        free(heap->vector[i].model);
        free(heap->vector[i].numeSofer);
    }
    free(heap->vector);
    heap->vector     = NULL;
    heap->lungime    = 0;
    heap->nrElemente = 0;
    heap->nrTotal    = 0;
}

/* ------------------------------------------------------------------- */

int main() {
    Heap heap = citireHeapDeMasiniDinFisier("masini.txt");

    printf("Heap initial (MAX dupa pret):\n");
    afisareHeap(heap);

    printf("--- Extragere top 3 ---\n");
    for (int i = 0; i < 3; i++) {
        Masina m = extrageMasina(&heap);
        printf("Extras: %s - %.2f\n\n", m.model, m.pret);
    }

    afisareHeap(heap);
    afiseazaHeapAscuns(heap);

    dezalocareHeap(&heap);
    return 0;
}