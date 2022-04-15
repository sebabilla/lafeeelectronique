#include <SDL.h>
#include <SDL_mixer.h>

#include "types.h"
#include "controle.h"
#include "affichage.h"
#include "musique.h"
#include "animations.h"

void AnimationIntroduction(void)
{
	Clavier clavier = EntreeJoueur();
	if (clavier.bouton == FERMERFENETRE)
	{
		partie.programme_en_cours = 0;
		return ;
	}
		
	if (clavier.bouton == ECHAP)
	{
		partie.stade_intro = -1;
		return;
	}
	
	if (clavier.bouton == ENTREE || clavier.bouton == ESPACE)
		partie.stade_intro++;
	
	AfficherLan();
	
	int compteur_animation = SDL_GetTicks();
			
	switch(partie.stade_intro)
	{
		case 0:
			for (int i = 0; i < 5; i++)
				AfficherTexteIntro(10 + i, LARGEUR_FENETRE / 2, 50 + 50 * i);
			
			if (compteur_animation / 25 % 60 > 30)
				AfficherGrandeFee(compteur_animation / 50 % 30, 0);
			else
				AfficherGrandeFee(30 - compteur_animation / 50 % 30, 0);
			break;
		case 1:
			for (int i = 0; i < 4; i++)
				AfficherTexteIntro(15 + i, 50, HAUTEUR_FENETRE / 2 + i * 50);
			
			if (compteur_animation / 25 % 60 > 30)
				AfficherGrandGnome(LARGEUR_FENETRE / 5 * 3 + compteur_animation / 50 % 30, HAUTEUR_FENETRE / 3);
			else
				AfficherGrandGnome(LARGEUR_FENETRE / 5 * 3 + (30 - compteur_animation / 50 % 30), HAUTEUR_FENETRE / 3);					
			break;
		case 2:
			for (int i = 0; i < 4; i++)
				AfficherTexteIntro(19 + i, LARGEUR_FENETRE / 2, 50 + i * 50);
			for (int i = 0; i < 6; i++)
				AfficherTexteIntro(23 + i, LARGEUR_FENETRE / 2.5, HAUTEUR_FENETRE / 2 - 100 + i * 50);

			
			if (compteur_animation / 25 % 60 > 30)
			{
				AfficherGrandeFee(compteur_animation / 50 % 30, 0);
				AfficherGrandGnome(LARGEUR_FENETRE / 5 * 3 + compteur_animation / 50 % 30, HAUTEUR_FENETRE / 3);
			}
			else
			{
				AfficherGrandeFee(30 - compteur_animation / 50 % 30, 0);
				AfficherGrandGnome(LARGEUR_FENETRE / 5 * 3 + (30 - compteur_animation / 50 % 30), HAUTEUR_FENETRE / 3);					
			}
			break;
		default:
			break;
	}
}

void AnimationFeeNouvelleAnnee1(Terrain *terrain)
{
	Clavier clavier = EntreeJoueur();
	if (clavier.bouton == FERMERFENETRE)
	{
		partie.programme_en_cours = 0;
		return;
	}
	
	
	AfficherAnnee(1);
	
	AfficherLesTerrainsSansJoueur(terrain);
	
	int ralentir_animation = partie.fee_pose_dechets / 15;

	AfficherPetiteFee(ralentir_animation);
	
	if (partie.fee_pose_dechets % 15 == 0)
		JouerDechets(terrain, ralentir_animation);

	ApparitionColonneDechets(terrain, ralentir_animation);
}

void AnimationFeeNouvelleAnnee2(Terrain *terrain)
{
	AfficherAnnee(1);

	AfficherPetiteFee(LARGEUR_TERRAIN);
	
	AfficherLesTerrainsSansJoueur(terrain);
}

