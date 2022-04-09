#include <SDL.h>
#include <stdio.h>
#include <SDL_image.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL_ttf.h>

#include "types.h"
#include "affichage.h"

SDL_Window *window;
SDL_Renderer *renderer;
int frame_limit;
TTF_Font *font;
SDL_Texture *textures_images[NOMBRE_IMAGES];
SDL_Texture *textures_icones[NOMBRE_ICONES];
SDL_Texture *textures_tuiles[NOMBRE_TUILES];
SDL_Texture *textures_langages[NOMBRE_LANGAGES * 2];
int longueur_langage[NOMBRE_LANGAGES * 2];
SDL_Texture *texture_textes[LIGNES_TEXTE];
int longueur_texte[LIGNES_TEXTE];

//------------Gestion de la SDL-----------------------------------------

void SDL_ExitWithError(const char *message)
{
	SDL_Log("ERREUR : %s > %s\n", message, SDL_GetError());
	SDL_Quit();
	exit(EXIT_FAILURE);
}

void limit_fps(void)
{
	unsigned t = SDL_GetTicks();
	
	if (frame_limit < t)
	{
		frame_limit = SDL_GetTicks() + FPS;
		return;
	}
	else if (frame_limit > t + FPS)
		SDL_Delay(FPS);
	else
		SDL_Delay(frame_limit - t);
	frame_limit = SDL_GetTicks() + FPS;
}

void InitialisationAffichage(void)
{
	window = NULL;
	renderer = NULL;

	frame_limit = 0;
		
	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) != 0)
		SDL_ExitWithError("Initiation SDL");
		
	if (SDL_CreateWindowAndRenderer(LARGEUR_FENETRE, HAUTEUR_FENETRE, 0, &window, &renderer))
		SDL_ExitWithError("Impossible de creer la fenetre et le rendu");
	
	SDL_SetWindowTitle(window, TITRE);

	if (TTF_Init() < 0)
		SDL_ExitWithError("Initiation TTF");
		
	font = TTF_OpenFont("res/OtomanopeeOne-Regular.ttf", 100);
	if (!font)
		SDL_ExitWithError("Chargement de la police");
		
	int drapeau = IMG_INIT_PNG;
	int initialise=IMG_Init(drapeau);
	if((initialise&drapeau) != drapeau) 
		SDL_ExitWithError("Initiation SDL_image");
	
	ChargerImages("res/petite_fee.png", 0);
	ChargerImages("res/petit_gnome.png", 1);
	ChargerImages("res/fee.png", 2);
	ChargerImages("res/gnome.png", 3);
	ChargerImages("res/petit_gnome_pousse.png", 4);
	ChargerImages("res/lan.png", 5);
	ChargerImages("res/fee_titre.png", 6);
	ChargerImages("res/fee_finale.png", 7);
	ChargerImages("res/fee_gnome.png", 8);
	ChargerImages("res/yeux.png", 9);
	
	ChargerIcones("res/card.png", 0);
	ChargerIcones("res/batterie.png", 1);
	ChargerIcones("res/usb.png", 2);
	ChargerIcones("res/film.png", 3);
	ChargerIcones("res/light.png", 4);
	ChargerIcones("res/smartphone.png", 5);
	ChargerIcones("res/ecran.png", 6);
	
	ChargerTuiles("res/terrain_centre.png", 0);
	ChargerTuiles("res/terrain_bord.png", 1);
	ChargerTuiles("res/tuile_jaune.png", 2);
	ChargerTuiles("res/tuile_rouge.png", 3);
	ChargerTuiles("res/tuile_grise.png", 4);
	ChargerTuiles("res/tuile_sombre.png", 5);
}

void ChargerImages(char *l, int i)
{
	SDL_Surface *image = NULL;
	image = IMG_Load(l);
	textures_images[i] = NULL;
	if(!image)
		SDL_ExitWithError("Impossible de charger images");
	if ((textures_images[i] = SDL_CreateTextureFromSurface(renderer, image)) == NULL)
		SDL_ExitWithError("Impossible de charger images");
	SDL_FreeSurface(image);
}

