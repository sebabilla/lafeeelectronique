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
Partie InitialisationPartie(void)
{
	Partie p = {	
				.etat = CHOIX_LANGUE,
				.langage = -1,
				.chargement = 0,
				.menu = {0, 0, 0},
				.menu_niveau = 0,
				.debloque = {1, 0, 0},
				.programme_en_cours = 1,
				.fee_pose_dechets = 0,
				.en_cours = 1,
				.en_pause = 1,
				.stade_intro = 0,
				.stade_fin = 0
				};
	
	return p;
}

void ChoixLangage(Clavier c)
{
	if (c.direction == BAS)
		if (partie.langage > -NOMBRE_LANGAGES)
			partie.langage--;
	
	if (c.direction == HAUT)
		if (partie.langage < -1)
			partie.langage++;
}

int ActionsLangage(void)
{
	Clavier clavier = EntreeJoueur();
	
	if (clavier.bouton == FERMERFENETRE || clavier.bouton == ECHAP)
	{
		partie.programme_en_cours = 0;
		return 0;
	}
	if (clavier.bouton == ENTREE || clavier.bouton == ESPACE)
	{
		partie.langage *= -1;
		return 1;
	}
		
	ChoixLangage(clavier);
	return 0;	
}

void ChoixMenu(Clavier c, int index_menu, int nombre_de_choix)
{
	if (c.direction == HAUT)
		if (partie.menu[index_menu] > 0)
			partie.menu[index_menu]--;
	
	if (c.direction == BAS)
		if (partie.menu[index_menu] < nombre_de_choix - 1)
			partie.menu[index_menu]++;
}

