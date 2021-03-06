#ifndef __AFFICHAGE_H__
#define __AFFICHAGE_H__

#define NOIR 0xFF000000
#define SOMBRE 0x90000000
#define GRIS 0xFF4D4D4D
#define GRIS_FONCE 0xFF202020
#define ROUGE 0xFF0000FF
#define VERT 0xFF008000
#define BLEU 0xFF844300
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
void AfficherChargement(void);
void ChargerLesImages(void);
void ChargerLesIcones(void);
void ChargerLesTuiles(void);
void ChargerImage(char *l, int i);
void ChargerIcone(char *l, int i);
void ChargerTuile(char *l, int i);

//------------Affichage des unités de base------------------------------

void AfficherTuile(const int x, const int y, const int statut);
void AfficherAchat(int x, int y, int statut);
void AfficherTir(int x, int y);

//------------Affichage des terrains------------------------------------
void ChargerTextes(int l);
void AfficherLesTerrainsAvecJoueur(const Terrain *t);
void AfficherLesTerrainsSansJoueur(const Terrain *t);
void AfficherUnTerrain(const Terrain *t);
void AssombrirTerrain(const Terrain *t);
void InfosTerrainActif(const Terrain *t);
void ApparitionColonneDechets(const Terrain *t, const int i);
void ApparitionDUnDechet(const Terrain *t, const int i, const int j);

//------------Affichage du joueur---------------------------------------
void AfficherJoueur(void);

//------------Affichage des textes--------------------------------------
int VerificationLangage(const char *s);
SDL_Texture *TextureTexte(char *texte, int i);
void TextesTraduits(char *l);
void DetruireTextes(void);
void EcrireTexteProvisoire(char *texte, int X, int Y, int W, int H);
void AfficherAnnee(int ajouter);
void AfficherOptions(void);
void AfficherMenu(int index_menu, int nombre_de_choix, int ligne_texte, int x, int largeur, int y);
void TextureLangages(void);
void AfficherLangage(int l);
void AfficherTitre(void);
void AfficherTexteIntro(int ligne, int x, int y);
void AfficherDebloque(int ligne, int x, int y);
void TexteTuto(int ligne);
void AfficherTuto(void);
void AfficherCredits(void);
void AfficherPerformances(Terrain *terrain);
void AfficherCarteAction(int nombre);

//------------Affichage des images--------------------------------------
void AfficherFondGris(void);
void AfficherPetiteFee(int position_x);
void AfficherPetitGnome(int position_x, int position_y);
void AfficherPousser(int position_x, int position_y);
void AfficherGrandeFee(int x, int y);
void AfficherGrandeFeeFinale(int x, int y);
void AfficherGrandGnome(int x, int y);
void AfficherFeeGnome(int x, int y);
void AfficherLan(void);
void AfficherYeux(void);
void AfficherCiel(Terrain *t);
 

#endif
