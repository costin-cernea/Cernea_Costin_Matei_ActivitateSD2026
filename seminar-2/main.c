#include<stdio.h>
#include<stdlib.h>
#include<string.h>

struct Film {
    int id;
    int durata;
    char *denumire;
    float buget;
    char varstaMinima;
};

struct Film initializare(int id, int durata, char *denumire, float buget, char varstaMinima) {
    struct Film s;
    s.id = id;
    s.durata = durata;
    s.denumire = malloc(strlen(denumire) + 1);
    strcpy(s.denumire, denumire);
    s.buget = buget;
    s.varstaMinima = varstaMinima;
    return s;
}

void afisare(struct Film s) {
    printf("Id: %d\n", s.id);
    printf("Nume: %s\n", s.denumire);
    printf("Durata: %d\n", s.durata);
    printf("Buget: %.2f\n", s.buget);
    printf("VarstaMinima: %c\n", s.varstaMinima);
    printf("\n");
}

void afisareVector(struct Film *vector, int nrElemente) {
    for (int i = 0; i < nrElemente; i++) {
        afisare(vector[i]);
    }
}

struct Film copyFilm(struct Film s) {
    return initializare(s.id, s.durata, s.denumire, s.buget, s.varstaMinima);
}

struct Film *copiazaPrimeleNElemente(struct Film *vector, int nrElemente, int nrElementeCopiate) {
    struct Film *vectorNou = malloc(nrElementeCopiate * sizeof(struct Film));

    for (int i = 0; i < nrElementeCopiate; i++) {
        vectorNou[i] = copyFilm(vector[i]);
    }

    return vectorNou;
}

void dezalocare(struct Film **vector, int *nrElemente) {
    for (int i = 0; i < *nrElemente; i++) {
        free((*vector)[i].denumire);
    }

    free(*vector);

    *vector = NULL;
    *nrElemente = 0;
}

void copiazaAnumiteElemente(struct Film *vector, int nrElemente, float bugetMediu,
                            struct Film **vectorNou, int *dimensiune) {

    *dimensiune = 0;

    /* prima parcurgere - numaram elementele */
    for (int i = 0; i < nrElemente; i++) {
        if (vector[i].buget < bugetMediu) {
            (*dimensiune)++;
        }
    }

    *vectorNou = malloc((*dimensiune) * sizeof(struct Film));

    int k = 0;

    /* a doua parcurgere - copiem elementele */
    for (int i = 0; i < nrElemente; i++) {
        if (vector[i].buget < bugetMediu) {
            (*vectorNou)[k] = copyFilm(vector[i]);
            k++;
        }
    }
}

struct Film getPrimulElementConditionat(struct Film *vector, int nrElemente, const char *numCautat) {
    struct Film s;
    s.id = -1;

    for (int i = 0; i < nrElemente; i++) {
        if (strcmp(vector[i].denumire, numCautat) == 0) {
            return vector[i];
        }
    }

    return s;
}

int main() {

    struct Film f1 = initializare(1, 120, "Dune", 20.6, 'A');
    afisare(f1);

    int nrFilme = 3;

    struct Film *filme = malloc(nrFilme * sizeof(struct Film));

    filme[0] = f1;
    filme[1] = initializare(2, 130, "Fight Club", 56, 'B');
    filme[2] = initializare(3, 200, "Wuthering Heights", 80, 'C');

    afisareVector(filme, nrFilme);

    int nrFilmeCopiate = 2;

    struct Film *vectorNou = copiazaPrimeleNElemente(filme, nrFilme, nrFilmeCopiate);

    struct Film *filmeIeftine;

    float prag = 50;

    int dimIeftine = 0;

    copiazaAnumiteElemente(filme, nrFilme, prag, &filmeIeftine, &dimIeftine);

    afisareVector(filmeIeftine, dimIeftine);

    struct Film f2 = getPrimulElementConditionat(vectorNou, nrFilmeCopiate, "");

    afisare(f2);

    dezalocare(&filmeIeftine, &dimIeftine);
    dezalocare(&vectorNou, &nrFilmeCopiate);

    return 0;
}