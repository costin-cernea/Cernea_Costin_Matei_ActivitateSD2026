#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct StructuraMasina {
    int id;
    int nrUsi;
    float pret;
    char *model;
    char *numeSofer;
    unsigned char serie;
};
typedef struct StructuraMasina Masina;
typedef struct Nod {
    Masina info;
    struct Nod *next;
    struct Nod *prev;
} Nod;
typedef struct ListaDuble {
    Nod *first;
    Nod *last;
    int nrNoduri;
} ListaDubla;

Masina citireMasinaDinFisier(FILE *file) {
    char buffer[100];
    char sep[3] = ",\n";
    fgets(buffer, 100, file);
    char *aux;
    Masina m1;
    aux = strtok(buffer, sep);
    m1.id = atoi(aux);
    m1.nrUsi = atoi(strtok(NULL, sep));
    m1.pret = atof(strtok(NULL, sep));
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

void afisareListaMasini(ListaDubla listaDubla) {
    Nod *aux = listaDubla.first;
    while (aux) {
        afisareMasina(aux->info);
        aux = aux->next;
    }
}

void adaugaMasinaInLista(ListaDubla *listaDubla, Masina masinaNoua) {
    Nod *nodNou = (Nod *) malloc(sizeof(Nod));
    nodNou->info = masinaNoua;
    nodNou->next = NULL;
    nodNou->prev = listaDubla->last;
    if (listaDubla->last) {
        listaDubla->last->next = nodNou;
    } else {
        listaDubla->first = nodNou;
    }
    listaDubla->last = nodNou;
}

void adaugaLaInceputInLista(ListaDubla *listaDubla, Masina masinaNoua) {
    Nod *nodNou = (Nod *) malloc(sizeof(Nod));
    nodNou->info = masinaNoua;
    nodNou->next = listaDubla->first;
    nodNou->prev = NULL;
    if (listaDubla->first) {
        listaDubla->first->prev = nodNou;
    } else {
        listaDubla->last = nodNou;
    }
    listaDubla->first = nodNou;
}


ListaDubla citireLDMasiniDinFisier(const char *numeFisier) {
    FILE *file = fopen(numeFisier, "r");
    ListaDubla listaDubla;
    listaDubla.first = NULL;
    listaDubla.last = NULL;
    listaDubla.nrNoduri = 0;
    if (!file) {
        fprintf(stderr, "Nu pot deschide fisierul!\n");
        return listaDubla;
    }
    char peek;
    while ((peek = fgetc(file)) != EOF) {
        ungetc(peek, file);
        adaugaMasinaInLista(&listaDubla, citireMasinaDinFisier(file));
    }
    fclose(file);
    return listaDubla;
}


void dezalocareLDMasini(ListaDubla listaDubla) {
    Nod *aux;
    if (listaDubla.first) {
        aux = listaDubla.first->next;
        while (aux) {
            free(aux->prev->info.model);
            free(aux->prev->info.numeSofer);
            free(aux->prev);
            aux = aux->next;
        }
        free(listaDubla.last->info.model);
        free(listaDubla.last->info.numeSofer);
        free(listaDubla.last);
    }
}

float calculeazaPretMediu(ListaDubla listaDubla) {
    return 0;
}

void stergeMasinaDupaID(/*lista masini*/ int id) {
    //sterge masina cu id-ul primit.
}

char *getNumeSoferMasinaScumpa(/*lista dublu inlantuita*/) {
    return NULL;
}

int main() {
    ListaDubla listaDubla;
    listaDubla = citireLDMasiniDinFisier("masini.txt");
    afisareListaMasini(listaDubla);
    dezalocareLDMasini(listaDubla);
    return 0;
}
