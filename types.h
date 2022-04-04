#ifndef __TYPE__H__
#define __TYPE__H__

#define TITRE "La Fée Electronique"
#define LARGEUR_TERRAIN 20
#define HAUTEUR_TERRAIN 5
#define TUILE 20

#define NB_TERRAINS 3
#define NOMBRE_PAYS 10

typedef enum Direction
{
	HAUT, BAS, GAUCHE, DROITE, RIEN
} Direction;

typedef enum Bouton
{
	ENTREE, ESPACE, ECHAP, FERMERFENETRE, SANS
} Bouton;

typedef struct Manette
{
	Direction direction;
	Bouton bouton;
} Manette;

typedef struct Partie
{
	int langage;
	int programme_en_cours;
	int pause;
	int reset;
	int temps;
	int resultat;
} Partie;

typedef struct Pays
{
	char nom[15];
	int x;
	int y;
	int dechets_electroniques;
	int ratio_lineaire;
} Pays;

typedef struct Terrain
{
	Pays *pays;
	int statut;
	int matrice[LARGEUR_TERRAIN + 2][HAUTEUR_TERRAIN];
	int matrice_dechets[LARGEUR_TERRAIN][HAUTEUR_TERRAIN];
	struct Terrain *suivant;
} Terrain;

typedef struct Joueur
{
	Terrain *terrain_actif;
	int recyclage;
	int x;
	int y;
	int annee;
} Joueur;

typedef struct Clavier
{
	Direction direction;
	Bouton bouton;
} Clavier;

typedef struct Textes
{
		char bravo[10];
		char incorrect[15];
		char pause[10];
		char fleches[60];
		char entree[60];
		char espace[60];
		char but[10];
		char credits[10];
		char auteur[100];
		char sources[100];
		char musique[100];
		char bruitages[100];
} Textes;

#endif
