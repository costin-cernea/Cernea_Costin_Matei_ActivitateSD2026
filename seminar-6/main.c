/*
 * Fisier: masini.c
 * Necesita un fisier "masini.txt" in acelasi director cu executabilul.
 * Format fiecare linie: id,nrUsi,pret,model,numeSofer,serie
 * Exemplu: 1,4,25000.00,Dacia,Ion,A
 *
 * Compilare (Terminal): gcc masini.c -o masini
 * Rulare:               ./masini
 */

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
typedef struct Nod Nod;

struct Nod {
    Masina info;
    Nod* next;
};

struct HashTable {
    int dim;
    Nod** vector;
};
typedef struct HashTable HashTable;

/* -------------------------------------------------------------------
 * citireMasinaDinFisier
 * Citeste o linie din fisier si o parseaza in campurile structurii.
 * strtok imparte sirul dupa separatorii dati (virgula si newline).
 * Primul apel primeste sirul, urmatoarele primesc NULL ca sa continue
 * din locul unde s-a oprit.
 *
 * fgets returneaza NULL la EOF sau eroare - verificam asta inainte
 * de orice strtok, altfel programul crapa cu segfault pe buffer gol.
 * Daca linia e invalida, returnam o masina cu id = -1 ca santinela.
 * ------------------------------------------------------------------- */
