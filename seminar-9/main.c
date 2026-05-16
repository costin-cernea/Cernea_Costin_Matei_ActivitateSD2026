

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
 * Fiecare nod retine si inaltimea subarborelui sau.
 * Asta ne permite sa calculam factorul de echilibru in O(1)
 * fara sa parcurgem arborele de fiecare data.
 */
struct Nod {
    Masina info;
    int inaltime;
    struct Nod* stanga;
    struct Nod* dreapta;
};
typedef struct Nod Nod;

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
/* Functii ajutatoare AVL                                               */
/* ------------------------------------------------------------------- */

static int inaltimeNod(Nod* nod) {
    return nod ? nod->inaltime : -1;
}

static int max2(int a, int b) {
    return a > b ? a : b;
}

static void actualizeazaInaltime(Nod* nod) {
    nod->inaltime = 1 + max2(inaltimeNod(nod->stanga), inaltimeNod(nod->dreapta));
}

/* Factorul de echilibru = inaltime(stanga) - inaltime(dreapta).
 * > 1  => subarborele stang e prea inalt  -> rotatie dreapta
 * < -1 => subarborele drept e prea inalt  -> rotatie stanga  */
static int factorEchilibru(Nod* nod) {
    return inaltimeNod(nod->stanga) - inaltimeNod(nod->dreapta);
}

/*
 * Rotatii AVL
 *
 * Rotatie dreapta (caz stanga-stanga):
 *       z                y
 *      / \             /   \
 *     y   T4   =>    x       z
 *    / \            / \     / \
 *   x   T3         T1  T2  T3  T4
 */
static Nod* rotatieDreapta(Nod* z) {
    Nod* y  = z->stanga;
    Nod* T3 = y->dreapta;

    y->dreapta = z;
    z->stanga  = T3;

    actualizeazaInaltime(z);
    actualizeazaInaltime(y);
    return y;  /* noua radacina a subarborelui */
}

/* Rotatie stanga (caz dreapta-dreapta): oglinda rotatiei dreapta */
static Nod* rotatieStanga(Nod* z) {
    Nod* y  = z->dreapta;
    Nod* T2 = y->stanga;

    y->stanga  = z;
    z->dreapta = T2;

    actualizeazaInaltime(z);
    actualizeazaInaltime(y);
    return y;
}

/* ------------------------------------------------------------------- */

/*
 * adaugaMasinaInArboreEchilibrat
 * Insertie BST clasica, urmata de reechilibrare AVL pe drumul de intoarcere.
 * Returneaza radacina (posibil schimbata de o rotatie).
 *
 * Cele 4 cazuri de dezechilibru:
 *   LL (stanga-stanga)  -> rotatie dreapta
 *   RR (dreapta-dreapta)-> rotatie stanga
 *   LR (stanga-dreapta) -> rotatie stanga pe copil, apoi dreapta pe nod
 *   RL (dreapta-stanga) -> rotatie dreapta pe copil, apoi stanga pe nod
 */
Nod* adaugaMasinaInArboreEchilibrat(Nod* radacina, Masina masinaNoua) {
    /* Insertie BST normala */
    if (radacina == NULL) {
        Nod* nou     = malloc(sizeof(Nod));
        nou->info    = masinaNoua;
        nou->inaltime = 0;
        nou->stanga  = NULL;
        nou->dreapta = NULL;
        return nou;
    }

    if (masinaNoua.id < radacina->info.id) {
        radacina->stanga = adaugaMasinaInArboreEchilibrat(radacina->stanga, masinaNoua);
    } else if (masinaNoua.id > radacina->info.id) {
        radacina->dreapta = adaugaMasinaInArboreEchilibrat(radacina->dreapta, masinaNoua);
    } else {
        return radacina;  /* id duplicat, nu inseram */
    }

    actualizeazaInaltime(radacina);
    int fe = factorEchilibru(radacina);

    /* LL */
    if (fe > 1 && masinaNoua.id < radacina->stanga->info.id)
        return rotatieDreapta(radacina);

    /* RR */
    if (fe < -1 && masinaNoua.id > radacina->dreapta->info.id)
        return rotatieStanga(radacina);

    /* LR */
    if (fe > 1 && masinaNoua.id > radacina->stanga->info.id) {
        radacina->stanga = rotatieStanga(radacina->stanga);
        return rotatieDreapta(radacina);
    }

    /* RL */
    if (fe < -1 && masinaNoua.id < radacina->dreapta->info.id) {
        radacina->dreapta = rotatieDreapta(radacina->dreapta);
        return rotatieStanga(radacina);
    }

    return radacina;
}

