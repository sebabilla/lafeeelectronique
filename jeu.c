#include <stdlib.h>
#include <SDL_mixer.h>
#include <math.h>

#include "types.h"
#include "controle.h"
#include "jeu.h"


//------------Initialisation et rechargement----------------------------
Partie *InitialisationPartie(void)
{
	Partie *p = malloc(sizeof(Partie));
	if (p == NULL)
	{
		printf("Erreur d'allocation mémoire");
		exit(EXIT_FAILURE);
	}
	
	p->etat = CHOIX_LANGUE;
	p->langage = -1;
	p->programme_en_cours = 1;
	p->fee_pose_dechets = 0;
	p->en_cours = 1;
	p->en_pause = 1;
	p->introduction.stade = 0;
	p->introduction.temps = 0;
	p->introduction.inverser = 1;
	
	return p;
}

void LibererPartie(Partie *p)
{
	free(p);
	p = NULL;
}

void Choixlangage(Partie *p, Clavier c)
{
	if (c.direction == BAS)
		if (p->langage > -4)
			p->langage--;
	
	if (c.direction == HAUT)
		if (p->langage < -1)
			p->langage++;
}

int ActionsLangage(Partie *partie)
{
	Clavier clavier = EntreeJoueur();
	
	if (clavier.bouton == FERMERFENETRE)
	{
		partie->programme_en_cours = 0;
		return 0;
	}
	if (clavier.bouton == ENTREE || clavier.bouton == ESPACE)
	{
		partie->langage *= -1;
		return 1;
	}
		
	Choixlangage(partie, clavier);
	return 0;	
}

//------------Gestion terrain-------------------------------------------
Terrain *NouveauTerrain(void)
{
	Terrain *t = malloc(sizeof(Terrain));
	if (t == NULL)
	{
		printf("Erreur d'allocation mémoire");
		exit(EXIT_FAILURE);
	}
	return t;
}

void InitialiserTerrain(Terrain *t, Pays *pays)
{
	t->pays = pays;
	t->statut = 0;
	for (int i = 0; i < LARGEUR_TERRAIN + 2; i ++)
		for (int j = 0; j < HAUTEUR_TERRAIN; j++)
			if (i == 0 || i == LARGEUR_TERRAIN + 1)
				t->matrice[i][j] = -1;
			else
				t->matrice[i][j] = 0;
	for (int i = 0; i < LARGEUR_TERRAIN; i ++)
		for (int j = 0; j < HAUTEUR_TERRAIN; j++)
			t->matrice_dechets[i][j] = 0;
	
	t->suivant = NULL;
}

void AjouterTerrainFin(Terrain *t, Pays *pays)
{
	if (t->suivant == NULL)
	{
		Terrain *temp = NouveauTerrain();
		InitialiserTerrain(temp, pays);
	
		t->suivant = temp;
		return;
	}
		
	AjouterTerrainFin(t->suivant, pays);
		
	
}

void LibererTerrain(Terrain *t)
{
	free(t);
	t = NULL;
}

void DetruireTerrains(Terrain *t)
{
	if (t->suivant != NULL)
		DetruireTerrains(t->suivant);
		
	LibererTerrain(t);
}

//------------Gestion Joueur-------------------------------------------
Joueur *NouveauJoueur(void)
{
	Joueur *j = malloc(sizeof(Joueur));
	if (j == NULL)
	{
		printf("Erreur d'allocation mémoire");
		exit(EXIT_FAILURE);
	}
	return j;
}

void InitialiserJoueur(Joueur *j, Terrain *t)
{
	j->terrain_actif = t;
	j->recyclage = PotentielRecyclage(t);
	j->x = 0;
	j->y = 0;
	j->annee = 2021;
	j->temps = 0;
}

int PotentielRecyclage(Terrain *t)
{
	int compteur = 0;
	for (int i = 1; i <= LARGEUR_TERRAIN; i++)
		for (int j = 0; j < HAUTEUR_TERRAIN; j++)
			compteur += t->matrice[i][j];;
	float potentiel = (float)t->pays->ratio_lineaire * compteur / 100;
	if (potentiel < 1 && rand() % 3 == 0)
		potentiel = ceil(potentiel);
	
	return potentiel;
}

void LibererJoueur(Joueur *j)
{
	free(j);
	j = NULL;
}