void ChargerIcones(char *l, int i)
{
	SDL_Surface *image = NULL;
	image = IMG_Load(l);
	textures_icones[i] = NULL;
	if(!image)
		SDL_ExitWithError("Impossible de charger icones");
	if ((textures_icones[i] = SDL_CreateTextureFromSurface(renderer, image)) == NULL)
		SDL_ExitWithError("Impossible de charger icones");
	SDL_FreeSurface(image);
}

void ChargerTuiles(char *l, int i)
{
	SDL_Surface *image = NULL;
	image = IMG_Load(l);
	textures_tuiles[i] = NULL;
	if(!image)
		SDL_ExitWithError("Impossible de charger images");
	if ((textures_tuiles[i] = SDL_CreateTextureFromSurface(renderer, image)) == NULL)
		SDL_ExitWithError("Impossible de charger images");
	SDL_FreeSurface(image);
}

void AfficherLeRendu(void)
{
	SDL_RenderPresent(renderer);
			
	#ifndef __EMSCRIPTEN__
		limit_fps();
	#endif
	
	SDL_Rect fond_fenetre = {.x = 0, .y = 0, .w = LARGEUR_FENETRE, .h = HAUTEUR_FENETRE};
	SDL_RenderClear(renderer);					
	
	if (SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE) != 0)
		SDL_ExitWithError("Impossible de changer la couleur du rendu");
		
	if (SDL_RenderFillRect(renderer, &fond_fenetre) != 0)
		SDL_ExitWithError("Impossible de dessiner un rectangle");
}