Masina citireMasinaDinFisier(FILE* file) {
    char buffer[100];
    char sep[3] = ",\n";
    Masina m1;
    m1.id = -1;
    m1.model = NULL;
    m1.numeSofer = NULL;

    if (!fgets(buffer, 100, file)) return m1;  /* EOF sau eroare */

    char* aux = strtok(buffer, sep);
    if (!aux) return m1;  /* linie goala */

    m1.id = atoi(aux);
    m1.nrUsi = atoi(strtok(NULL, sep));
    m1.pret = atof(strtok(NULL, sep));

    /* malloc aloca memorie pe heap; +1 pentru caracterul '\0' de la sfarsit */
    aux = strtok(NULL, sep);
    m1.model = malloc(strlen(aux) + 1);
    strcpy(m1.model, aux);

    aux = strtok(NULL, sep);
    m1.numeSofer = malloc(strlen(aux) + 1);
    strcpy(m1.numeSofer, aux);

    /* *strtok(...) dereferentiaza pointerul ca sa obtinem primul caracter */
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

void afisareListaMasini(Nod* lista) {
    while (lista) {
        afisareMasina(lista->info);
        lista = lista->next;
    }
}

/* -------------------------------------------------------------------
 * adaugaMasinaInLista
 * Primeste un pointer la pointer (**lista) ca sa poata modifica
 * chiar capul listei atunci cand aceasta e goala.
 * ------------------------------------------------------------------- */
void adaugaMasinaInLista(Nod** lista, Masina masinaNoua) {
    Nod* nou = malloc(sizeof(Nod));
    nou->info = masinaNoua;
    nou->next = NULL;

    if (*lista == NULL) {
        *lista = nou;
    } else {
        Nod* p = *lista;
        while (p->next) {
            p = p->next;
        }
        p->next = nou;
    }
}

/* -------------------------------------------------------------------
 * initializareHashTable
 * Aloca un vector de pointeri la Nod (fiecare slot e o lista inlantuita).
 * Initial toate sloturile sunt NULL (lista vida).
 * ------------------------------------------------------------------- */
HashTable initializareHashTable(int dimensiune) {
    HashTable ht;
    ht.dim = dimensiune;
    ht.vector = (Nod**)malloc(sizeof(Nod*) * dimensiune);
    for (int i = 0; i < dimensiune; i++) {
        ht.vector[i] = NULL;
    }
    return ht;
}

/* -------------------------------------------------------------------
 * calculeazaHash
 * Functia de dispersie: mapeaza id-ul la un index din [0, dimensiune).
 * Coliziunile (doua id-uri cu acelasi hash) sunt rezolvate prin
 * inlantuire (chaining) - masinile ajung in aceeasi lista.
 * ------------------------------------------------------------------- */
int calculeazaHash(int id, int dimensiune) {
    return (id * 3) % dimensiune;
}

void inserareMasinaInTabela(HashTable hash, Masina masina) {
    int hashCode = calculeazaHash(masina.id, hash.dim);
    /* Indiferent daca avem coliziune sau nu, adaugam in lista de pe slot */
    adaugaMasinaInLista(&hash.vector[hashCode], masina);
}

/* -------------------------------------------------------------------
 * citireMasiniDinFisier
 * Bucla foloseste rezultatul lui citireMasinaDinFisier ca santinela:
 * daca id == -1, linia a fost invalida (goala sau EOF) si o sarim.
 * Asta rezolva bug-ul clasic cu !feof() care mai facea un apel
 * in plus dupa ultima linie valida si provoca segfault.
 * ------------------------------------------------------------------- */
HashTable citireMasiniDinFisier(const char* numeFisier) {
    FILE* file = fopen(numeFisier, "r");
    HashTable hash = initializareHashTable(4);
    if (file) {
        while (!feof(file)) {
            Masina masinaCitita = citireMasinaDinFisier(file);
            if (masinaCitita.id != -1) {
                inserareMasinaInTabela(hash, masinaCitita);
            }
        }
        fclose(file);
    } else {
        printf("Eroare: fisierul '%s' nu a putut fi deschis.\n", numeFisier);
    }
    return hash;
}

void afisareTabelaDeMasini(HashTable ht) {
    for (int i = 0; i < ht.dim; i++) {
        printf("Cluster %d:\n", i + 1);
        afisareListaMasini(ht.vector[i]);
        printf("\n____________________\n");
    }
}

/* -------------------------------------------------------------------
 * dezalocareTabelaDeMasini
 * Pentru fiecare slot parcurgem lista inlantuita nod cu nod.
 * Ordinea de free conteaza: mai intai campurile char* din interiorul
 * structurii Masina, apoi nodul in sine, altfel pierdem referinta
 * la date inainte sa le eliberam (memory leak).
 * La final eliberam si vectorul de pointeri si resetam dimensiunea.
 * ------------------------------------------------------------------- */
void dezalocareTabelaDeMasini(HashTable* ht) {
    for (int i = 0; i < ht->dim; i++) {
        Nod* curent = ht->vector[i];
        while (curent) {
            Nod* urmator = curent->next;  /* salvam next inainte de free */
            free(curent->info.model);
            free(curent->info.numeSofer);
            free(curent);
            curent = urmator;
        }
        ht->vector[i] = NULL;
    }
    free(ht->vector);
    ht->vector = NULL;
    ht->dim = 0;
}

/* -------------------------------------------------------------------
 * calculeazaPreturiMediiPerClustere
 * Parcurge toate sloturile. Pentru fiecare slot nevid, calculeaza
 * suma preturilor si numarul de masini, apoi imparte.
 * Returneaza un vector alocat dinamic de dimensiune *nrClustere,
 * unde *nrClustere = numarul de sloturi care contin cel putin o masina.
 * Indicele din vectorul rezultat corespunde ordinii in care sunt
 * intalnite sloturile nevide (nu indexului din hash table).
 * Apelatorul este responsabil sa elibereze memoria returnata (free).
 * ------------------------------------------------------------------- */
float* calculeazaPreturiMediiPerClustere(HashTable ht, int* nrClustere) {
    *nrClustere = 0;

    /* Prima trecere: numaram cate sloturi sunt nevide */
    for (int i = 0; i < ht.dim; i++) {
        if (ht.vector[i] != NULL) {
            (*nrClustere)++;
        }
    }

    if (*nrClustere == 0) return NULL;

    float* medii = (float*)malloc(sizeof(float) * (*nrClustere));
    int idx = 0;

    /* A doua trecere: calculam media pentru fiecare slot nevid */
    for (int i = 0; i < ht.dim; i++) {
        if (ht.vector[i] != NULL) {
            float suma = 0.0f;
            int contor = 0;
            Nod* p = ht.vector[i];
            while (p) {
                suma += p->info.pret;
                contor++;
                p = p->next;
            }
            medii[idx++] = suma / contor;
        }
    }

    return medii;
}

/* -------------------------------------------------------------------
 * getMasinaDupaCheie
 * Cauta in slotul corespunzator hash-ului si parcurge lista pana
 * gaseste id-ul dorit. Returneaza o copie profunda (deep copy) a
 * structurii, adica aloca memorie noua pentru campurile char*.
 * Daca masina nu e gasita, returneaza o masina cu id = -1.
 * ------------------------------------------------------------------- */
Masina getMasinaDupaCheie(HashTable ht, int id) {
    Masina m;
    m.id = -1;
    m.model = NULL;
    m.numeSofer = NULL;

    int hashCode = calculeazaHash(id, ht.dim);
    Nod* cautare = ht.vector[hashCode];
    while (cautare) {
        if (cautare->info.id == id) {
            m = cautare->info;
            m.numeSofer = (char*)malloc((strlen(cautare->info.numeSofer) + 1) * sizeof(char));
            strcpy(m.numeSofer, cautare->info.numeSofer);
            m.model = (char*)malloc((strlen(cautare->info.model) + 1) * sizeof(char));
            strcpy(m.model, cautare->info.model);
            return m;
        }
        cautare = cautare->next;
    }
    return m;
}

int main() {
    HashTable hash = citireMasiniDinFisier("masini.txt");
    afisareTabelaDeMasini(hash);

    Masina test = getMasinaDupaCheie(hash, 8);
    if (test.id != -1) {
        afisareMasina(test);
        free(test.model);
        free(test.numeSofer);
    } else {
        printf("Masina cu id=8 nu a fost gasita.\n");
    }

    /* Preturile medii per cluster */
    int nrClustere = 0;
    float* medii = calculeazaPreturiMediiPerClustere(hash, &nrClustere);
    printf("Preturi medii per cluster:\n");
    for (int i = 0; i < nrClustere; i++) {
        printf("  Cluster nevid %d: %.2f\n", i + 1, medii[i]);
    }
    free(medii);

    dezalocareTabelaDeMasini(&hash);

    return 0;
}