void* citireArboreDeMasiniDinFisier(const char* numeFisier) {
    FILE* file = fopen(numeFisier, "r");
    Nod* radacina = NULL;

    if (!file) {
        printf("Eroare: fisierul '%s' nu a putut fi deschis.\n", numeFisier);
        return NULL;
    }

    while (!feof(file)) {
        Masina m = citireMasinaDinFisier(file);
        if (m.id != -1) {
            radacina = adaugaMasinaInArboreEchilibrat(radacina, m);
        }
    }
    fclose(file);
    return radacina;
}

/* Cele trei parcurgeri - identice cu BST-ul simplu */
void afisareMasiniInOrdine(Nod* radacina) {
    if (radacina) {
        afisareMasiniInOrdine(radacina->stanga);
        afisareMasina(radacina->info);
        afisareMasiniInOrdine(radacina->dreapta);
    }
}

void afisareMasiniPreOrdine(Nod* radacina) {
    if (radacina) {
        afisareMasina(radacina->info);
        afisareMasiniPreOrdine(radacina->stanga);
        afisareMasiniPreOrdine(radacina->dreapta);
    }
}

void afisareMasiniPostOrdine(Nod* radacina) {
    if (radacina) {
        afisareMasiniPostOrdine(radacina->stanga);
        afisareMasiniPostOrdine(radacina->dreapta);
        afisareMasina(radacina->info);
    }
}

void afisareMasiniDinArbore(Nod* radacina) {
    printf("=== InOrdine (sortat dupa id) ===\n");
    afisareMasiniInOrdine(radacina);
    printf("=== PreOrdine ===\n");
    afisareMasiniPreOrdine(radacina);
    printf("=== PostOrdine ===\n");
    afisareMasiniPostOrdine(radacina);
}

void dezalocareArboreDeMasini(Nod** radacina) {
    if (*radacina) {
        dezalocareArboreDeMasini(&(*radacina)->stanga);
        dezalocareArboreDeMasini(&(*radacina)->dreapta);
        free((*radacina)->info.model);
        free((*radacina)->info.numeSofer);
        free(*radacina);
        *radacina = NULL;
    }
}

/* ------------------------------------------------------------------- */
/* Functii preluate din seminarul 8 - functioneaza identic pe AVL      */
/* ------------------------------------------------------------------- */

Masina getMasinaByID(Nod* radacina, int id) {
    Masina m;
    m.id = -1;
    m.model = NULL;
    m.numeSofer = NULL;

    if (radacina == NULL) return m;

    if (radacina->info.id == id) {
        m = radacina->info;
        m.model = malloc(strlen(radacina->info.model) + 1);
        strcpy(m.model, radacina->info.model);
        m.numeSofer = malloc(strlen(radacina->info.numeSofer) + 1);
        strcpy(m.numeSofer, radacina->info.numeSofer);
        return m;
    }

    if (id < radacina->info.id)
        return getMasinaByID(radacina->stanga, id);
    else
        return getMasinaByID(radacina->dreapta, id);
}

int determinaNumarNoduri(Nod* radacina) {
    if (!radacina) return 0;
    return 1 + determinaNumarNoduri(radacina->stanga)
             + determinaNumarNoduri(radacina->dreapta);
}

int calculeazaInaltimeArbore(Nod* radacina) {
    return inaltimeNod(radacina);
}

float calculeazaPretTotal(Nod* radacina) {
    if (!radacina) return 0;
    return radacina->info.pret
         + calculeazaPretTotal(radacina->stanga)
         + calculeazaPretTotal(radacina->dreapta);
}

float calculeazaPretulMasinilorUnuiSofer(Nod* radacina, const char* numeSofer) {
    if (!radacina) return 0;
    float total = 0;
    if (strcmp(radacina->info.numeSofer, numeSofer) == 0)
        total += radacina->info.pret;
    total += calculeazaPretulMasinilorUnuiSofer(radacina->stanga, numeSofer);
    total += calculeazaPretulMasinilorUnuiSofer(radacina->dreapta, numeSofer);
    return total;
}

/* ------------------------------------------------------------------- */

int main() {
    Nod* arbore = citireArboreDeMasiniDinFisier("masini.txt");

    afisareMasiniDinArbore(arbore);

    printf("Numar noduri: %d\n", determinaNumarNoduri(arbore));
    printf("Inaltime AVL: %d\n", calculeazaInaltimeArbore(arbore));
    printf("Pret total: %.2f\n", calculeazaPretTotal(arbore));
    printf("Pret total Ionescu: %.2f\n",
           calculeazaPretulMasinilorUnuiSofer(arbore, "Ionescu"));

    Masina gasita = getMasinaByID(arbore, 5);
    if (gasita.id != -1) {
        printf("=== Masina cu id=5 ===\n");
        afisareMasina(gasita);
        free(gasita.model);
        free(gasita.numeSofer);
    }

    dezalocareArboreDeMasini(&arbore);
    return 0;
}