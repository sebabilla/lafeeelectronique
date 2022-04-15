#ifndef __TYPE__H__
#define __TYPE__H__

#define TITRE "La Fée Electronique"

#define LARGEUR_FENETRE 1024
#define HAUTEUR_FENETRE 768
#define FPS 16

#define LIGNES_TEXTE 114
#define NOMBRE_IMAGES 13
#define NOMBRE_ICONES 7
#define NOMBRE_TUILES 13
#define NOMBRE_LANGAGES 3
#define OPTIONS_MENU 3
#define NOMBRE_PAYS 10

#define LARGEUR_TERRAIN 20
#define HAUTEUR_TERRAIN 5
#define TUILE 20

#define ANIM_RECYCLER 5
#define ANIM_DEBLOQUE 1
#define ANIM_POUSSER 1
#define ANIM_NIV_FINI 1

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
	CHARGEMENT, CHOIX_LANGUE, INTRODUCTION, MENU_PRINCIPAL, NOUVELLE_PARTIE, NOUVELLE_ANNEE, EN_COURS, TRANSITION_FEE, TRANSITION_GNOME, FIN_DE_PARTIE, OPTIONS, CREDITS, QUITTER
} Etat;

typedef struct Manette
{
	Direction direction;
	Bouton bouton;
} Manette;

typedef struct Partie
{
	Etat etat;
	int langage;
	int chargement;
	int menu[3];
	int menu_niveau;
	int debloque[3]; // nouvelle difficulté (plus facile)
	int programme_en_cours;
	int fee_pose_dechets;
	int en_cours;
	int en_pause;
	int stade_intro;
	int stade_fin;
} Partie;

typedef struct Pays
{
	char nom[15];
	int x;
	int y;
	int numero;
	int dechets_electroniques;
	int ratio_lineaire;
	int variation_gmt;
} Pays;

typedef struct Terrain
{
	Pays *pays;
	int statut;
	int matrice[LARGEUR_TERRAIN][HAUTEUR_TERRAIN];
	int matrice_dechets[LARGEUR_TERRAIN][HAUTEUR_TERRAIN];
	int matrice_copie[LARGEUR_TERRAIN][HAUTEUR_TERRAIN];
	int recyclable;
	struct Terrain *suivant;
	struct Terrain *precedent;
} Terrain;

typedef struct AnimationJoueur
{
	int x;
	int y;
	int temps;
} AnimationJoueur;

typedef struct Joueur
{
	Terrain *terrain_actif;
	int recyclage;
	int x;
	int y;
	int bonus;
	int annee;
	int temps;
	int nombre_pas;
	int nombre_deplacements;
	int nombre_recyclages;
	int nombre_resets;
	int pouvoir[5];
	AnimationJoueur animation[ANIM_RECYCLER + 
								ANIM_DEBLOQUE + 
								ANIM_POUSSER + 
								ANIM_NIV_FINI];
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

extern Partie partie;
extern Joueur joueur;

#endif
