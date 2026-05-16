
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

typedef struct NodP NodP;
typedef struct NodS NodS;

/* Nod din lista principala - reprezinta un nod al grafului */
struct NodP {
    Masina m;
    NodP*  next;
    NodS*  vecini;  /* lista de adiacenta a acestui nod */
};

/* Nod din lista secundara - reprezinta un vecin (muchie) */
struct NodS {
    NodS* next;
    NodP* info;   /* pointer catre nodul vecin din lista principala */
};

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

void inserareListaPrincipala(NodP** graf, Masina m) {
    NodP* nodNou   = malloc(sizeof(NodP));
    nodNou->m      = m;
    nodNou->next   = NULL;
    nodNou->vecini = NULL;

    if (*graf) {
        NodP* aux = *graf;
        while (aux->next) aux = aux->next;
        aux->next = nodNou;
    } else {
        *graf = nodNou;
    }
}

void inserareListaSecundara(NodS** lista, NodP* vecin) {
    NodS* nodNou = malloc(sizeof(NodS));
    nodNou->info = vecin;
    nodNou->next = NULL;

    if (*lista) {
        NodS* aux = *lista;
        while (aux->next) aux = aux->next;
        aux->next = nodNou;
    } else {
        *lista = nodNou;
    }
}

/*
 * cautaNodDupaID - parcurge lista principala si returneaza pointerul
 * la nodul cu id-ul cautat, sau NULL daca nu exista.
 * Bug in original: instructiunile erau in ordine gresita (return
 * inainte de temp=temp->next, iar return NULL era in interiorul while).
 */
NodP* cautaNodDupaID(NodP* listaPrincipala, int id) {
    NodP* temp = listaPrincipala;
    while (temp) {
        if (temp->m.id == id) return temp;
        temp = temp->next;
    }
    return NULL;
}

/*
 * inserareMuchie - adauga o muchie neorientata intre doua noduri.
 * Fiecare nod apare in lista de vecini a celuilalt.
 */
void inserareMuchie(NodP* listaPrincipala, int idStart, int idStop) {
    NodP* start = cautaNodDupaID(listaPrincipala, idStart);
    NodP* stop  = cautaNodDupaID(listaPrincipala, idStop);
    if (start && stop) {
        inserareListaSecundara(&start->vecini, stop);
        inserareListaSecundara(&stop->vecini, start);
    }
}

NodP* citireNoduriMasiniDinFisier(const char* numeFisier) {
    FILE* f = fopen(numeFisier, "r");
    NodP* graf = NULL;

    if (!f) {
        printf("Eroare: fisierul '%s' nu a putut fi deschis.\n", numeFisier);
        return NULL;
    }

    while (!feof(f)) {
        Masina m = citireMasinaDinFisier(f);
        if (m.id != -1) {
            inserareListaPrincipala(&graf, m);
        }
    }
    fclose(f);
    return graf;
}

void citireMuchiiDinFisier(const char* numeFisier, NodP* graf) {
    FILE* f = fopen(numeFisier, "r");

    if (!f) {
        printf("Eroare: fisierul '%s' nu a putut fi deschis.\n", numeFisier);
        return;
    }

    int idStart, idStop;
    while (fscanf(f, "%d %d", &idStart, &idStop) == 2) {
        inserareMuchie(graf, idStart, idStop);
    }
    fclose(f);
}

void afisareListaVecini(NodP* graf, int id) {
    NodP* nod = cautaNodDupaID(graf, id);
    if (!nod) {
        printf("Nodul cu id=%d nu a fost gasit.\n", id);
        return;
    }
    printf("Vecinii nodului %d (%s):\n", id, nod->m.model);
    NodS* cap = nod->vecini;
    if (!cap) printf("  (niciun vecin)\n");
    while (cap) {
        afisareMasina(cap->info->m);
        cap = cap->next;
    }
}

void afisareGraf(NodP* graf) {
    NodP* p = graf;
    while (p) {
        printf("Nod %d (%s) -> vecini: ", p->m.id, p->m.model);
        NodS* v = p->vecini;
        if (!v) printf("(niciun vecin)");
        while (v) {
            printf("%d ", v->info->m.id);
            v = v->next;
        }
        printf("\n");
        p = p->next;
    }
}

/*
 * dezalocareNoduriGraf
 * Ordinea conteaza: mai intai lista secundara (vecinii) a fiecarui nod,
 * apoi campurile char* din Masina, apoi nodul principal.
 * Nu eliberam Masina-ele vecine - ele apartin listei principale,
 * nu listei secundare (NodS stocheaza doar un pointer, nu o copie).
 */
void dezalocareNoduriGraf(void* listaPrincipala) {
    NodP* p = (NodP*)listaPrincipala;
    while (p) {
        /* eliberam lista secundara (nodurile NodS, nu masinile din ele) */
        NodS* v = p->vecini;
        while (v) {
            NodS* tempS = v->next;
            free(v);
            v = tempS;
        }
        /* eliberam campurile dinamice din masina si nodul principal */
        NodP* tempP = p->next;
        free(p->m.model);
        free(p->m.numeSofer);
        free(p);
        p = tempP;
    }
}

/* ------------------------------------------------------------------- */

int main() {
    NodP* graf = citireNoduriMasiniDinFisier("masini.txt");
    citireMuchiiDinFisier("muchii.txt", graf);

    printf("=== Structura grafului ===\n");
    afisareGraf(graf);

    printf("\n=== Vecinii nodului 8 ===\n");
    afisareListaVecini(graf, 8);

    dezalocareNoduriGraf(graf);
    return 0;
}