void AnimationsJoueur(void)
{
	if (joueur.animation[ANIM_RECYCLER].temps == 0 && // = ANIM_DEBLOQUE - 1
		joueur.animation[ANIM_RECYCLER + ANIM_DEBLOQUE].temps == 0) //  = ANIM_POUSSER - 1
		AfficherPetitGnome(joueur.x + 8 * TUILE, joueur.y);
	else if (joueur.animation[ANIM_RECYCLER + ANIM_DEBLOQUE].temps != 0)
		{
			AfficherPousser(joueur.x + 8 * TUILE, joueur.y);
			joueur.animation[ANIM_RECYCLER + ANIM_DEBLOQUE].temps--;
		}

	for (int i = 0; i < ANIM_RECYCLER; i++)
		if (joueur.animation[i].temps != 0)
			{
				AfficherTir(joueur.animation[i].x + 8 * TUILE, joueur.animation[i].y);
				joueur.animation[i].temps--;
				joueur.animation[i].x += 3;
			}
}

void AjouterAnimRecyclage(void)
{
	for (int i = 0; i < ANIM_RECYCLER; i++)
		if (joueur.animation[i].temps == 0)
			{
				joueur.animation[i].temps = 20;
				joueur.animation[i].x = joueur.x + 2 * TUILE;
				joueur.animation[i].y = joueur.y + TUILE;
				return;
			}
}

void AnimationPousser(void)
{
	joueur.animation[ANIM_RECYCLER + ANIM_DEBLOQUE].temps = 20;
}

void AnimationDebloques(void)
{
	if (joueur.animation[ANIM_RECYCLER].temps < 1)
		return;
	joueur.animation[ANIM_RECYCLER].temps--; // position de ANIM_DEBLOQUE
	joueur.animation[ANIM_RECYCLER].x++;
	
	AfficherPetitGnome(LARGEUR_FENETRE - joueur.animation[ANIM_RECYCLER].x - 12 * TUILE, joueur.animation[ANIM_RECYCLER].y);
	
	int ligne = 36;
	if (joueur.annee == 2025)
		ligne = 90;
	if (joueur.annee == 2027)
		ligne = 37;
	if (joueur.annee == 2028)
		ligne = 91;
	if (joueur.annee == 2030)
		ligne = 38;
		
	AfficherDebloque(ligne, LARGEUR_FENETRE - joueur.animation[ANIM_RECYCLER].x - 15 * TUILE, joueur.animation[ANIM_RECYCLER].y - 2 * TUILE);	
}

void AnimationFindePartie(Terrain *terrain)
{
	Clavier clavier = EntreeJoueur();
	if (clavier.bouton == FERMERFENETRE)
	{
		partie.programme_en_cours = 0;
		return ;
	}
	
	if (clavier.bouton == ENTREE || clavier.bouton == ESPACE || clavier.bouton == ECHAP)
	{	
		partie.stade_fin++;
		if (partie.stade_fin == 2)
			JouerMusique(0);
	}	
	
	AfficherLan();
	
	int compteur_animation = SDL_GetTicks();
	
	int alignement;
			
	switch(partie.stade_fin)
	{
		case 0:
			for (int i = 0; i < 4; i++)
				AfficherTexteIntro(39 + i, LARGEUR_FENETRE / 2, 50 + 50 * i);
			
			if (compteur_animation / 25 % 60 > 30)
				AfficherGrandeFeeFinale(compteur_animation / 50 % 30, 0);
			else
				AfficherGrandeFeeFinale(30 - compteur_animation / 50 % 30, 0);
			break;
		case 1:
			for (int i = 0; i < 4; i++)
				AfficherTexteIntro(43 + i, 50, 200 + i * 50);
			
			if (compteur_animation / 25 % 60 > 30)
				AfficherFeeGnome(LARGEUR_FENETRE - 500 + compteur_animation / 50 % 30, HAUTEUR_FENETRE - 500);
			else
				AfficherFeeGnome(LARGEUR_FENETRE - 470 - compteur_animation / 50 % 30, HAUTEUR_FENETRE - 500);
			break;
		case 2:
			alignement = 300;
			if (partie.langage == 2)
				alignement = 250;
			else if (partie.langage == 3)
				alignement = 180;
			for (int i = 0; i < 2; i++)
				AfficherTexteIntro(47 + i, alignement, 50 + i * 50);
			
			AfficherYeux();
			
			break;
		case 3:
			AfficherPerformances(terrain);
		default:
			break;
	}
}
