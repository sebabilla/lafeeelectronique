#include <SDL.h>
#include <SDL_mixer.h>

#include "types.h"
#include "controle.h"
#include "affichage.h"
#include "musique.h"
#include "animations.h"

void AnimationIntroduction(Partie *partie)
{
	Clavier clavier = EntreeJoueur();
	if (clavier.bouton == FERMERFENETRE)
	{
		partie->programme_en_cours = 0;
		return ;
	}
		
	if (clavier.bouton == ECHAP)
	{
		partie->stade_intro = -1;
		return;
	}
	
	if (clavier.bouton == ENTREE || clavier.bouton == ESPACE)
		partie->stade_intro++;
	
	AfficherLan();
	
	int compteur_animation = SDL_GetTicks();
			
	switch(partie->stade_intro)
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

void AnimationFeeNouvelleAnnee1(Partie *partie, Joueur *joueur, Terrain *terrain)
{
	Clavier clavier = EntreeJoueur();
	if (clavier.bouton == FERMERFENETRE)
	{
		partie->programme_en_cours = 0;
		return;
	}
	
	
	AfficherInfosFee(joueur);
	
	AfficherLesTerrainsSansJoueur(terrain);
	
	int ralentir_animation = partie->fee_pose_dechets / 15;

	AfficherPetiteFee(ralentir_animation);
	
	if (partie->fee_pose_dechets % 15 == 0)
		JouerDechets(terrain, ralentir_animation);

	ApparitionColonneDechets(terrain, ralentir_animation);
}

void AnimationFeeNouvelleAnnee2(Partie *partie, Joueur *joueur, Terrain *terrain)
{
	AfficherInfosFee(joueur);

	AfficherPetiteFee(LARGEUR_TERRAIN);
	
	AfficherLesTerrainsSansJoueur(terrain);
}

void InitialiserTableauAnimation(Joueur *j)
{
	for (int i = 0; i < ANIM_RECYCLER + ANIM_DEBLOQUE + ANIM_POUSSER + ANIM_NIV_FINI; i++)
		{
			j->animation[i].x = 0;
			j->animation[i].y = 0;
			j->animation[i].temps = 0;
		}
}

void AnimationsJoueur(Joueur *j)
{
	if (j->animation[ANIM_RECYCLER].temps == 0 && // = ANIM_DEBLOQUE - 1
		j->animation[ANIM_RECYCLER + ANIM_DEBLOQUE].temps == 0) //  = ANIM_POUSSER - 1
		AfficherPetitGnome(j->x, j->y);
	else if (j->animation[ANIM_RECYCLER + ANIM_DEBLOQUE].temps != 0)
		{
			AfficherPousser(j->x, j->y);
			j->animation[ANIM_RECYCLER + ANIM_DEBLOQUE].temps--;
		}

	for (int i = 0; i < ANIM_RECYCLER; i++)
		if (j->animation[i].temps != 0)
			{
				AfficherTir(j->animation[i].x + 8 * TUILE, j->animation[i].y);
				j->animation[i].temps--;
				j->animation[i].x += 3;
			}
}

void AjouterAnimRecyclage(Joueur *j)
{
	for (int i = 0; i < ANIM_RECYCLER; i++)
		if (j->animation[i].temps == 0)
			{
				j->animation[i].temps = 20;
				j->animation[i].x = j->x + 2 * TUILE;
				j->animation[i].y = j->y + TUILE;
				return;
			}
}

void AnimationPousser(Joueur *j)
{
	j->animation[ANIM_RECYCLER + ANIM_DEBLOQUE].temps = 20;
}

void AnimationDebloques(Joueur *j)
{
	if (j->animation[ANIM_RECYCLER].temps < 1)
		return;
	j->animation[ANIM_RECYCLER].temps--; // position de ANIM_DEBLOQUE
	j->animation[ANIM_RECYCLER].x++;
	
	AfficherPetitGnome(LARGEUR_FENETRE - j->animation[ANIM_RECYCLER].x - 12 * TUILE, j->animation[ANIM_RECYCLER].y);
	
	int ligne = 36;
	if (j->annee == 2027)
		ligne = 37;
	if (j->annee == 2030)
		ligne = 38;
		
	AfficherDebloque(ligne, LARGEUR_FENETRE - j->animation[ANIM_RECYCLER].x - 15 * TUILE, j->animation[ANIM_RECYCLER].y - 2 * TUILE);	
}

void AnimationFindePartie(Joueur *j, Partie *p)
{
	Clavier clavier = EntreeJoueur();
	if (clavier.bouton == FERMERFENETRE)
	{
		p->programme_en_cours = 0;
		return ;
	}
	
	if (clavier.bouton == ENTREE || clavier.bouton == ESPACE || clavier.bouton == ECHAP)
	{	
		p->stade_fin++;
		if (p->stade_fin == 2)
			JouerMusique(0);
	}	
	
	AfficherLan();
	
	int compteur_animation = SDL_GetTicks();
			
	switch(p->stade_fin)
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
			for (int i = 0; i < 2; i++)
				AfficherTexteIntro(47 + i, 250, 50 + i * 50);
			
			AfficherYeux();
			
			break;
		default:
			break;
	}
}
