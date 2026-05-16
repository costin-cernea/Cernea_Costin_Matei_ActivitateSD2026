

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

struct Nod {
    Masina info;
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

void adaugaMasinaInArbore(Nod** radacina, Masina masinaNoua) {
    if (*radacina == NULL) {
        Nod* nou    = malloc(sizeof(Nod));
        nou->info   = masinaNoua;
        nou->stanga = NULL;
        nou->dreapta = NULL;
        *radacina   = nou;
        return;
    }
    if (masinaNoua.id < (*radacina)->info.id) {
        adaugaMasinaInArbore(&(*radacina)->stanga, masinaNoua);
    } else {
        adaugaMasinaInArbore(&(*radacina)->dreapta, masinaNoua);
    }
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
            adaugaMasinaInArbore(&radacina, m);
        }
    }
    fclose(file);
    return radacina;
}

/*
 * Cele trei moduri clasice de parcurgere a unui arbore binar:
 *
 * InOrdine    (stanga, radacina, dreapta) - produce elementele sortate dupa cheie
 * PostOrdine  (stanga, dreapta, radacina) - util la dezalocare
 * PreOrdine   (radacina, stanga, dreapta) - util la copiere/serializare
 */
void afisareMasiniDinArbore(Nod* radacina) {  /* InOrdine */
    if (radacina) {
        afisareMasiniDinArbore(radacina->stanga);
        afisareMasina(radacina->info);
        afisareMasiniDinArbore(radacina->dreapta);
    }
}

void afisareMasinaPostOrdine(Nod* radacina) {
    if (radacina) {
        afisareMasinaPostOrdine(radacina->stanga);
        afisareMasinaPostOrdine(radacina->dreapta);
        afisareMasina(radacina->info);
    }
}

void afisareMasinaPreOrdine(Nod* radacina) {
    if (radacina) {
        afisareMasina(radacina->info);
        afisareMasinaPreOrdine(radacina->stanga);
        afisareMasinaPreOrdine(radacina->dreapta);
    }
}

/*
 * dezalocareArboreDeMasini
 * Parcurgere PostOrdine: eliberam mai intai subarborii,
 * abia apoi nodul curent (altfel pierdem referintele la copii).
 * Originalul avea un bug: dezaloca radacina->stanga de doua ori
 * in loc de stanga si dreapta.
 */
void dezalocareArboreDeMasini(Nod** radacina) {
    if (*radacina) {
        dezalocareArboreDeMasini(&(*radacina)->stanga);
        dezalocareArboreDeMasini(&(*radacina)->dreapta);  /* fix: era stanga din nou */
        free((*radacina)->info.model);
        free((*radacina)->info.numeSofer);
        free(*radacina);
        *radacina = NULL;
    }
}

/*
 * getMasinaByID
 * Exploateaza proprietatea BST: daca id-ul cautat e mai mic decat
 * nodul curent mergem stanga, altfel dreapta. O(h) unde h = inaltimea.
 * Returneaza o copie profunda a masinii gasite, sau masina cu id=-1.
 */
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

    if (id < radacina->info.id) {
        return getMasinaByID(radacina->stanga, id);
    } else {
        return getMasinaByID(radacina->dreapta, id);
    }
}

int determinaNumarNoduri(Nod* radacina) {
    if (radacina) {
        return determinaNumarNoduri(radacina->stanga)
             + determinaNumarNoduri(radacina->dreapta) + 1;
    }
    return 0;
}

int calculeazaMaxim(int a, int b) {
    return a > b ? a : b;
}

int calculeazaInaltimeArbore(Nod* radacina) {
    if (radacina == NULL) return -1;
    return 1 + calculeazaMaxim(
        calculeazaInaltimeArbore(radacina->stanga),
        calculeazaInaltimeArbore(radacina->dreapta)
    );
}

float calculeazaPretTotal(Nod* radacina) {
    if (radacina == NULL) return 0;
    return radacina->info.pret
         + calculeazaPretTotal(radacina->stanga)
         + calculeazaPretTotal(radacina->dreapta);
}

/*
 * calculeazaPretulMasinilorUnuiSofer
 * Parcurge intregul arbore (nu putem folosi proprietatea BST aici,
 * deoarece cheia e id-ul, nu numele soferului).
 * Adunam pretul doar daca numeSofer coincide cu cel cautat.
 */
float calculeazaPretulMasinilorUnuiSofer(Nod* radacina, const char* numeSofer) {
    if (radacina == NULL) return 0;

    float total = 0;
    if (strcmp(radacina->info.numeSofer, numeSofer) == 0) {
        total += radacina->info.pret;
    }

    total += calculeazaPretulMasinilorUnuiSofer(radacina->stanga, numeSofer);
    total += calculeazaPretulMasinilorUnuiSofer(radacina->dreapta, numeSofer);
    return total;
}

/* ------------------------------------------------------------------- */

int main() {
    Nod* arbore = citireArboreDeMasiniDinFisier("masini.txt");

    printf("=== InOrdine (sortat dupa id) ===\n");
    afisareMasiniDinArbore(arbore);

    printf("=== PreOrdine ===\n");
    afisareMasinaPreOrdine(arbore);

    printf("=== PostOrdine ===\n");
    afisareMasinaPostOrdine(arbore);

    printf("Numar total noduri: %d\n", determinaNumarNoduri(arbore));
    printf("Inaltime: %d\n", calculeazaInaltimeArbore(arbore));
    printf("Pret total: %.2f\n", calculeazaPretTotal(arbore));

    float pretIonescu = calculeazaPretulMasinilorUnuiSofer(arbore, "Ionescu");
    printf("Pret total Ionescu: %.2f\n", pretIonescu);

    Masina gasita = getMasinaByID(arbore, 3);
    if (gasita.id != -1) {
        printf("=== Masina cu id=3 ===\n");
        afisareMasina(gasita);
        free(gasita.model);
        free(gasita.numeSofer);
    }

    dezalocareArboreDeMasini(&arbore);
    return 0;
}