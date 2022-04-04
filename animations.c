#include <SDL.h>

#include "types.h"
#include "controle.h"
#include "affichage.h"

int AnimationIntroduction(Partie *partie)
{
	Clavier clavier = EntreeJoueur();
	if (clavier.bouton == FERMERFENETRE)
		partie->programme_en_cours = 0;
	
	if (clavier.bouton == ENTREE || clavier.bouton == ESPACE)
		partie->introduction.stade++;
	
	AfficherFondGris();
	
	partie->introduction.temps++;
	if (partie->introduction.temps % 60 == 0)
		partie->introduction.inverser *= -1;
			
	switch(partie->introduction.stade)
	{
		case 0:
			AfficherTexteIntro(10, LARGEUR_FENETRE / 2, 50);
			AfficherTexteIntro(11, LARGEUR_FENETRE / 2, 100);
			AfficherTexteIntro(12, LARGEUR_FENETRE / 2, 150);
			AfficherTexteIntro(13, LARGEUR_FENETRE / 2, 200);
			AfficherTexteIntro(14, LARGEUR_FENETRE / 2, 250);
			if (partie->introduction.inverser == 1)
				AfficherGrandeFee(partie->introduction.temps % 60 / 2, 0);
			else
				AfficherGrandeFee((60 - partie->introduction.temps % 60) / 2, 0);
			break;
		case 1:
			AfficherTexteIntro(15, 50, HAUTEUR_FENETRE / 2);
			AfficherTexteIntro(16, 50, HAUTEUR_FENETRE / 2 + 50);
			AfficherTexteIntro(17, 50, HAUTEUR_FENETRE / 2 + 100);
			AfficherTexteIntro(18, 50, HAUTEUR_FENETRE / 2 + 150);
			if (partie->introduction.inverser == 1)
				AfficherGrandeGnomette(LARGEUR_FENETRE / 5 * 3 + partie->introduction.temps % 60 / 2, HAUTEUR_FENETRE / 3);
			else
				AfficherGrandeGnomette(LARGEUR_FENETRE / 5 * 3 + (60 - partie->introduction.temps % 60) / 2, HAUTEUR_FENETRE / 3);					
			break;
		case 2:
			AfficherTexteIntro(19, LARGEUR_FENETRE / 2, 50);
			AfficherTexteIntro(20, LARGEUR_FENETRE / 2, 100);
			AfficherTexteIntro(21, LARGEUR_FENETRE / 2, 150);
			AfficherTexteIntro(22, LARGEUR_FENETRE / 2, 200);
			AfficherTexteIntro(23, LARGEUR_FENETRE / 2.5, HAUTEUR_FENETRE / 2 - 100);
			AfficherTexteIntro(24, LARGEUR_FENETRE / 2.5, HAUTEUR_FENETRE / 2 - 50);
			AfficherTexteIntro(25, LARGEUR_FENETRE / 2.5, HAUTEUR_FENETRE / 2);
			AfficherTexteIntro(26, LARGEUR_FENETRE / 2.5, HAUTEUR_FENETRE / 2 + 50);
			AfficherTexteIntro(27, LARGEUR_FENETRE / 2.5, HAUTEUR_FENETRE / 2 + 100);
			AfficherTexteIntro(28, LARGEUR_FENETRE / 2.5, HAUTEUR_FENETRE / 2 + 150);
			
			if (partie->introduction.inverser == 1)
			{
				AfficherGrandeFee(partie->introduction.temps % 60 / 2, 0);
				AfficherGrandeGnomette(LARGEUR_FENETRE / 5 * 3 + partie->introduction.temps % 60 / 2, HAUTEUR_FENETRE / 3);
			}
			else
			{
				AfficherGrandeFee((60 - partie->introduction.temps % 60) / 2, 0);
				AfficherGrandeGnomette(LARGEUR_FENETRE / 5 * 3 + (60 - partie->introduction.temps % 60) / 2, HAUTEUR_FENETRE / 3);
			}
			break;
		default:
			partie->introduction.stade = 0;
			partie->introduction.temps = 0;
			return 1;
			break;
	}
	return 0;
}

void AnimationFeeNouvelleAnnee1(Partie *partie, Joueur *joueur, Terrain *terrain)
{
	AfficherInfosFee(joueur);
	
	AfficherLesTerrainsSansJoueur(terrain);
	
	int ralentir_animation = partie->fee_pose_dechets / 20;

	AfficherPetiteFee(ralentir_animation);

	ApparitionColonneDechets(terrain, ralentir_animation);
}

void AnimationFeeNouvelleAnnee2(Partie *partie, Joueur *joueur, Terrain *terrain)
{
	AfficherInfosFee(joueur);
	
	AfficherLesTerrainsSansJoueur(terrain);
}
