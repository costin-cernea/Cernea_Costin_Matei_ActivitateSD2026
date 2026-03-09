#define _CRT_SECURE_NO_WARNINGS
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

void afisareMasina(Masina masina) {
	printf("\nID: %d", masina.id);
	printf("\nNumar usi: %d", masina.nrUsi);
	printf("\nPret: %5.2f", masina.pret);
	printf("\nModel: %s", masina.model);
	printf("\nNume sofer: %s", masina.numeSofer);
	printf("\nSerie: %c", masina.serie);
	printf("\n");
}

void afisareVectorMasini(Masina* masini, int nrMasini) {
	for (int i = 0; i < nrMasini; i++) {
		afisareMasina(masini[i]);
	}
}

void adaugaMasinaInVector(Masina** masini, int* nrMasini, Masina masinaNoua) {

	Masina* aux = (Masina*)malloc(sizeof(Masina) * ((*nrMasini) + 1));

	for (int i = 0; i < *nrMasini; i++) {
		aux[i] = (*masini)[i]; // shallow copy
	}

	aux[*nrMasini] = masinaNoua;

	free(*masini);
	*masini = aux;

	(*nrMasini)++;
}

Masina citireMasinaFisier(FILE* file) {

	Masina m;

	char linie[100];

	if (fgets(linie, 100, file) == NULL) {
		m.id = -1;
		return m;
	}

	char delimitator[] = ",\n";

	m.id = atoi(strtok(linie, delimitator));
	m.nrUsi = atoi(strtok(NULL, delimitator));
	m.pret = (float)atof(strtok(NULL, delimitator));

	char* model = strtok(NULL, delimitator);
	m.model = (char*)malloc(strlen(model) + 1);
	strcpy(m.model, model);

	char* numeSofer = strtok(NULL, delimitator);
	m.numeSofer = (char*)malloc(strlen(numeSofer) + 1);
	strcpy(m.numeSofer, numeSofer);

	char* serie = strtok(NULL, delimitator);
	m.serie = serie[0];

	return m;
}

Masina* citireVectorMasiniFisier(const char* numeFisier, int* nrMasiniCitite) {

	*nrMasiniCitite = 0;

	Masina* masini = NULL;

	FILE* file = fopen(numeFisier, "r");

	if (file == NULL) {
		printf("Fisierul nu a putut fi deschis!\n");
		return NULL;
	}

	while (1) {

		Masina m = citireMasinaFisier(file);

		if (m.id == -1) {
			break;
		}

		adaugaMasinaInVector(&masini, nrMasiniCitite, m);
	}

	fclose(file);

	return masini;
}

void dezalocareVectorMasini(Masina** vector, int* nrMasini) {

	for (int i = 0; i < *nrMasini; i++) {
		free((*vector)[i].model);
		free((*vector)[i].numeSofer);
	}

	free(*vector);

	*vector = NULL;
	*nrMasini = 0;
}

int main() {

	int nrMasini = 0;

	Masina* masini = citireVectorMasiniFisier("masini.txt", &nrMasini);

	afisareVectorMasini(masini, nrMasini);

	dezalocareVectorMasini(&masini, &nrMasini);

	return 0;
}