#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL_mixer.h>

#ifdef __EMSCRIPTEN__
	#include <emscripten.h>
#endif

#include "types.h"
#include "controle.h"
#include "jeu.h"
#include "pays.h"
#include "affichage.h"
#include "musique.h"

Partie partie;
Terrain *liste_terrains;
Joueur *joueur;
Pays pays[NOMBRE_PAYS];

void BouclePrincipale(void);

int main(int argc, char *argv[])
{
	
	srand(time(NULL));

	InitialisationAffichage();
	
	InitialisationSon();
	
	InitialisationPartie(&partie);
	
	TextureLangages();
	
	InitialiserPays(pays);
	
	liste_terrains = NouveauTerrain();
	InitialiserTerrain(liste_terrains, &pays[0]);
	for (int i = 1; i < NOMBRE_PAYS; i++)
		AjouterTerrainFin(liste_terrains, &pays[i]);
	
	for (int i = 0; i < 1; i++)
	{
		GenererTousLesDechets(liste_terrains);
		AjouterTousLesDechets(liste_terrains);
	}
	
	joueur = NouveauJoueur();
	InitialiserJoueur(joueur, liste_terrains);

//------------Boucle principale adaptée à emcc ou gcc-------------------

	#ifdef __EMSCRIPTEN__
		emscripten_set_main_loop(BouclePrincipale, -1, 1);
    #endif
    #ifndef __EMSCRIPTEN__
		while (partie.programme_en_cours)
			BouclePrincipale();
	#endif
	
//------------Sortie de la boucle, on nettoie tout----------------------
	
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
//------------Menu de démarrage-----------------------------------------
	if (partie.langage == 0) // remplacer par < 0
	{
		AfficherIllustration();
		AfficherLangage(partie.langage);
		if (ActionsLangage(&partie) == 1)
		{
			ChargerTextes(partie.langage);
			//JouerBruitage(7);
		}
	}
	else
	{	
	
	Bouton action = ActionJoueur(joueur);
	
	if (action == FERMERFENETRE)
		partie.programme_en_cours = 0;

	AfficherLesTerrains(liste_terrains, joueur);
	AfficherJoueur(joueur);
	}
	

	
	
	AfficherLeRendu();

//------------Affichage quand le moteur physique est skippé-------------
/*									
	if (partie.pause == 1 || partie.resultat == 1 || partie.resultat == 0)		
		ActionsALArret(&partie);
*/
}





