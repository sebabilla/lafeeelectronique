#include <stdlib.h>
#include <SDL.h>
#include <SDL_mixer.h>
#include <math.h>

#include "types.h"
#include "controle.h"
#include "animations.h"
#include "musique.h"
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
	p->chargement = 0;
	p->menu = 0;
	p->menu_pause = 0;
	p->debloque = 0;
	p->programme_en_cours = 1;
	p->fee_pose_dechets = 0;
	p->en_cours = 1;
	p->en_pause = 1;
	p->stade_intro = 0;
	p->stade_fin = 0;
	
	return p;
}

void LibererPartie(Partie *p)
{
	free(p);
	p = NULL;
}

void ChoixLangage(Partie *p, Clavier c)
{
	if (c.direction == BAS)
		if (p->langage > -NOMBRE_LANGAGES)
			p->langage--;
	
	if (c.direction == HAUT)
		if (p->langage < -1)
			p->langage++;
}

int ActionsLangage(Partie *partie)
{
	Clavier clavier = EntreeJoueur();
	
	if (clavier.bouton == FERMERFENETRE || clavier.bouton == ECHAP)
	{
		partie->programme_en_cours = 0;
		return 0;
	}
	if (clavier.bouton == ENTREE || clavier.bouton == ESPACE)
	{
		partie->langage *= -1;
		return 1;
	}
		
	ChoixLangage(partie, clavier);
	return 0;	
}

void ChoixMenu(Partie *p, Clavier c)
{
	if (c.direction == GAUCHE)
		if (p->menu > 0)
			p->menu--;
	
	if (c.direction == DROITE)
		if (p->menu < OPTIONS_MENU - 1)
			p->menu++;
}

int ActionsMenu(Partie *partie)
{
	Clavier clavier = EntreeJoueur();
	
	if (clavier.bouton == FERMERFENETRE)
	{
		partie->programme_en_cours = 0;
		return 0;
	}
	if (clavier.bouton == ECHAP)
		return -1;
	
	
	if (clavier.bouton == ENTREE || clavier.bouton == ESPACE)
		return 1;
		
	ChoixMenu(partie, clavier);
	return 0;	
}

void ChoixPause(Partie *p, Clavier c)
{
	if (c.direction == GAUCHE)
		if (p->menu_pause > 0)
			p->menu_pause--;
	
	if (c.direction == DROITE)
		if (p->menu_pause < OPTIONS_MENU - 1)
			p->menu_pause++;
}

int ActionPause(Joueur *j, Partie *p)
{
	Clavier clavier = EntreeJoueur();
	
	if (clavier.bouton == FERMERFENETRE)
		p->programme_en_cours = 0;
	
	if (clavier.bouton == ENTREE || clavier.bouton == ESPACE)
		return 1;
		
	ChoixPause(p, clavier);
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
	for (int i = 0; i < LARGEUR_TERRAIN; i++)
		for (int j = 0; j < HAUTEUR_TERRAIN; j++)
		{
				t->matrice[i][j] = 0;
				t->matrice_dechets[i][j] = 0;
				t->matrice_copie[i][j] = 0;
		}
			
	t->recyclable = 0;
	
	t->suivant = NULL;
	t->precedent = NULL;
}

void AjouterTerrainFin(Terrain *t, Pays *pays)
{
	if (t->suivant == NULL)
	{
		Terrain *temp = NouveauTerrain();
		InitialiserTerrain(temp, pays);
	
		t->suivant = temp;
		t->suivant->precedent = t;
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
	j->x = -1;
	j->y = 0;
	j->annee = 2021;
	j->temps = SDL_GetTicks();
	j->nombre_pas = 0;
	j->nombre_deplacements = 0;
	j->nombre_recyclages = 0;
	j->nombre_resets = 0;
	InitialiserTableauAnimation(j);
}

int PotentielRecyclage(Terrain *t)
{
	int compteur = 0;
	for (int i = 0; i < LARGEUR_TERRAIN; i++)
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
			&& j->terrain_actif->recyclable > 0)
		DetruireDechet(j, clavier.direction);
		
	if (clavier.bouton == ENTREE && 
			clavier.direction != RIEN)
		PousserDechet(j, clavier.direction);
	
	BougerJoueur(j, clavier);
}

void BougerJoueur(Joueur *j, Clavier c)
{
	switch(c.direction)
	{
		case HAUT:
			if (j->y < 1 || RencontreDechet(j, c.direction) == 1)
				return;
			else
			{
				j->nombre_pas++;
				j->y--;
			}
			break;
		case BAS:
			if (j->y > HAUTEUR_TERRAIN - 2 || RencontreDechet(j, c.direction) == 1)
				return;
			else
			{
				j->nombre_pas++;
				j->y++;
			}
			break;
		case GAUCHE:
			if (j->x < 0)
			{
				if (j->terrain_actif->precedent != NULL)
					ChangerTerrainActifPrecedent(j);
				else
					return;
			}
			if (RencontreDechet(j, c.direction) == 1)
				return;
			else
			{
				j->nombre_pas++;
				j->x--;
			}
			break;
		case DROITE:
			if (j->x >= LARGEUR_TERRAIN)
			{
				if (j->terrain_actif->suivant != NULL)
					ChangerTerrainActifSuivant(j);
				else
				{
					j->nombre_pas++;
					j->x++;
				}
			}
			else if (RencontreDechet(j, c.direction) == 1)
				return;
			else
			{
				j->nombre_pas++;
				j->x++;
			}
			break;
		default:
			return;
	} 
}

