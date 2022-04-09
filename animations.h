#ifndef __ANIMATION_H__
#define __ANIMATION_H__

void AnimationIntroduction(Partie *partie);
void AnimationFeeNouvelleAnnee1(Partie *partie, Joueur *joueur, Terrain *terrain); 
void AnimationFeeNouvelleAnnee2(Partie *partie, Joueur *joueur, Terrain *terrain); 
void InitialiserTableauAnimation(Joueur *j);
void AnimationsJoueur(Joueur *j);
void AjouterAnimRecyclage(Joueur *j);
void AnimationPousser(Joueur *j);
void AnimationDebloques(Joueur *j);
void AnimationFindePartie(Joueur *j, Partie *p);

#endif
