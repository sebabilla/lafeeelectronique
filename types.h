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

typedef enum Etat
{
	CHOIX_LANGUE, INTRODUCTION, MENU_PRINCIPAL, NOUVELLE_PARTIE, NOUVELLE_ANNEE, EN_COURS, TRANSITION_FEE, TRANSITION_GNOME, PAUSE, CREDITS, QUITTER
} Etat;

typedef struct Manette
{
	Direction direction;
	Bouton bouton;
} Manette;

typedef struct Introduction
{
	int stade;
	int temps;
	int inverser;
} Introduction;

typedef struct Partie
{
	int langage;
	int programme_en_cours;
	int fee_pose_dechets;
	Introduction introduction;
	Etat etat;
} Partie;

typedef struct Pays
{
	char nom[15];
	int x;
	int y;
	int numero;
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
	int temps;
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