void ChangerTerrainActifSuivant(Joueur *j)
{
	j->x = -1;
	j->terrain_actif = j->terrain_actif->suivant;
	j->recyclage = PotentielRecyclage(j->terrain_actif);
}

void ChangerTerrainActifPrecedent(Joueur *j)
{
	j->x = LARGEUR_TERRAIN + 1;
	j->terrain_actif = j->terrain_actif->precedent;
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
	j->x = -1;
	j->y = 0;
	j->annee++;
	j->recyclage = PotentielRecyclage(j->terrain_actif);
	
	
}

void DebloquerMode(Joueur *j, Partie *p)
{
	int lancer_anim = 0;
	
	if (j->annee == 2023 && p->debloque == 0)
	{
		p->debloque = 1;
		lancer_anim = 1;
	} 
	if (j->annee == 2027 && p->debloque == 1)
	{
		p->debloque = 2;
		lancer_anim = 1;
	}
	if (j->annee == 2030 && p->debloque == 2)
	{
		p->debloque = 3;
		lancer_anim = 1;
	}
	if (lancer_anim == 1)
	{	
		JouerBruitage(9);
		j->animation[ANIM_RECYCLER].temps = 400; // position de ANIM_DEBLOQUE
		j->animation[ANIM_RECYCLER].x = 2 * TUILE;
		j->animation[ANIM_RECYCLER].y = HAUTEUR_FENETRE - 3 * TUILE;
	}
}


//------------Gestion Dechets-------------------------------------------

int RandomDechet(int actuel)
{
	if (actuel == 0)
		return rand() % 3 + 1;
	if (actuel == 1)
		return rand() % 5 + 1;
	if (actuel == 2)
		return rand() % 7 + 1;
	
	return 0;
}

