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

Partie partie; // variable externe, cf types.h
Joueur joueur; // variable externe, cf types.h
Terrain *liste_terrains;
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
		while (partie.programme_en_cours)
			BouclePrincipale();
	#endif
	
//------------Sortie de la boucle, on nettoie tout----------------------
	
	if (partie.etat != CHOIX_LANGUE)
		DetruireTextes();
		
	if (partie.etat != CHOIX_LANGUE && partie.etat != INTRODUCTION && partie.etat != MENU_PRINCIPAL && partie.etat != CREDITS)
		DetruireTerrains(liste_terrains);
	
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
	switch(partie.etat)
	{
		case CHOIX_LANGUE:
			
			AfficherLangage(partie.langage);
			if (ActionsLangage() == 1)
				partie.etat = CHARGEMENT;
			break;
			
		case CHARGEMENT:
			AfficherChargement();
			
			if (partie.chargement == 0)
			{			
				InitialiserPays(pays);			
				liste_terrains = NouveauTerrain();		
			}
			
			if (partie.chargement == 1)
				ChargerLesImages();
			if (partie.chargement == 2)
				ChargerLesIcones();
			if (partie.chargement == 3)
				ChargerLesTuiles();
			if (partie.chargement == 4)
				ChargerLesMusiques();
			if (partie.chargement == 5)
				ChargerLesBruitages();
			if (partie.chargement == 6)
				ChargerTextes(partie.langage);			
			if (partie.chargement == 7)
			{
				partie.chargement = 5;
				partie.etat = INTRODUCTION;
				JouerBruitage(9);
				JouerMusique(0);
			}
			
			partie.chargement++;		
			break;			
		
		case INTRODUCTION:
		
			AnimationIntroduction();
			
			if (partie.stade_intro > 2)
			{
				partie.stade_intro = 0;
				partie.etat = MENU_PRINCIPAL;
				JouerMusique(1);
			}
			else if (partie.stade_intro == -1)
			{
				partie.stade_intro = 0;
				partie.langage = -1;
				partie.etat = CHOIX_LANGUE;
				DetruireTextes();
				Mix_FadeOutMusic(500);
			}				
			break;
			
		case MENU_PRINCIPAL:
			
			AfficherTitre();
			
			AfficherMenu(0, 3, 33, LARGEUR_FENETRE / 2, 300, HAUTEUR_FENETRE / 2 + 50);
			
			int choix = ActionsMenu(0, 3);
			
			if (choix == 1)
			{
				if (partie.menu[0] == 0)
				{
					partie.etat = NOUVELLE_PARTIE;
					Mix_FadeOutMusic(500);
					JouerBruitage(9);
				}
				if (partie.menu[0] == 1 && partie.debloque[1] == 1)
				{
					partie.etat = NOUVELLE_PARTIE;
					Mix_FadeOutMusic(500);
					JouerBruitage(9);
				}
				if (partie.menu[0] == 2 && partie.debloque[2] == 1)
				{
					partie.etat = CREDITS;
					JouerBruitage(9);
				}
			}
			else if (choix == -1)
			{
				partie.stade_intro = 0;
				partie.langage = -1;
				partie.etat = CHOIX_LANGUE;
				Mix_FadeOutMusic(500);
			}
				
			break;
		
		case NOUVELLE_PARTIE:
			
			InitialiserTerrain(liste_terrains, &pays[0]);
			for (int i = 1; i < NOMBRE_PAYS; i++)
				AjouterTerrainFin(liste_terrains, &pays[i]);
			
			joueur = InitialiserJoueur(liste_terrains);
			
			if (partie.menu[0] == 1)
				joueur.bonus = 20;
			
			partie.etat = NOUVELLE_ANNEE;
			break;
		
		case NOUVELLE_ANNEE:
			
			if (partie.fee_pose_dechets == 0)
				GenererTousLesDechets(liste_terrains);
			
			partie.fee_pose_dechets++;
			
			if (partie.fee_pose_dechets < LARGEUR_TERRAIN * 15 - 1)
			{
				AnimationFeeNouvelleAnnee1(liste_terrains);
			}		
			else
			{
				if (partie.fee_pose_dechets == LARGEUR_TERRAIN * 15 - 1)
				{
					AjouterTousLesDechets(liste_terrains);
					JouerBruitage(5);
					
				}
				else if (partie.fee_pose_dechets == LARGEUR_TERRAIN * 20)
				{			
					partie.fee_pose_dechets = 0;
					NouvelleAnnee(liste_terrains);
					if (joueur.annee < 2027)
						JouerMusique(2);
					else if (joueur.annee == 2031)
						JouerMusique(0);
					else
						JouerMusique(3);
					partie.etat = EN_COURS;
				}
				AnimationFeeNouvelleAnnee2(liste_terrains);
			}
			
			if (joueur.animation[ANIM_RECYCLER].temps > 0) // position de ANIM_DEBLOQUE
					AnimationDebloques();				
			
			break;
		
		case EN_COURS:
		
			ActionJoueur(); // La grosse fonction de jeu	
			
			AfficherCiel(joueur.terrain_actif);
			
			AfficherLesTerrainsAvecJoueur(liste_terrains);
			
			AfficherJoueur();
			
			AfficherAnnee(0);
			
			AnimationsJoueur();
			
			AfficherTuto();
	
			if (AnneeFinie() == 1)
			{
				if (joueur.annee == 2031)
				{
						partie.etat = FIN_DE_PARTIE;
						joueur.temps = (SDL_GetTicks() - joueur.temps) / 1000;
						JouerMusique(4);
				}
						
				else
				{
					partie.etat = NOUVELLE_ANNEE;
					joueur.pouvoir[1] = 1;
					DebloquerMode();
					Mix_FadeOutMusic(500);
				}
			}
				
			if (partie.en_cours == 0)
			{
				partie.etat = OPTIONS;
				partie.en_cours = 1;
				partie.menu_niveau = 0;
				PauseMusique();				
			}
				
			break;
			
		case FIN_DE_PARTIE:
			AnimationFindePartie(liste_terrains);
			
			if (partie.stade_fin > 3)
			{
				DetruireTerrains(liste_terrains);
				liste_terrains = NouveauTerrain();				
				partie.stade_fin = 0;
				partie.etat = MENU_PRINCIPAL;
				JouerMusique(1);
			}
			break;
			
		case CREDITS:
		
			if (ActionsMenu(0, 1) == 1)
				partie.etat = MENU_PRINCIPAL;
			AfficherCredits();
			break;
			
		case OPTIONS:
			
			AfficherAnnee(0);
			
			AfficherLesTerrainsAvecJoueur(liste_terrains);
			
			AfficherOptions();
			
			if (partie.menu_niveau == 0)
			{
				if (ActionsMenu(1, 5) == 1)
				{
					switch (partie.menu[1])
					{
						case 0:
							partie.etat = EN_COURS;
							PauseMusique();
							break;
						case 1:
							if (joueur.pouvoir[1] == 1)
								partie.menu_niveau = 1;
							break;
						case 2:
							partie.menu_niveau = 1;
							break;
						case 3:
							if (joueur.pouvoir[3] == 1)
								partie.menu_niveau = 1;
							break;
						case 4:
							if (joueur.pouvoir[4] == 1)
								partie.menu_niveau = 1;
							break;
						default:
							break;
					}
				}
			}	
			else if (partie.menu_niveau == 1)
			{
				AfficherMenu(2, 2, 86, 540, 200, 250 + (partie.menu[1] - 1) * 60);
				
				switch (partie.menu[1])
				{
					case 1:
						if (ActionsMenu(2, 2) == 1)
						{
							if (partie.menu[2] == 1)
							{
								ResetTousLesTerrains(liste_terrains);
								joueur.terrain_actif = liste_terrains;
								joueur.x = -1;
								joueur.y = 0;
								joueur.nombre_resets++;
								partie.etat = EN_COURS;
								partie.menu[1] = 0;
								PauseMusique();
							}
							else
								partie.menu_niveau = 0;
						}
						break;
						
					case 2:
						if (ActionsMenu(2, 2) == 1)
						{
							if (partie.menu[2] == 1)
							{
								DetruireTerrains(liste_terrains);
								liste_terrains = NouveauTerrain();
								partie.etat = MENU_PRINCIPAL;
								PauseMusique();
								JouerMusique(1);
							}
							else
								partie.menu_niveau = 0;
						}
						break;
						
					case 3:
						TexteTuto(88);							
						if (ActionsMenu(2, 2) == 1)
						{
							if (partie.menu[2] == 1 && (joueur.terrain_actif->pays->numero == 1 ||
														joueur.terrain_actif->pays->numero == 4 ||
														joueur.terrain_actif->pays->numero == 8 ||
														joueur.terrain_actif->pays->numero == 9))
							{
								EchangerAppareils(liste_terrains);
								joueur.pouvoir[3] = 0;
								joueur.pouvoir[1] = 0; // empecher reset
								partie.menu_niveau = 2;
							}
						else
							partie.menu_niveau = 0;
						}	
						break;
						
					case 4:
						TexteTuto(89);
						if (ActionsMenu(2, 2) == 1)
						{
							if (partie.menu[2] == 1 && (joueur.terrain_actif->pays->numero == 0 ||
														joueur.terrain_actif->pays->numero == 3 ||
														joueur.terrain_actif->pays->numero == 5 ||
														joueur.terrain_actif->pays->numero == 8))
							{
								joueur.pouvoir[4] = 0;
								partie.menu_niveau = 2;
							}
							else
								partie.menu_niveau = 0;
						}
						break;
						
					default:
						break;										
				}
			}
			else if (partie.menu_niveau == 2)
			{
				if (partie.menu[1] == 3)
					AfficherCarteAction(3);
				if (partie.menu[1] == 4)
					AfficherCarteAction(4);
				if (ActionsMenu(1, 1) == 1)
				{
					partie.etat = EN_COURS;
					PauseMusique();
				}
			}
			break;
		
		default:
			break;
	}
		
	AfficherLeRendu();
}