int ActionsMenu(int index_menu, int nombre_de_choix)
{
	Clavier clavier = EntreeJoueur();
	
	if (clavier.bouton == FERMERFENETRE)
	{
		partie.programme_en_cours = 0;
		return 0;
	}
	if (clavier.bouton == ECHAP)
		return -1;
	
	
	if (clavier.bouton == ENTREE)
		return 1;
		
	ChoixMenu(clavier, index_menu, nombre_de_choix);
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

int PotentielRecyclage(Terrain *t)
{	
	int compteur = 0;
	for (int i = 0; i < LARGEUR_TERRAIN; i++)
		for (int j = 0; j < HAUTEUR_TERRAIN; j++)
			compteur += t->matrice[i][j];
	float potentiel = ((float)t->pays->ratio_lineaire + joueur.bonus) * compteur / 100;
	if (potentiel < 1 && rand() % 3 == 0)
		potentiel = ceil(potentiel);
	
	return potentiel;
}

void EchangerAppareils(Terrain *t)
{
	int compteur_dechets = 0;
	
	for (int i = 0; i < LARGEUR_TERRAIN; i++)
		for (int j = 0; j < HAUTEUR_TERRAIN; j++)
			compteur_dechets += joueur.terrain_actif->matrice[i][j];
			
	compteur_dechets /= 3;
	
	ViderUntiers(compteur_dechets);
	
	Terrain *pointeur_cameroun = TrouverPays(t, 2);
	
	AjouterATerrainSpecifique(pointeur_cameroun, compteur_dechets);		
}

int ViderUntiers(int compteur_dechets)
{	
	for (int limite = 0; limite < 1000; limite++)
		for (int i = 0; i < LARGEUR_TERRAIN; i++)
			for (int j = 0; j < HAUTEUR_TERRAIN; j++)
			{
				if (joueur.terrain_actif->matrice[i][j] > 0)
					if (rand() % 100 == 0)
							{
								joueur.terrain_actif->matrice[i][j]--;
								compteur_dechets--;
							}
				if (compteur_dechets <= 0)
					return 1;
			}
	return 0;
}

Terrain *TrouverPays(Terrain *t, int numero_pays)
{
	if (t->pays->numero == numero_pays)
		return t;
		
	return TrouverPays(t->suivant, numero_pays);
}

int AjouterATerrainSpecifique(Terrain *t, int compteur_dechets)
{
	for (int limite = 0; limite < 1000; limite++)
		for (int i = 0; i < LARGEUR_TERRAIN; i++)
			for (int j = 0; j < HAUTEUR_TERRAIN; j++)
			{
				if (t->matrice[i][j] == 0)
					if (rand() % 100 == 0)
					{
						t->matrice[i][j]++;
						compteur_dechets--;
					}
				if (compteur_dechets <= 0)
					return 1;
			}

	return 0;
}

//------------Gestion Joueur-------------------------------------------
Joueur InitialiserJoueur(Terrain *t)
{
	Joueur j = {
				.terrain_actif = t,
				.x = -1,
				.y = 0,
				.bonus = 0,
				.annee = 2021,
				.temps = SDL_GetTicks(),
				.nombre_pas = 0,
				.nombre_deplacements = 0,
				.nombre_recyclages = 0,
				.nombre_resets = 0,
				.pouvoir = {1, 1, 1, 0, 0} // 3 et 4 sont debloques en jeu
				};
				
	for (int i = 0; i < ANIM_RECYCLER + ANIM_DEBLOQUE + ANIM_POUSSER + ANIM_NIV_FINI; i++)
	{
		joueur.animation[i].x = 0;
		joueur.animation[i].y = 0;
		joueur.animation[i].temps = 0;
	}
	
	return j;	
}

void ActionJoueur(void)
{
	Clavier clavier = EntreeJoueur();
	
	if (clavier.bouton == FERMERFENETRE)
		partie.programme_en_cours = 0;
	
	if (clavier.bouton == ECHAP)
		partie.en_cours = 0;
		
	if (clavier.bouton == ESPACE && 
			clavier.direction != RIEN 
			&& joueur.terrain_actif->recyclable > 0)
		DetruireDechet(clavier.direction);
		
	if (clavier.bouton == ENTREE && 
			clavier.direction != RIEN)
		PousserDechet(clavier.direction);
	
	BougerJoueur(clavier);
}

void BougerJoueur(Clavier c)
{
	switch(c.direction)
	{
		case HAUT:
			if (joueur.y < 1 || RencontreDechet(c.direction) == 1)
				return;
			else
			{
				joueur.nombre_pas++;
				joueur.y--;
			}
			break;
		case BAS:
			if (joueur.y > HAUTEUR_TERRAIN - 2 || RencontreDechet(c.direction) == 1)
				return;
			else
			{
				joueur.nombre_pas++;
				joueur.y++;
			}
			break;
		case GAUCHE:
			if (joueur.x < 0)
			{
				if (joueur.terrain_actif->precedent != NULL)
					ChangerTerrainActifPrecedent();
				else
					return;
			}
			if (RencontreDechet(c.direction) == 1)
				return;
			else
			{
				joueur.nombre_pas++;
				joueur.x--;
			}
			break;
		case DROITE:
			if (joueur.x >= LARGEUR_TERRAIN)
			{
				if (joueur.terrain_actif->suivant != NULL)
					ChangerTerrainActifSuivant();
				else
				{
					joueur.nombre_pas++;
					joueur.x++;
				}
			}
			else if (RencontreDechet(c.direction) == 1)
				return;
			else
			{
				joueur.nombre_pas++;
				joueur.x++;
			}
			break;
		default:
			return;
	} 
}

void ChangerTerrainActifSuivant(void)
{
	joueur.x = -1;
	joueur.terrain_actif = joueur.terrain_actif->suivant;
}

void ChangerTerrainActifPrecedent(void)
{
	joueur.x = LARGEUR_TERRAIN + 1;
	joueur.terrain_actif = joueur.terrain_actif->precedent;
}

int AnneeFinie(void)
{
	if (joueur.terrain_actif->suivant == NULL &&
		joueur.x >= LARGEUR_TERRAIN + 1)
		return 1;
	return 0;
}

void NouvelleAnnee(Terrain *t)
{
	partie.etat = NOUVELLE_ANNEE;
	joueur.terrain_actif = t;
	joueur.x = -1;
	joueur.y = 0;
	joueur.annee++;	
}

void DebloquerMode(void)
{
	int lancer_anim = 0;
	
	if (joueur.annee == 2023 && partie.debloque[2] == 0)
	{
		partie.debloque[2] = 1;
		lancer_anim = 1;
	} 
	if (joueur.annee == 2025)
	{
		joueur.pouvoir[3] = 1;
		lancer_anim = 1;
	}
	
	if (joueur.annee == 2027 && partie.debloque[1] == 0)
	{
		partie.debloque[1] = 1;
		lancer_anim = 1;
	}
	if (joueur.annee == 2028)
	{
		joueur.pouvoir[4] = 1;
		lancer_anim = 1;
	}
	
	if (joueur.annee == 2030)
	{
		lancer_anim = 1;
	}
	if (lancer_anim == 1)
	{	
		JouerBruitage(9);
		joueur.animation[ANIM_RECYCLER].temps = 400; // position de ANIM_DEBLOQUE
		joueur.animation[ANIM_RECYCLER].x = 2 * TUILE;
		joueur.animation[ANIM_RECYCLER].y = HAUTEUR_FENETRE - 3 * TUILE;
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
		{
			if (t->matrice_dechets[i][j] > 0)
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

int RencontreDechet(Direction d)
{
	if (joueur.x > LARGEUR_TERRAIN) // eviter cas de fin d'année
		return 0;
	
	switch(d)
	{
		case HAUT:
			if (joueur.x < 0 || joueur.x > LARGEUR_TERRAIN - 1)
				return 0;
			if (joueur.terrain_actif->matrice[joueur.x][joueur.y - 1] > 0)
				return 1;
			break;
		case BAS:
			if (joueur.x < 0 || joueur.x > LARGEUR_TERRAIN - 1)
				return 0;
			if (joueur.terrain_actif->matrice[joueur.x][joueur.y + 1] > 0)
				return 1;
			break;
		case GAUCHE:
			if (joueur.x <= 0)
				return 0;			
			if (joueur.terrain_actif->matrice[joueur.x - 1][joueur.y] > 0)
				return 1;
			break;
		case DROITE:
			if (joueur.x >= LARGEUR_TERRAIN - 1)
				return 0;
			if (joueur.terrain_actif->matrice[joueur.x + 1][joueur.y] > 0)
				return 1;
			break;
		default:
			return 0;
	}
	return 0; 
}

void DetruireDechet(Direction d)
{
	if (joueur.x > LARGEUR_TERRAIN) // eviter cas de fin d'année
		return;
	
	switch(d)
	{
		case HAUT:
			if (joueur.y > 0)
				if (joueur.terrain_actif->matrice[joueur.x][joueur.y - 1] > 0  &&
						joueur.terrain_actif->matrice[joueur.x][joueur.y - 1] < 3)
				{
					joueur.nombre_recyclages++;
					joueur.terrain_actif->matrice[joueur.x][joueur.y - 1]--;
					joueur.terrain_actif->recyclable--;
					AjouterAnimRecyclage();
					JouerBruitage(6);
				}
			return;
			break;
		case BAS:
			if (joueur.y < HAUTEUR_TERRAIN - 1)
				if (joueur.terrain_actif->matrice[joueur.x][joueur.y + 1] > 0 &&
						joueur.terrain_actif->matrice[joueur.x][joueur.y + 1] < 3)
				{
					joueur.nombre_recyclages++;
					joueur.terrain_actif->matrice[joueur.x][joueur.y + 1]--;
					joueur.terrain_actif->recyclable--;
					AjouterAnimRecyclage();
					JouerBruitage(6);
				}
			return;
			break;
		case GAUCHE:
			if (joueur.x > 0)
				if (joueur.terrain_actif->matrice[joueur.x - 1][joueur.y] > 0 &&
						joueur.terrain_actif->matrice[joueur.x - 1][joueur.y] < 3)
				{
					joueur.nombre_recyclages++;
					joueur.terrain_actif->matrice[joueur.x - 1][joueur.y]--;
					joueur.terrain_actif->recyclable--;
					AjouterAnimRecyclage();
					JouerBruitage(6);
				}
			return;
			break;
		case DROITE:
			if (joueur.x < LARGEUR_TERRAIN - 1)
				if (joueur.terrain_actif->matrice[joueur.x + 1][joueur.y] > 0 &&
						joueur.terrain_actif->matrice[joueur.x + 1][joueur.y] < 3)
				{
					joueur.nombre_recyclages++;
					joueur.terrain_actif->matrice[joueur.x + 1][joueur.y]--;
					joueur.terrain_actif->recyclable--;
					AjouterAnimRecyclage();
					JouerBruitage(6);
				}
			return;
			break;
		default:
			return;
	}
	return; 
}

void PousserDechet(Direction d)
{
	if (joueur.x > LARGEUR_TERRAIN + 1) // eviter cas de fin d'année
		return;
	
	AnimationPousser();
	switch(d)
	{
		case HAUT:
			if (joueur.y > 1)
				if (joueur.terrain_actif->matrice[joueur.x][joueur.y - 1] == 1  &&
						joueur.terrain_actif->matrice[joueur.x][joueur.y - 2] == 0)
				{
					joueur.nombre_deplacements++;
					joueur.terrain_actif->matrice[joueur.x][joueur.y - 1]--;
					joueur.terrain_actif->matrice[joueur.x][joueur.y - 2]++;
					JouerBruitage(7);
				}
			return;
			break;
		case BAS:
			if (joueur.y < HAUTEUR_TERRAIN - 2) 
				if (joueur.terrain_actif->matrice[joueur.x][joueur.y + 1] == 1 &&
						joueur.terrain_actif->matrice[joueur.x][joueur.y + 2] == 0)
				{
					joueur.nombre_deplacements++;
					joueur.terrain_actif->matrice[joueur.x][joueur.y + 1]--;
					joueur.terrain_actif->matrice[joueur.x][joueur.y + 2]++;
					JouerBruitage(7);
				}
			return;
			break;
		case GAUCHE:
			if (joueur.x > 1)
				if (joueur.terrain_actif->matrice[joueur.x - 1][joueur.y] == 1 &&
						joueur.terrain_actif->matrice[joueur.x - 2][joueur.y] == 0)
				{
					joueur.nombre_deplacements++;
					joueur.terrain_actif->matrice[joueur.x - 1][joueur.y]--;
					joueur.terrain_actif->matrice[joueur.x - 2][joueur.y]++;
					JouerBruitage(8);
				}
			return;
			break;
		case DROITE:
			if (joueur.x < LARGEUR_TERRAIN - 2)
				if (joueur.terrain_actif->matrice[joueur.x + 1][joueur.y] == 1 &&
						joueur.terrain_actif->matrice[joueur.x + 2][joueur.y] == 0)
				{
					joueur.nombre_deplacements++;
					joueur.terrain_actif->matrice[joueur.x + 1][joueur.y]--;
					joueur.terrain_actif->matrice[joueur.x + 2][joueur.y]++;
					JouerBruitage(8);
				}
			return;
			break;
		default:
			return;
	}
	return; 
}

