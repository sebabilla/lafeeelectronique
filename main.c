#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL_mixer.h>

#ifdef __EMSCRIPTEN__
	#include <emscripten.h>
#endif

#include "types.h"
#include "jeu.h"
#include "pays.h"
#include "affichage.h"
#include "musique.h"
#include "animations.h"
#include "controle.h"

Partie *partie;
Terrain *liste_terrains;
Joueur *joueur;
Pays pays[NOMBRE_PAYS];

void BouclePrincipale(void);

int main(int argc, char *argv[])
{
	
	srand(time(NULL));

	InitialisationAffichage();
	
	InitialisationSon();
	
	partie = InitialisationPartie();
	
	TextureLangages();
	
//------------Boucle principale adaptée à emcc ou gcc-------------------

	#ifdef __EMSCRIPTEN__
		emscripten_set_main_loop(BouclePrincipale, -1, 1);
    #endif
    #ifndef __EMSCRIPTEN__
		while (partie->programme_en_cours)
			BouclePrincipale();
	#endif
	
//------------Sortie de la boucle, on nettoie tout----------------------
	
	if (partie->etat != CHOIX_LANGUE)
		DetruireTextes();
		
	if (partie->etat != CHOIX_LANGUE && partie->etat != INTRODUCTION && partie->etat != MENU_PRINCIPAL && partie->etat != CREDITS)
		DetruireTerrains(liste_terrains);
	
	LibererPartie(partie);
	
	LibererJoueur(joueur);
	
	DestructionSon();
	
	DestructionAffichage();
	
	#ifdef __EMSCRIPTEN__
		emscripten_cancel_main_loop();
	#endif
	
	return EXIT_SUCCESS;
}

//------------Quand le jeu tourne---------------------------------------

