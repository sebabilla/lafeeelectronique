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
	
	InitialiserPays(pays);
	
	liste_terrains = NouveauTerrain();	
	
	joueur = NouveauJoueur();
	
//------------Boucle principale adaptée à emcc ou gcc-------------------

	#ifdef __EMSCRIPTEN__
		emscripten_set_main_loop(BouclePrincipale, -1, 1);
    #endif
    #ifndef __EMSCRIPTEN__
		while (partie->programme_en_cours)
			BouclePrincipale();
	#endif
	
//------------Sortie de la boucle, on nettoie tout----------------------
	
	LibererPartie(partie);
	
	LibererJoueur(joueur);
	
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
	switch(partie->etat)
	{
		case CHOIX_LANGUE:
			
			AfficherLangage(partie->langage);
			if (ActionsLangage(partie) == 1)
			{
				ChargerTextes(partie->langage);
				partie->etat = INTRODUCTION;
				//JouerBruitage(7);
			}
			break;
		
		case INTRODUCTION:
		
			AnimationIntroduction(partie);
			
			if (partie->introduction.stade > 2)
			{
				partie->introduction.stade = 0;
				partie->etat = NOUVELLE_PARTIE;
			}
			else if (partie->introduction.stade == -1)
			{
				partie->introduction.stade = 0;
				partie->langage = -1;
				partie->etat = CHOIX_LANGUE;
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
			
			if (partie->fee_pose_dechets < LARGEUR_TERRAIN * 20 - 1)
				AnimationFeeNouvelleAnnee1(partie, joueur, liste_terrains);
			
			else if (partie->fee_pose_dechets == LARGEUR_TERRAIN * 20 - 1)
				AjouterTousLesDechets(liste_terrains);
			
			else if (partie->fee_pose_dechets <= LARGEUR_TERRAIN * 23 - 1)
				AnimationFeeNouvelleAnnee2(partie, joueur, liste_terrains);
			
			else
			{			
				partie->fee_pose_dechets = 0;
				NouvelleAnnee(joueur, partie, liste_terrains);			
				partie->etat = EN_COURS;
			}
			break;
		
		case EN_COURS:
		
			ActionJoueur(joueur, partie); // La grosse fonction de jeu

			AfficherInfosJoueur(joueur);
			
			AfficherPetiteGnomette(joueur->x, joueur->y);	
			
			AfficherLesTerrainsAvecJoueur(liste_terrains, joueur);
			
			AfficherJoueur(joueur);
			
			if (AnneeFinie(joueur) == 1)
				partie->etat = NOUVELLE_ANNEE;
				
			if (partie->en_cours == 0)
			{
				partie->etat = PAUSE;
				partie->en_cours = 1;				
			}
				
			break;
			
		case PAUSE:
			
			ActionPause(joueur, partie);
			
			AfficherInfosJoueur(joueur);
			
			AfficherPetiteGnomette(joueur->x, joueur->y);
			
			AfficherLesTerrainsAvecJoueur(liste_terrains, joueur);
			
			AfficherPause();
			
			if (partie->en_pause == 0)
			{
				partie->etat = EN_COURS;
				partie->en_pause = 1;				
			}
			
			if (partie->en_pause == -1)
			{
				partie->etat = INTRODUCTION;
				partie->en_pause = 1;				
			}
			break;
		
		default:
			break;
	}
		
	AfficherLeRendu();
}