int GenererDechets(Terrain *t)
{
	int compteur_dechets = 0;
	
	for (int i = 0; i < LARGEUR_TERRAIN;i ++)
		for (int j = 0; j < HAUTEUR_TERRAIN; j++)
			t->matrice_dechets[i][j] = 0;
	
	for (int limite = 0; limite < 1000; limite++)
		for (int i = 0; i < LARGEUR_TERRAIN; i++)
			for (int j = 0; j < HAUTEUR_TERRAIN; j++)
			{
				if (t->matrice_dechets[i][j] == 0)
					switch(t->matrice[i][j])
					{
						case 0:
							if (rand() % 100 == 0)
							{
								t->matrice_dechets[i][j] = RandomDechet(0);
								compteur_dechets++;
							}
							break;
						case 1:
							if (rand() % 200 == 0)
							{
								t->matrice_dechets[i][j] = RandomDechet(1);
								compteur_dechets++;
							}
							break;
						case 2:
							if (rand() % 300 == 0)
							{
								t->matrice_dechets[i][j] = RandomDechet(2);
								compteur_dechets++;
							}
							break;
						default:
							break;
					}
				if (compteur_dechets >= t->pays->dechets_electroniques)
					return 1;
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
	for (int i = 0; i < LARGEUR_TERRAIN; i++)
		for (int j = 0; j < HAUTEUR_TERRAIN; j++)
			if (t->matrice_dechets[i][j] > 0)
			{
				t->matrice[i][j]++;
				t->matrice_copie[i][j] = t->matrice[i][j];
			}
			
	t->recyclable = PotentielRecyclage(t);
}

void AjouterTousLesDechets(Terrain *t)
{
	AjouterDechets(t);
	if (t->suivant != NULL)
		AjouterTousLesDechets(t->suivant);
}

void ResetTerrain(Terrain *t)
{
	for (int i = 0; i < LARGEUR_TERRAIN; i++)
		for (int j = 0; j < HAUTEUR_TERRAIN; j++)
			t->matrice[i][j] = t->matrice_copie[i][j];
			
	t->recyclable = PotentielRecyclage(t);
}

void ResetTousLesTerrains(Terrain *t)
{
	ResetTerrain(t);
	if (t->suivant != NULL)
		ResetTousLesTerrains(t->suivant);
}

//------------Gestion Interaction Joueurs/Dechets-----------------------

int RencontreDechet(Joueur *j, Direction d)
{
	if (j->x > LARGEUR_TERRAIN) // eviter cas de fin d'année
		return 0;
	
	switch(d)
	{
		case HAUT:
			if (j->x < 0 || j->x > LARGEUR_TERRAIN - 1)
				return 0;
			if (j->terrain_actif->matrice[j->x][j->y - 1] > 0)
				return 1;
			break;
		case BAS:
			if (j->x < 0 || j->x > LARGEUR_TERRAIN - 1)
				return 0;
			if (j->terrain_actif->matrice[j->x][j->y + 1] > 0)
				return 1;
			break;
		case GAUCHE:
			if (j->x <= 0)
				return 0;			
			if (j->terrain_actif->matrice[j->x - 1][j->y] > 0)
				return 1;
			break;
		case DROITE:
			if (j->x >= LARGEUR_TERRAIN - 1)
				return 0;
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
	if (j->x > LARGEUR_TERRAIN) // eviter cas de fin d'année
		return;
	
	switch(d)
	{
		case HAUT:
			if (j->y > 0)
				if (j->terrain_actif->matrice[j->x][j->y - 1] > 0  &&
						j->terrain_actif->matrice[j->x][j->y - 1] < 3)
				{
					j->nombre_recyclages++;
					j->terrain_actif->matrice[j->x][j->y - 1]--;
					j->terrain_actif->recyclable--;
					AjouterAnimRecyclage(j);
					JouerBruitage(6);
				}
			return;
			break;
		case BAS:
			if (j->y < HAUTEUR_TERRAIN - 1)
				if (j->terrain_actif->matrice[j->x][j->y + 1] > 0 &&
						j->terrain_actif->matrice[j->x][j->y + 1] < 3)
				{
					j->nombre_recyclages++;
					j->terrain_actif->matrice[j->x][j->y + 1]--;
					j->terrain_actif->recyclable--;
					AjouterAnimRecyclage(j);
					JouerBruitage(6);
				}
			return;
			break;
		case GAUCHE:
			if (j->x > 0)
				if (j->terrain_actif->matrice[j->x - 1][j->y] > 0 &&
						j->terrain_actif->matrice[j->x - 1][j->y] < 3)
				{
					j->nombre_recyclages++;
					j->terrain_actif->matrice[j->x - 1][j->y]--;
					j->terrain_actif->recyclable--;
					AjouterAnimRecyclage(j);
					JouerBruitage(6);
				}
			return;
			break;
		case DROITE:
			if (j->x < LARGEUR_TERRAIN - 1)
				if (j->terrain_actif->matrice[j->x + 1][j->y] > 0 &&
						j->terrain_actif->matrice[j->x + 1][j->y] < 3)
				{
					j->nombre_recyclages++;
					j->terrain_actif->matrice[j->x + 1][j->y]--;
					j->terrain_actif->recyclable--;
					AjouterAnimRecyclage(j);
					JouerBruitage(6);
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
	if (j->x > LARGEUR_TERRAIN + 1) // eviter cas de fin d'année
		return;
	
	AnimationPousser(j);
	switch(d)
	{
		case HAUT:
			if (j->y > 1)
				if (j->terrain_actif->matrice[j->x][j->y - 1] == 1  &&
						j->terrain_actif->matrice[j->x][j->y - 2] == 0)
				{
					j->nombre_deplacements++;
					j->terrain_actif->matrice[j->x][j->y - 1]--;
					j->terrain_actif->matrice[j->x][j->y - 2]++;
					JouerBruitage(7);
				}
			return;
			break;
		case BAS:
			if (j->y < HAUTEUR_TERRAIN - 2) 
				if (j->terrain_actif->matrice[j->x][j->y + 1] == 1 &&
						j->terrain_actif->matrice[j->x][j->y + 2] == 0)
				{
					j->nombre_deplacements++;
					j->terrain_actif->matrice[j->x][j->y + 1]--;
					j->terrain_actif->matrice[j->x][j->y + 2]++;
					JouerBruitage(7);
				}
			return;
			break;
		case GAUCHE:
			if (j->x > 1)
				if (j->terrain_actif->matrice[j->x - 1][j->y] == 1 &&
						j->terrain_actif->matrice[j->x - 2][j->y] == 0)
				{
					j->nombre_deplacements++;
					j->terrain_actif->matrice[j->x - 1][j->y]--;
					j->terrain_actif->matrice[j->x - 2][j->y]++;
					JouerBruitage(8);
				}
			return;
			break;
		case DROITE:
			if (j->x < LARGEUR_TERRAIN - 2)
				if (j->terrain_actif->matrice[j->x + 1][j->y] == 1 &&
						j->terrain_actif->matrice[j->x + 2][j->y] == 0)
				{
					j->nombre_deplacements++;
					j->terrain_actif->matrice[j->x + 1][j->y]--;
					j->terrain_actif->matrice[j->x + 2][j->y]++;
					JouerBruitage(8);
				}
			return;
			break;
		default:
			return;
	}
	return; 
}