void BouclePrincipale(void)
{
	switch(partie->etat)
	{
		case CHOIX_LANGUE:
			
			AfficherLangage(partie->langage);
			if (ActionsLangage(partie) == 1)
				partie->etat = CHARGEMENT;
			break;
			
		case CHARGEMENT:
			AfficherChargement(partie);
			
			if (partie->chargement == 0)
			{			
				InitialiserPays(pays);			
				liste_terrains = NouveauTerrain();		
				joueur = NouveauJoueur();
			}
			
			if (partie->chargement == 1)
				ChargerLesImages();
			if (partie->chargement == 2)
				ChargerLesIcones();
			if (partie->chargement == 3)
				ChargerLesTuiles();
			if (partie->chargement == 4)
				ChargerLesMusiques();
			if (partie->chargement == 5)
				ChargerLesBruitages();
			if (partie->chargement == 6)
				ChargerTextes(partie->langage);			
			if (partie->chargement == 7)
			{
				partie->chargement = 5;
				partie->etat = INTRODUCTION;
				JouerBruitage(9);
				JouerMusique(0);
			}
			
			partie->chargement++;		
			break;			
		
		case INTRODUCTION:
		
			AnimationIntroduction(partie);
			
			if (partie->stade_intro > 2)
			{
				partie->stade_intro = 0;
				partie->etat = MENU_PRINCIPAL;
				JouerMusique(1);
			}
			else if (partie->stade_intro == -1)
			{
				partie->stade_intro = 0;
				partie->langage = -1;
				partie->etat = CHOIX_LANGUE;
				DetruireTextes();
				Mix_FadeOutMusic(500);
			}				
			break;
			
		case MENU_PRINCIPAL:
			
			AfficherMenu(partie);
			
			AfficherTitre();
			
			int choix = ActionsMenu(partie);
			
			if (choix == 1)
			{
				if (partie->menu == 0)
				{
					partie->etat = NOUVELLE_PARTIE;
					Mix_FadeOutMusic(500);
					JouerBruitage(9);
				}
				if (partie->menu == 2 && partie->debloque > 0)
				{
					partie->etat = CREDITS;
					JouerBruitage(9);
				}
			}
			else if (choix == -1)
			{
				partie->stade_intro = 0;
				partie->langage = -1;
				partie->etat = CHOIX_LANGUE;
				Mix_FadeOutMusic(500);
			}
				
			break;
		
		case NOUVELLE_PARTIE:
			
			InitialiserTerrain(liste_terrains, &pays[0]);
			for (int i = 1; i < NOMBRE_PAYS; i++)
				AjouterTerrainFin(liste_terrains, &pays[i]);
			
			InitialiserJoueur(joueur, liste_terrains);
			
			partie->etat = NOUVELLE_ANNEE;
			break;
		
		case NOUVELLE_ANNEE:
			
			if (partie->fee_pose_dechets == 0)
				GenererTousLesDechets(liste_terrains);
			
			partie->fee_pose_dechets++;
			
			if (partie->fee_pose_dechets < LARGEUR_TERRAIN * 15 - 1)
			{
				AnimationFeeNouvelleAnnee1(partie, joueur, liste_terrains);
			}		
			else
			{
				if (partie->fee_pose_dechets == LARGEUR_TERRAIN * 15 - 1)
				{
					AjouterTousLesDechets(liste_terrains);
					JouerBruitage(5);
					
				}
				else if (partie->fee_pose_dechets == LARGEUR_TERRAIN * 20)
				{			
					partie->fee_pose_dechets = 0;
					NouvelleAnnee(joueur, partie, liste_terrains);
					if (joueur->annee < 2027)
						JouerMusique(2);
					else if (joueur->annee == 2031)
						JouerMusique(0);
					else
						JouerMusique(3);
					partie->etat = EN_COURS;
				}
				AnimationFeeNouvelleAnnee2(partie, joueur, liste_terrains);
			}
			
			if (joueur->animation[ANIM_RECYCLER].temps > 0) // position de ANIM_DEBLOQUE
					AnimationDebloques(joueur);				
			
			break;
		
		case EN_COURS:
		
			ActionJoueur(joueur, partie); // La grosse fonction de jeu	
			
			AfficherLesTerrainsAvecJoueur(liste_terrains, joueur);
			
			AfficherJoueur(joueur);
			
			AfficherInfosJoueur(joueur);
			
			AnimationsJoueur(joueur);
			
			AfficherTuto(joueur);
	
			if (AnneeFinie(joueur) == 1)
			{
				if (joueur->annee == 2031)
				{
						partie->etat = FIN_DE_PARTIE;
						joueur->temps = (SDL_GetTicks() - joueur->temps) / 1000;
						JouerMusique(4);
				}
						
				else
				{
					partie->etat = NOUVELLE_ANNEE;
					DebloquerMode(joueur, partie);
					Mix_FadeOutMusic(500);
				}
			}
				
			if (partie->en_cours == 0)
			{
				partie->etat = PAUSE;
				partie->en_cours = 1;
				PauseMusique();				
			}
				
			break;
			
		case FIN_DE_PARTIE:
			AnimationFindePartie(joueur, partie);
			
			if (partie->stade_fin > 3)
			{
				DetruireTerrains(liste_terrains);
				LibererJoueur(joueur);
				liste_terrains = NouveauTerrain();
				joueur = NouveauJoueur();				
				partie->stade_fin = 0;
				partie->etat = MENU_PRINCIPAL;
				JouerMusique(1);
			}
			break;
			
		case CREDITS:
		
			if (ActionsMenu(partie) == 1)
				partie->etat = MENU_PRINCIPAL;
			AfficherCredits();
			break;
			
		case PAUSE:
			
			AfficherInfosJoueur(joueur);
			
			AfficherPetitGnome(joueur->x, joueur->y);
			
			AfficherLesTerrainsAvecJoueur(liste_terrains, joueur);
			
			AfficherPause(partie);
			
			if (ActionPause(joueur, partie) == 1)
				switch(partie->menu_pause)
				{
					case 0:
						partie->etat = EN_COURS;
						PauseMusique();	
						break;
					case 1:
						ResetTousLesTerrains(liste_terrains);
						joueur->terrain_actif = liste_terrains;
						joueur->x = -1;
						joueur->y = 0;
						joueur->nombre_resets++;
						partie->etat = EN_COURS;
						partie->menu_pause = 0;
						PauseMusique();
						break;
					case 2:
						DetruireTerrains(liste_terrains);
						LibererJoueur(joueur);
						liste_terrains = NouveauTerrain();
						joueur = NouveauJoueur();
						partie->etat = MENU_PRINCIPAL;
						PauseMusique();
						JouerMusique(1);
						break;
					default:
						break;
				}
			break;
		
		default:
			break;
	}
		
	AfficherLeRendu();
}