void ActionJoueur(Joueur *j, Partie *p)
{
	Clavier clavier = EntreeJoueur();
	
	if (clavier.bouton == FERMERFENETRE)
		p->programme_en_cours = 0;
	
	if (clavier.bouton == ECHAP)
		p->en_cours = 0;
		
	if (clavier.bouton == ESPACE && 
			clavier.direction != RIEN 
			&& j->recyclage > 0)
		DetruireDechet(j, clavier.direction);
		
	if (clavier.bouton == ENTREE && 
			clavier.direction != RIEN)
		PousserDechet(j, clavier.direction);
	
	BougerJoueur(j, clavier);
}

void ActionPause(Joueur *j, Partie *p)
{
	Clavier clavier = EntreeJoueur();
	
	if (clavier.bouton == FERMERFENETRE)
		p->programme_en_cours = 0;
	
	if (clavier.bouton == ENTREE || clavier.bouton == ESPACE)
		p->en_pause = 0;
		
	if (clavier.bouton == ECHAP)
		p->en_pause = -1;
}

void BougerJoueur(Joueur *j, Clavier c)
{
	switch(c.direction)
	{
		case HAUT:
			if (j->y <= 0 || RencontreDechet(j, c.direction) == 1)
				return;
			else
				j->y--;
			break;
		case BAS:
			if (j->y >= HAUTEUR_TERRAIN - 1 || RencontreDechet(j, c.direction) == 1)
				return;
			else
				j->y++;
			break;
		case GAUCHE:
			if (j->x <= 0 || RencontreDechet(j, c.direction) == 1)
				return;
			else
				j->x--;
			break;
		case DROITE:
			if (j->x >= LARGEUR_TERRAIN + 1)
			{
				if (j->terrain_actif->suivant != NULL)
					ChangerTerrainActif(j);
				else
					return;
			}
			else if (RencontreDechet(j, c.direction) == 1)
				return;
			else
				j->x++;
			break;
		default:
			return;
	} 
}

void ChangerTerrainActif(Joueur *j)
{
	j->x = 0;
	j->terrain_actif = j->terrain_actif->suivant;
	j->recyclage = PotentielRecyclage(j->terrain_actif);
}

int AnneeFinie(Joueur *j)
{
	if (j->terrain_actif->suivant == NULL &&
		j->x >= LARGEUR_TERRAIN + 1)
		return 1;
	return 0;
}

void NouvelleAnnee(Joueur *j, Partie *p, Terrain *t)
{
	p->etat = NOUVELLE_ANNEE;
	j->terrain_actif = t;
	j->x = 0;
	j->y = 0;
	j->annee++;
	j->recyclage = PotentielRecyclage(j->terrain_actif);
}


//------------Gestion Dechets-------------------------------------------

int GenererDechets(Terrain *t)
{
	int terrain_actuel[LARGEUR_TERRAIN * HAUTEUR_TERRAIN] = {0};
		
	for (int i = 1; i < LARGEUR_TERRAIN + 1; i ++)
		for (int j = 0; j < HAUTEUR_TERRAIN; j++)
			terrain_actuel[j * LARGEUR_TERRAIN + i] = t->matrice[i][j];
			
	int nouveaux_dechets[LARGEUR_TERRAIN * HAUTEUR_TERRAIN] = {0};
	int compteur_dechets = 0;
	
	for (int limite = 0; limite < 1000; limite++)
		for (int m = 0; m < LARGEUR_TERRAIN * HAUTEUR_TERRAIN; m++)
		{
			switch(terrain_actuel[m])
			{
				case 0:
					if (rand() % 100 == 0 && nouveaux_dechets[m] == 0)
					{
						nouveaux_dechets[m] = 1;
						compteur_dechets++;
					}
					break;
				case 1:
					if (rand() % 200 == 0 && nouveaux_dechets[m] == 0)
					{
						nouveaux_dechets[m] = 1;
						compteur_dechets++;
					}
					break;
				case 2:
					if (rand() % 300 == 0 && nouveaux_dechets[m] == 0)
					{
						nouveaux_dechets[m] = 1;
						compteur_dechets++;
					}
					break;
				default:
					break;
			}
			if (compteur_dechets >= t->pays->dechets_electroniques)
			{
				for (int i = 0; i < LARGEUR_TERRAIN;i ++)
					for (int j = 0; j < HAUTEUR_TERRAIN; j++)
						t->matrice_dechets[i][j] = nouveaux_dechets[j * LARGEUR_TERRAIN + i];
				return 1;
			}
		}
	return 0;	
}

void GenererTousLesDechets(Terrain *t)
{
	GenererDechets(t);
	if (t->suivant != NULL)
		GenererTousLesDechets(t->suivant);
}

