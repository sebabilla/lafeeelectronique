#ifndef __AFFICHAGE_H__
#define __AFFICHAGE_H__

#define LARGEUR_FENETRE 1024
#define HAUTEUR_FENETRE 768
#define FPS 16

#define LIGNES_TEXTE 14
#define NOMBRE_LANGAGES 4

#define NOIR 0xFF000000
#define GRIS 0xFF7F7F7F
#define ROUGE 0xFF0000FF
#define VERT 0xFF008000
#define BLEU 0xFFFF901E
#define JAUNE 0xFF00FFFF
#define ROSE 0xFFC693FE
#define MARRON 0xFF024EAE
#define TURQUOISE 0xFFC3E617
#define BLANC 0xFFFFFFFF


//------------Gestion de la SDL-----------------------------------------
void SDL_ExitWithError(const char *message);
void limit_fps(void);
void InitialisationAffichage(void);
void AfficherLeRendu(void);
void DestructionAffichage(void);

//------------Affichage de l'unite de base------------------------------

void AfficherTuile(int x, int y, int statut);

//------------Affichage des terrains------------------------------------
void AfficherLesTerrains(const Terrain *t, const Joueur *j);
void AfficherUnTerrain(const Terrain *t);

//------------Affichage du joueur---------------------------------------
void AfficherJoueur(const Joueur *j);
void AssombrirTerrain(const Terrain *t);
void IlluminerTerrain(const Terrain *t);

//------------Affichage des textes--------------------------------------
void ChargerTextes(int l);
int VerificationLangage(const char *s);
SDL_Texture *TextureTexte(char *texte, int i);
void TextesTraduits(char *l);
void TextesNiveaux(void);
void EcrireTexteProvisoire(char *texte, int X, int Y, int W, int H);
void AfficherBravo(void);
void AfficherPause(void);
void TextureLangages(void);
void AfficherLangage(int l);
void AfficherIllustration (void);
 

#endif
