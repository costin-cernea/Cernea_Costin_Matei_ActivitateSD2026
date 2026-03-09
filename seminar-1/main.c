#include<stdio.h>
#include<stdlib.h>
#include<string.h>

struct Joc {
	int id;
	int numarJucatori;
	char* denumire;
	float pret;
	char rating;
};

struct Joc initializare(int id, int numarJucatori, char* denumire, float pret, char rating) {
	struct Joc s;
	s.id = id;
	s.numarJucatori = numarJucatori;
	s.denumire = malloc(strlen(denumire) + 1);
	strcpy(s.denumire, denumire);
	s.pret = pret;
	s.rating = rating;
	return s;
}

void afisare(struct Joc s) {
	printf("Id: %d\n", s.id);
	printf("Nume: %s\n", s.denumire);
	printf("Numar jucatori: %d\n", s.numarJucatori);
	printf("Rating: %c\n", s.rating);
	printf("Pret: %f\n", s.pret);
}

void modificaDenumire(struct Joc* s,char* denumire) {
	free(s->denumire);
	s->denumire = malloc(strlen(denumire) + 1);
	strcpy(s->denumire, denumire);
}

float caculeazaPretIndividual(struct Joc s){
	return s.pret / s.numarJucatori;
}

void dezalocare(struct Joc *s) {
	free(s->denumire);
}

int main() {
	struct Joc s;
	s = initializare(1, 2, "monopoly", 120, 'a');

	afisare(s);

	modificaDenumire(&s, "secret h");

	afisare(s);

	printf("pret individual: %f\n", caculeazaPretIndividual(s));

	dezalocare(&s);

	return 0;
}