void AjouterDechets(Terrain *t)
{
	for (int i = 0; i < LARGEUR_TERRAIN; i ++)
		for (int j = 0; j < HAUTEUR_TERRAIN; j++)
			t->matrice[i + 1][j] += t->matrice_dechets[i][j];
}

void AjouterTousLesDechets(Terrain *t)
{
	AjouterDechets(t);
	if (t->suivant != NULL)
		AjouterTousLesDechets(t->suivant);
}

//------------Gestion Interaction Joueurs/Dechets-----------------------

int RencontreDechet(Joueur *j, Direction d)
{
	switch(d)
	{
		case HAUT:
			if (j->terrain_actif->matrice[j->x][j->y - 1] > 0)
				return 1;
			break;
		case BAS:
			if (j->terrain_actif->matrice[j->x][j->y + 1] > 0)
				return 1;
			break;
		case GAUCHE:
			if (j->terrain_actif->matrice[j->x - 1][j->y] > 0)
				return 1;
			break;
		case DROITE:
			if (j->terrain_actif->matrice[j->x + 1][j->y] > 0)
				return 1;
			break;
		default:
			return 0;
	}
	return 0; 
}

void DetruireDechet(Joueur *j, Direction d)
{
	switch(d)
	{
		case HAUT:
			if (j->y > 0)
				if (j->terrain_actif->matrice[j->x][j->y - 1] > 0  &&
						j->terrain_actif->matrice[j->x][j->y - 1] < 3)
				{
					j->terrain_actif->matrice[j->x][j->y - 1]--;
					j->recyclage--;
				}
			return;
			break;
		case BAS:
			if (j->y < HAUTEUR_TERRAIN - 1)
				if (j->terrain_actif->matrice[j->x][j->y + 1] > 0 &&
						j->terrain_actif->matrice[j->x][j->y + 1] < 3)
				{
					j->terrain_actif->matrice[j->x][j->y + 1]--;
					j->recyclage--;
				}
			return;
			break;
		case GAUCHE:
			if (j->x > 1)
				if (j->terrain_actif->matrice[j->x - 1][j->y] > 0 &&
						j->terrain_actif->matrice[j->x - 1][j->y] < 3)
				{
					j->terrain_actif->matrice[j->x - 1][j->y]--;
					j->recyclage--;
				}
			return;
			break;
		case DROITE:
			if (j->x < LARGEUR_TERRAIN)
				if (j->terrain_actif->matrice[j->x + 1][j->y] > 0 &&
						j->terrain_actif->matrice[j->x + 1][j->y] < 3)
				{
					j->terrain_actif->matrice[j->x + 1][j->y]--;
					j->recyclage--;
				}
			return;
			break;
		default:
			return;
	}
	return; 
}

void PousserDechet(Joueur *j, Direction d)
{
	switch(d)
	{
		case HAUT:
			if (j->y > 1)
				if (j->terrain_actif->matrice[j->x][j->y - 1] == 1  &&
						j->terrain_actif->matrice[j->x][j->y - 2] == 0)
				{
					j->terrain_actif->matrice[j->x][j->y - 1]--;
					j->terrain_actif->matrice[j->x][j->y - 2]++;
				}
			return;
			break;
		case BAS:
			if (j->y < HAUTEUR_TERRAIN - 2) 
				if (j->terrain_actif->matrice[j->x][j->y + 1] == 1 &&
						j->terrain_actif->matrice[j->x][j->y + 2] == 0)
				{
					j->terrain_actif->matrice[j->x][j->y + 1]--;
					j->terrain_actif->matrice[j->x][j->y + 2]++;
				}
			return;
			break;
		case GAUCHE:
			if (j->x > 2)
				if (j->terrain_actif->matrice[j->x - 1][j->y] == 1 &&
						j->terrain_actif->matrice[j->x - 2][j->y] == 0)
				{
					j->terrain_actif->matrice[j->x - 1][j->y]--;
					j->terrain_actif->matrice[j->x - 2][j->y]++;
				}
			return;
			break;
		case DROITE:
			if (j->x < LARGEUR_TERRAIN)
				if (j->terrain_actif->matrice[j->x + 1][j->y] == 1 &&
						j->terrain_actif->matrice[j->x + 2][j->y] == 0)
				{
					j->terrain_actif->matrice[j->x + 1][j->y]--;
					j->terrain_actif->matrice[j->x + 2][j->y]++;
				}
			return;
			break;
		default:
			return;
	}
	return; 
}

