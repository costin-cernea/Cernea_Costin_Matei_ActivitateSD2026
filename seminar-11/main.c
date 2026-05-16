

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

typedef struct NodArbore {
    Masina info;
    struct NodArbore* right;
    struct NodArbore* left;
} NodArbore;

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
/* Functii AVL                                                          */
/* ------------------------------------------------------------------- */

/*
 * calculeazaInaltimeArbore returneaza 0 pentru NULL si 1+ pentru noduri.
 * verificaEchilibru = inaltime(stanga) - inaltime(dreapta):
 *   2  => dezechilibru stanga
 *  -2  => dezechilibru dreapta
 */
int calculeazaInaltimeArbore(NodArbore* root) {
    if (root == NULL) return 0;
    int dr = calculeazaInaltimeArbore(root->right);
    int st = calculeazaInaltimeArbore(root->left);
    return 1 + (dr > st ? dr : st);
}

int verificaEchilibru(NodArbore* root) {
    return calculeazaInaltimeArbore(root->left)
         - calculeazaInaltimeArbore(root->right);
}

void rotireStanga(NodArbore** root) {
    NodArbore* aux = (*root)->right;
    (*root)->right = aux->left;
    aux->left      = (*root);
    (*root)        = aux;
}

void rotireDreapta(NodArbore** root) {
    NodArbore* aux = (*root)->left;
    (*root)->left  = aux->right;
    aux->right     = (*root);
    (*root)        = aux;
}

void adaugaMasinaInArboreEchilibrat(NodArbore** root, Masina masinaNoua) {
    if (*root == NULL) {
        NodArbore* nou = malloc(sizeof(NodArbore));
        nou->left  = NULL;
        nou->right = NULL;
        nou->info  = masinaNoua;
        *root = nou;
        return;
    }

    if (masinaNoua.id > (*root)->info.id) {
        adaugaMasinaInArboreEchilibrat(&(*root)->right, masinaNoua);
    } else if (masinaNoua.id < (*root)->info.id) {
        adaugaMasinaInArboreEchilibrat(&(*root)->left, masinaNoua);
    } else {
        return;  /* id duplicat */
    }

    int fe = verificaEchilibru(*root);

    if (fe == -2) {
        /* Dezechilibru dreapta */
        if (verificaEchilibru((*root)->right) == 1) {
            /* Caz RL: rotatie dreapta pe copil, apoi stanga pe nod */
            rotireDreapta(&(*root)->right);
        }
        rotireStanga(root);
    } else if (fe == 2) {
        /* Dezechilibru stanga */
        if (verificaEchilibru((*root)->left) == -1) {
            /* Caz LR: rotatie stanga pe copil, apoi dreapta pe nod */
            rotireStanga(&(*root)->left);
        }
        rotireDreapta(root);
    }
}

NodArbore* citireArboreDeMasiniDinFisier(const char* numeFisier) {
    NodArbore* root = NULL;
    FILE* file = fopen(numeFisier, "r");

    if (!file) {
        printf("Eroare: fisierul '%s' nu a putut fi deschis.\n", numeFisier);
        return NULL;
    }

    while (!feof(file)) {
        Masina m = citireMasinaDinFisier(file);
        if (m.id != -1) {
            adaugaMasinaInArboreEchilibrat(&root, m);
        }
    }
    fclose(file);
    return root;
}

/* Cele trei parcurgeri */
void afisareMasiniInOrdine(NodArbore* root) {   /* stanga, radacina, dreapta */
    if (root) {
        afisareMasiniInOrdine(root->left);
        afisareMasina(root->info);
        afisareMasiniInOrdine(root->right);
    }
}

void afisareMasiniPreOrdine(NodArbore* root) {  /* radacina, stanga, dreapta */
    if (root) {
        afisareMasina(root->info);
        afisareMasiniPreOrdine(root->left);
        afisareMasiniPreOrdine(root->right);
    }
}

void afisareMasiniPostOrdine(NodArbore* root) { /* stanga, dreapta, radacina */
    if (root) {
        afisareMasiniPostOrdine(root->left);
        afisareMasiniPostOrdine(root->right);
        afisareMasina(root->info);
    }
}

void afisareMasiniDinArbore(NodArbore* root) {
    printf("=== InOrdine ===\n");
    afisareMasiniInOrdine(root);
    printf("=== PreOrdine ===\n");
    afisareMasiniPreOrdine(root);
    printf("=== PostOrdine ===\n");
    afisareMasiniPostOrdine(root);
}

void dezalocareArboreDeMasini(NodArbore** root) {
    if (*root) {
        dezalocareArboreDeMasini(&(*root)->left);
        dezalocareArboreDeMasini(&(*root)->right);
        free((*root)->info.model);
        free((*root)->info.numeSofer);
        free(*root);
        *root = NULL;
    }
}

/* ------------------------------------------------------------------- */
/* Functii preluate din seminarele precedente                           */
/* ------------------------------------------------------------------- */

Masina getMasinaByID(NodArbore* root, int id) {
    Masina m;
    m.id = -1;
    m.model = NULL;
    m.numeSofer = NULL;

    if (root == NULL) return m;

    if (root->info.id == id) {
        m = root->info;
        m.model = malloc(strlen(root->info.model) + 1);
        strcpy(m.model, root->info.model);
        m.numeSofer = malloc(strlen(root->info.numeSofer) + 1);
        strcpy(m.numeSofer, root->info.numeSofer);
        return m;
    }

    if (id > root->info.id)
        return getMasinaByID(root->right, id);
    else
        return getMasinaByID(root->left, id);
}

int determinaNumarNoduri(NodArbore* root) {
    if (!root) return 0;
    return 1 + determinaNumarNoduri(root->left)
             + determinaNumarNoduri(root->right);
}

float calculeazaPretTotal(NodArbore* root) {
    if (!root) return 0;
    return root->info.pret
         + calculeazaPretTotal(root->left)
         + calculeazaPretTotal(root->right);
}

float calculeazaPretulMasinilorUnuiSofer(NodArbore* root, const char* numeSofer) {
    if (!root) return 0;
    float total = 0;
    if (strcmp(root->info.numeSofer, numeSofer) == 0)
        total += root->info.pret;
    total += calculeazaPretulMasinilorUnuiSofer(root->left, numeSofer);
    total += calculeazaPretulMasinilorUnuiSofer(root->right, numeSofer);
    return total;
}

/* ------------------------------------------------------------------- */

int main() {
    NodArbore* root = citireArboreDeMasiniDinFisier("masini.txt");

    afisareMasiniDinArbore(root);

    printf("Numar noduri: %d\n", determinaNumarNoduri(root));
    printf("Inaltime AVL: %d\n", calculeazaInaltimeArbore(root));
    printf("Pret total: %.2f\n", calculeazaPretTotal(root));
    printf("Pret total Ionescu: %.2f\n",
           calculeazaPretulMasinilorUnuiSofer(root, "Ionescu"));

    Masina gasita = getMasinaByID(root, 6);
    if (gasita.id != -1) {
        printf("=== Masina cu id=6 ===\n");
        afisareMasina(gasita);
        free(gasita.model);
        free(gasita.numeSofer);
    }

    dezalocareArboreDeMasini(&root);
    return 0;
}