void DestructionAffichage(void)
{
	
	for (int i = 0; i < NOMBRE_IMAGES; i++)
		SDL_DestroyTexture(textures_images[i]);
	for (int i = 0; i < NOMBRE_ICONES; i++)
		SDL_DestroyTexture(textures_icones[i]);
	for (int i = 0; i < NOMBRE_TUILES; i++)
		SDL_DestroyTexture(textures_tuiles[i]);
	for (int i = 0; i < NOMBRE_LANGAGES * 2; i++)
		SDL_DestroyTexture(textures_langages[i]);

	IMG_Quit();
	TTF_CloseFont(font);
	TTF_Quit();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

//------------Affichage de l'unite de base------------------------------

void AfficherTuile(int x, int y, int statut)
{
	int i = 1;
	switch(statut)
	{
		case 1:
			i = 2; // JAUNE
			break;
		case 2:
			i = 3; // ROUGE
			break;
		case 3:
			i = 4; // GRIS
			break;
		default:
			break;			
	}
	
	SDL_Rect rect = {.x = x, .y = y, .w = TUILE, .h = TUILE};
	if (SDL_RenderCopy(renderer, textures_tuiles[i], NULL, &rect) != 0)
		SDL_ExitWithError("Impossible d'afficher la texture");
}

void AfficherAchat(int x, int y, int statut)
{
	SDL_Rect rect = {x, y, TUILE, TUILE};
	if (SDL_RenderCopy(renderer, textures_icones[statut - 1], NULL, &rect) != 0)
		SDL_ExitWithError("Impossible d'afficher la texture");	
}

void AfficherTir(int x, int y)
{
	if (filledCircleColor(renderer, x, y, 5, ROSE) < 0)
		SDL_ExitWithError("Impossible d'afficher les tirs");
	if (circleColor(renderer, x, y, 5, ROUGE) < 0)
		SDL_ExitWithError("Impossible d'afficher les tirs");
}

//------------Affichage des terrains------------------------------------

void AfficherLesTerrainsAvecJoueur(const Terrain *t, const Joueur *j)
{	
	if (t->suivant != NULL)
		AfficherLesTerrainsAvecJoueur(t->suivant, j);

	if (t == j->terrain_actif)
		InfosTerrainActif(t, j);
	
	AfficherUnTerrain(t);
	
	if (t != j->terrain_actif)
		AssombrirTerrain(t);
}

void AfficherLesTerrainsSansJoueur(const Terrain *t)
{
	if (t->suivant != NULL)
		AfficherLesTerrainsSansJoueur(t->suivant);
	
	AfficherUnTerrain(t);
}

void AfficherUnTerrain(const Terrain *t)
{
	SDL_Rect rect = {.x = t->pays->x, .y = t->pays->y, .w = 400, .h = 100};
	if (SDL_RenderCopy(renderer, textures_tuiles[0], NULL, &rect) != 0)
		SDL_ExitWithError("Impossible d'afficher la texture");
		
	SDL_Rect rectb1 = {.x = t->pays->x - TUILE, .y = t->pays->y, .w = TUILE, .h = 100};
	if (SDL_RenderCopy(renderer, textures_tuiles[1], NULL, &rectb1) != 0)
		SDL_ExitWithError("Impossible d'afficher la texture");
		
	SDL_Rect rectb2 = {.x = t->pays->x + 400, .y = t->pays->y, .w = TUILE, .h = 100};
	if (SDL_RenderCopy(renderer, textures_tuiles[1], NULL, &rectb2) != 0)
		SDL_ExitWithError("Impossible d'afficher la texture");
	
	for (int i = 0; i < LARGEUR_TERRAIN; i++)
		for (int j = 0; j < HAUTEUR_TERRAIN; j++)
			if (t->matrice[i][j] > 0)
				AfficherTuile(t->pays->x + i * TUILE, t->pays->y + j * TUILE, t->matrice[i][j]);
	
	SDL_Rect message_rect = {.x = t->pays->x, .y = t->pays->y + TUILE * HAUTEUR_TERRAIN, .w = longueur_texte[t->pays->numero] * 7, .h = TUILE};
	if (SDL_RenderCopy(renderer, texture_textes[t->pays->numero], NULL, &message_rect) < 0)
		SDL_ExitWithError("Impossible d'écrire ligne e_waste");
		
	if (stringColor(renderer, t->pays->x + TUILE * (LARGEUR_TERRAIN + 1), t->pays->y + TUILE * 2.2 , " >", BLANC) < 0)
			SDL_ExitWithError("Impossible d'écrire ligne e_waste");
	
	if (t->suivant == NULL)
		if (stringColor (renderer, t->pays->x + TUILE * (LARGEUR_TERRAIN + 2), t->pays->y + TUILE * 2.2 , ">>>", BLANC) < 0)
			SDL_ExitWithError("Impossible d'écrire ligne e_waste");
}

void AssombrirTerrain(const Terrain *t)
{
	SDL_Rect rect = {.x = t->pays->x - TUILE, .y = t->pays->y, .w = 440, .h = 100};
	if (SDL_RenderCopy(renderer, textures_tuiles[5], NULL, &rect) != 0)
		SDL_ExitWithError("Impossible d'afficher la texture");
}

void InfosTerrainActif(const Terrain *t, const Joueur *j)
{
	char ratio[10];
	int long_texte_pays = t->pays->x + (longueur_texte[t->pays->numero] + 1) * 7;
	int long_pourcentage = 7 * 6;
	
	sprintf(ratio, " (%d %%)", j->terrain_actif->pays->ratio_lineaire);
	EcrireTexteProvisoire(ratio, 
							long_texte_pays, 
							t->pays->y + TUILE * HAUTEUR_TERRAIN, 
							long_pourcentage, 
							TUILE);
	
	for (int i = 0; i < t->recyclable; i++)

		AfficherTir((i + 1) * 6 + + long_texte_pays + long_pourcentage,
					t->pays->y + TUILE * (HAUTEUR_TERRAIN + 0.5));
}

void ApparitionColonneDechets(const Terrain *t, const int i)
{
	for (int x = 0; x < i; x++)
		for (int j = 0; j < HAUTEUR_TERRAIN; j++)
			ApparitionDUnDechet(t, x, j);	
}

void ApparitionDUnDechet(const Terrain *t, const int i, const int j)
{
	if (t->matrice_dechets[i][j] > 0)
	{
		AfficherAchat(t->pays->x + i * TUILE, t->pays->y + j * TUILE, t->matrice_dechets[i][j]);
	}
	
	if (t->suivant != NULL)
		ApparitionDUnDechet(t->suivant, i, j);
}

//------------Affichage du joueur---------------------------------------

void AfficherJoueur(const Joueur *j)
{
	int x = j->terrain_actif->pays->x + j->x * TUILE;
	int y = j->terrain_actif->pays->y + j->y * TUILE;
	
	SDL_Rect rect_illu = {x, y, TUILE, TUILE};
	if (SDL_RenderCopy(renderer, textures_images[1], NULL, &rect_illu) != 0)
		SDL_ExitWithError("Impossible d'afficher la texture");
}



//------------Affichage des textes--------------------------------------

void ChargerTextes(int l)
{
	char *lien;
	if (l == 1)
		lien = "res/EN.txt";
	if (l == 2)
		lien = "res/FR.txt";
	if (l == 3)
		lien = "res/JA.txt";
	
	TextesTraduits(lien);	
}

int VerificationLangage(const char *s)
{
	FILE *fichier;
	if ((fichier = fopen(s, "r")))
	{
		fclose(fichier);
		return 1;
	}
	else
		return 0;
}

SDL_Texture *TextureTexte(char *texte, int i)
{
	SDL_Color couleur;
	if (i == 0) // blanc
	{
		couleur.r = 250; couleur.g = 250; couleur.b = 250; couleur.a = 0;
	}
	if (i == 1) // jaune
	{
		couleur.r = 250; couleur.g = 250; couleur.b = 0; couleur.a = 0;
	}
										
	SDL_Surface *surface_message = TTF_RenderUTF8_Solid(font, texte, couleur);
	SDL_Texture *message = SDL_CreateTextureFromSurface(renderer, surface_message);
	SDL_FreeSurface(surface_message);
	
	return message;
}

void TextesTraduits(char *l)
{
	if (VerificationLangage(l) == 1)
	{		
		FILE *lecture = fopen(l, "r");
		char tampon[150];
		
		for (int i = 0; i < LIGNES_TEXTE; i++)
		{
			fgets(tampon, sizeof(tampon), lecture);
			tampon[strcspn(tampon, "\n")] = 0;
			longueur_texte[i] = strlen(tampon);
			texture_textes[i] = TextureTexte(tampon, 0);
		}		
		fclose(lecture);
	}
	else
	{
		printf("Fichier linguistique non trouvé");
		exit(EXIT_FAILURE);
	}
}

void DetruireTextes(void)
{
	for (int i = 0; i < LIGNES_TEXTE; i++)
		SDL_DestroyTexture(texture_textes[i]);
}

void EcrireTexteProvisoire(char *texte, int X, int Y, int W, int H)
{	
	SDL_Texture *message = TextureTexte(texte, 0);	
	SDL_Rect MessageRect = {.x = X, .y = Y, .w = W, .h = H};
	SDL_RenderCopy(renderer, message, NULL, &MessageRect);
	SDL_DestroyTexture(message);
}

void AfficherInfosJoueur(Joueur *j)
{
	SDL_Rect rect = {.x = 0, .y = 0, .w = LARGEUR_FENETRE, .h = 60};
	if (SDL_RenderCopy(renderer, textures_tuiles[4], NULL, &rect) != 0)
		SDL_ExitWithError("Impossible d'afficher la texture");
	
	char annee[5];
	sprintf(annee, "%d", j->annee);
	EcrireTexteProvisoire(annee, TUILE, 0, 80, 50);
		
}

void AfficherInfosFee(Joueur *j)
{
	SDL_Rect rect = {.x = 0, .y = 0, .w = LARGEUR_FENETRE, .h = 60};
	if (SDL_RenderCopy(renderer, textures_tuiles[4], NULL, &rect) != 0)
		SDL_ExitWithError("Impossible d'afficher la texture");
	
	char annee[5];
	sprintf(annee, "%d", j->annee + 1);
	EcrireTexteProvisoire(annee, TUILE, 0, 80, 50);		
}

void AfficherBravo(void)
{
	SDL_Rect message_rect = {.x = LARGEUR_FENETRE / 2 - 75, .y = HAUTEUR_FENETRE / 2 - 75, .w = 150, .h = 100};
	SDL_RenderCopy(renderer, texture_textes[0], NULL, &message_rect);
}

void AfficherIncorrect(void)
{
	SDL_Rect message_rect = {.x = LARGEUR_FENETRE / 2 - 75, .y = HAUTEUR_FENETRE / 2 - 75, .w = 150, .h = 100};
	SDL_RenderCopy(renderer, texture_textes[1], NULL, &message_rect);
}

void AfficherPause(void)
{
	SDL_Rect message_rect = {.x = LARGEUR_FENETRE / 2 - 50, .y = HAUTEUR_FENETRE / 2 - 50, .w = 150, .h = 100};
	SDL_RenderCopy(renderer, texture_textes[31], NULL, &message_rect);
}

void TextureLangages(void)
{
	char *lien = "res/LANG.txt";

	if (VerificationLangage(lien) == 1)
	{		
		FILE *lecture = fopen(lien, "r");
		char tampon[15];
		
		for (int i = 0; i < NOMBRE_LANGAGES; i++)
		{
			fgets(tampon, sizeof(tampon), lecture);
			tampon[strcspn(tampon, "\n")] = 0;
			longueur_langage[i * 2] = strlen(tampon);
			longueur_langage[i * 2 + 1] = longueur_texte[i * 2];
			textures_langages[i * 2] = TextureTexte(tampon, 0);
			textures_langages[i * 2 + 1] = TextureTexte(tampon, 1);
		}		
		fclose(lecture);
	}
	else
	{
		printf("Fichier linguistique non trouvé");
		exit(EXIT_FAILURE);
	}
}


void AfficherLangage(int l)
{
	for (int i = 0; i < NOMBRE_LANGAGES; i ++)
	{
		SDL_Rect message_rect = {.x = LARGEUR_FENETRE / 2 - longueur_langage[i * 2] * 7, .y = HAUTEUR_FENETRE / 2 + 50 + i * 40, .w = longueur_langage[i * 2] * 14, .h = 40};
		if (-l - 1 != i)
			SDL_RenderCopy(renderer, textures_langages[i * 2], NULL, &message_rect);
		else
			SDL_RenderCopy(renderer, textures_langages[i * 2 + 1], NULL, &message_rect);
	}
}

void AfficherTitre(void)
{
	if (boxColor(renderer, (LARGEUR_FENETRE - longueur_texte[32] * 31) / 2, 110, (LARGEUR_FENETRE + longueur_texte[32] * 31) / 2, 180, NOIR) < 0)
		SDL_ExitWithError("Impossible de dessiner le fond de l'intro'");
	
	SDL_Rect message_rect = {.x = (LARGEUR_FENETRE - longueur_texte[32] * 30) / 2, .y = 100, .w = longueur_texte[32] * 30, .h = 80};
	if (SDL_RenderCopy(renderer, texture_textes[32], NULL, &message_rect) < 0)
		SDL_ExitWithError("Impossible d'écrire l'intro");
	
	SDL_Rect rect_illu = {LARGEUR_FENETRE / 2 - 200, 0, 400, 110};
	if (SDL_RenderCopy(renderer, textures_images[6], NULL, &rect_illu) != 0)
		SDL_ExitWithError("Impossible d'afficher la texture");
}

void AfficherMenu(Partie *p)
{
	AfficherLan();
	
	int align_gauche = (LARGEUR_FENETRE - TUILE * ((longueur_texte[33] + longueur_texte[34] + longueur_texte[35]) / 2 + 6)) / 2;
	int x_gnomette = align_gauche - 8 * TUILE;
	
	Uint32 couleur[3] = {GRIS, GRIS_FONCE, GRIS_FONCE};
	if (p->menu == 1)
	{
		couleur[0] = GRIS_FONCE; couleur[1] = GRIS;
		x_gnomette += (longueur_texte[33] / 2 + 3) * TUILE;
	}
	if (p->menu == 2)
	{
		couleur[0] = GRIS_FONCE; couleur[2] = GRIS;
		x_gnomette += ((longueur_texte[33] + longueur_texte[34]) / 2 + 6) * TUILE;
	}
	
	if ((p->debloque == 0 && p->menu == 1) ||
			(p->debloque == 0 && p->menu == 2) ||
			(p->debloque == 1 && p->menu == 1))
		AfficherPousser(x_gnomette, HAUTEUR_FENETRE / 2 - 4 * TUILE);
	else			
		AfficherPetitGnome(x_gnomette, HAUTEUR_FENETRE / 2 - 4 * TUILE);
	
	SDL_Rect message_rect = {.y = HAUTEUR_FENETRE / 2, .h = 40};
	
	message_rect.x = align_gauche; message_rect.w = longueur_texte[33] / 2 * TUILE;
	if (roundedBoxColor(renderer, align_gauche - TUILE,
				HAUTEUR_FENETRE / 2 - TUILE,
				align_gauche + longueur_texte[33] / 2 * TUILE + TUILE,
				HAUTEUR_FENETRE / 2 + 40 + TUILE, TUILE, couleur[0]) < 0)
		SDL_ExitWithError("Impossible de dessiner boite menu");
	SDL_RenderCopy(renderer, texture_textes[33], NULL, &message_rect);
	
	message_rect.x = align_gauche + (longueur_texte[33] / 2 + 3) * TUILE; message_rect.w = longueur_texte[34] / 2 * TUILE;
	if (roundedBoxColor(renderer, align_gauche + (longueur_texte[33] / 2 + 2) * TUILE,
				HAUTEUR_FENETRE / 2 - TUILE,
				align_gauche + ((longueur_texte[33] + longueur_texte[34]) / 2 + 4) * TUILE,
				HAUTEUR_FENETRE / 2 + 40 + TUILE, TUILE, couleur[1]) < 0)
		SDL_ExitWithError("Impossible de dessiner boite menu");
	SDL_RenderCopy(renderer, texture_textes[34], NULL, &message_rect);
	if (p->debloque < 2)
		if (roundedBoxColor(renderer, align_gauche + (longueur_texte[33] / 2 + 2) * TUILE,
				HAUTEUR_FENETRE / 2 - TUILE,
				align_gauche + ((longueur_texte[33] + longueur_texte[34]) / 2 + 4) * TUILE,
				HAUTEUR_FENETRE / 2 + 40 + TUILE, TUILE, SOMBRE) < 0)
			SDL_ExitWithError("Impossible de dessiner boite menu");
	
	message_rect.x = align_gauche + ((longueur_texte[33] + longueur_texte[34]) / 2 + 6) * TUILE; message_rect.w = longueur_texte[35] / 2 * TUILE;
	if (roundedBoxColor(renderer, align_gauche + ((longueur_texte[33] + longueur_texte[34]) / 2 + 5) * TUILE,
				HAUTEUR_FENETRE / 2 - TUILE,
				align_gauche + ((longueur_texte[33] + longueur_texte[34] + longueur_texte[35]) / 2 + 7) * TUILE,
				HAUTEUR_FENETRE / 2 + 40 + TUILE, TUILE, couleur[2]) < 0)
		SDL_ExitWithError("Impossible de dessiner boite menu");
	SDL_RenderCopy(renderer, texture_textes[35], NULL, &message_rect);
	if (p->debloque < 1)
		if (roundedBoxColor(renderer, align_gauche + ((longueur_texte[33] + longueur_texte[34]) / 2 + 5) * TUILE,
				HAUTEUR_FENETRE / 2 - TUILE,
				align_gauche + ((longueur_texte[33] + longueur_texte[34] + longueur_texte[35]) / 2 + 7) * TUILE,
				HAUTEUR_FENETRE / 2 + 40 + TUILE, TUILE, SOMBRE) < 0)
			SDL_ExitWithError("Impossible de dessiner boite menu");
	
	
}

void AfficherTexteIntro(int ligne, int x, int y)
{
	SDL_Rect message_rect = {.x = x, .y = y, .w = longueur_texte[ligne] * 10, .h = 40};
	if (SDL_RenderCopy(renderer, texture_textes[ligne], NULL, &message_rect) < 0)
		SDL_ExitWithError("Impossible d'écrire l'intro");
}

void AfficherDebloque(int ligne, int x, int y)
{
	if (roundedBoxColor(renderer, x - TUILE / 2, y - TUILE / 2, x + longueur_texte[ligne] * 8 + TUILE / 2, y + 25 + TUILE / 2, TUILE / 4, BLEU) < 0)
		SDL_ExitWithError("Impossible d'écrire message debloque");
	
	SDL_Rect message_rect = {.x = x, .y = y, .w = longueur_texte[ligne] * 8, .h = 25};
	if (SDL_RenderCopy(renderer, texture_textes[ligne], NULL, &message_rect) < 0)
		SDL_ExitWithError("Impossible d'écrire message debloque");
}

//------------Affichage des images--------------------------------------

void AfficherFondGris(void)
{
	if (boxColor(renderer, 0, 0, LARGEUR_FENETRE, HAUTEUR_FENETRE, GRIS) < 0)
		SDL_ExitWithError("Impossible de dessiner le fond de l'intro'");
}

void AfficherPetiteFee(int position_x)
{
	SDL_Rect rect_illu = {.w = 2*TUILE, .h = 3*TUILE};
	rect_illu.x = 8 * TUILE + position_x * TUILE; 
	rect_illu.y = 0;
	if (SDL_RenderCopy(renderer, textures_images[0], NULL, &rect_illu) != 0)
		SDL_ExitWithError("Impossible d'afficher la texture");
	SDL_Rect message_rect = {.x = rect_illu.x + 3 * TUILE, .y = TUILE / 2, .w = longueur_texte[29] * 8, .h = 30};
	if (SDL_RenderCopy(renderer, texture_textes[29], NULL, &message_rect) < 0)
		SDL_ExitWithError("Impossible d'écrire injonction fée");
}

void AfficherPetitGnome(int position_x, int position_y)
{
	SDL_Rect rect_illu = {.w = 2*TUILE, .h = 2.5*TUILE};
	rect_illu.x = 8 * TUILE + position_x; 
	rect_illu.y = 0 + position_y;
	if (SDL_RenderCopy(renderer, textures_images[1], NULL, &rect_illu) != 0)
		SDL_ExitWithError("Impossible d'afficher la texture");
}

void AfficherPousser(int position_x, int position_y)
{
	SDL_Rect rect_illu = {.w = 2*TUILE, .h = 2.5*TUILE};
	rect_illu.x = 8 * TUILE + position_x; 
	rect_illu.y = 0 + position_y;
	if (SDL_RenderCopy(renderer, textures_images[4], NULL, &rect_illu) != 0)
		SDL_ExitWithError("Impossible d'afficher la texture");
}

void AfficherGrandeFee(int x, int y)
{
	SDL_Rect rect_illu = {.w = 477, .h = 700};
	rect_illu.x = x; 
	rect_illu.y = y;
	if (SDL_RenderCopy(renderer, textures_images[2], NULL, &rect_illu) != 0)
			SDL_ExitWithError("Impossible d'afficher la texture");
}

void AfficherGrandeFeeFinale(int x, int y)
{
	SDL_Rect rect_illu = {.w = 477, .h = 700};
	rect_illu.x = x; 
	rect_illu.y = y;
	if (SDL_RenderCopy(renderer, textures_images[7], NULL, &rect_illu) != 0)
			SDL_ExitWithError("Impossible d'afficher la texture");
}

void AfficherGrandGnome(int x, int y)
{
	SDL_Rect rect_illu = {.w = 349, .h = 500};
	rect_illu.x = x; 
	rect_illu.y = y;
	if (SDL_RenderCopy(renderer, textures_images[3], NULL, &rect_illu) != 0)
		SDL_ExitWithError("Impossible d'afficher la texture");
}

void AfficherFeeGnome(int x, int y)
{
	SDL_Rect rect_illu = {.w = 500, .h = 500};
	rect_illu.x = x; 
	rect_illu.y = y;
	if (SDL_RenderCopy(renderer, textures_images[8], NULL, &rect_illu) != 0)
		SDL_ExitWithError("Impossible d'afficher la texture");
}

void AfficherLan(void)
{
	SDL_Rect rect_illu = {0, 0, LARGEUR_FENETRE, HAUTEUR_FENETRE};
	if (SDL_RenderCopy(renderer, textures_images[5], NULL, &rect_illu) != 0)
		SDL_ExitWithError("Impossible d'afficher la texture");
}

void AfficherYeux(void)
{
	SDL_Rect rect_illu = {LARGEUR_FENETRE / 2 - 150, 300, 300, 60};
	if (SDL_RenderCopy(renderer, textures_images[9], NULL, &rect_illu) != 0)
		SDL_ExitWithError("Impossible d'afficher la texture");
}





