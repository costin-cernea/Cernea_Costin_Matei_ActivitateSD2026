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
} Nod;

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

void afisareListaMasini(Nod *cap) {
    while (cap != NULL) {
        afisareMasina(cap->info);
        cap = cap->next;
    }
}

void adaugaMasinaInLista(Nod **cap, Masina masinaNoua) {
    Nod *temp = (Nod *) malloc(sizeof(Nod));
    temp->info = masinaNoua;
    temp->next = NULL;
    if (!(*cap)) {
        *cap = temp;
    } else {
        Nod *p = *cap;
        while (p->next) {
            p = p->next;
        }
        p->next = temp;
    }
}

void adaugaLaInceputInLista(Nod *cap, Masina masinaNoua) {
    //adauga la inceputul listei o noua masina pe care o primim ca parametru
}

// ✅ MODIFICAT: înlocuit while(!feof) cu verificare explicită de EOF
void *citireListaMasiniDinFisier(const char *numeFisier) {
    Nod *cap = NULL;
    FILE *file = fopen(numeFisier, "r");
    if (!file) {
        fprintf(stderr, "Nu pot deschide fisierul!\n");
        return NULL;
    }
    char peek;
    while ((peek = fgetc(file)) != EOF) {
        ungetc(peek, file);
        adaugaMasinaInLista(&cap, citireMasinaDinFisier(file));
    }
    fclose(file);
    return cap;
}

void dezalocareListaMasini(Nod **temp) {
    while (*temp) {
        free((*temp)->info.model);
        free((*temp)->info.numeSofer);
        Nod *nou = (*temp)->next;
        free(*temp);
        *temp = nou;
    }
}

float calculeazaPretMediu(/*lista de masini*/) {
    return 0;
}

void stergeMasiniDinSeria(/*lista masini*/ char serieCautata) {
}

float calculeazaPretulMasinilorUnuiSofer(/*lista masini*/ const char *numeSofer) {
    return 0;
}

int main() {
    Nod *cap = citireListaMasiniDinFisier("masini.txt");
    afisareListaMasini(cap);
    dezalocareListaMasini(&cap);
    return 0;
}
