#include <SDL.h>
#include <stdio.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL_ttf.h>

#include "types.h"
#include "affichage.h"

SDL_Window *window;
SDL_Renderer *renderer;
int frame_limit;
TTF_Font *font;
SDL_Texture *texture_illustration;
SDL_Texture *textures_niveaux[5];
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
		
	SDL_Surface *image = NULL;
	texture_illustration = NULL;	
	if ((image = SDL_LoadBMP("res/ballons_vent.bmp")) == NULL)
		SDL_ExitWithError("Impossible de charger l'image");
	if ((texture_illustration = SDL_CreateTextureFromSurface(renderer, image)) == NULL)
		SDL_ExitWithError("Impossible de charger l'image");
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
	TTF_CloseFont(font);
	TTF_Quit();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

//------------Affichage de l'unite de base------------------------------

void AfficherTuile(int x, int y, int statut)
{
	Uint32 couleur;
	switch(statut)
	{
		case -1:
			couleur = MARRON;
			break;		
		case 0:
			couleur = VERT;
			break;
		case 1:
			couleur = JAUNE;
			break;
		case 2:
			couleur = ROUGE;
			break;
		case 3:
			couleur = GRIS;
			break;	
		case 99:
			couleur = TURQUOISE;
			break;	
		default:
			break;			
	}
	
	if (boxColor(renderer, x, y, x + TUILE, y + TUILE, couleur) < 0)
		SDL_ExitWithError("Impossible de tracer les box");
	if (rectangleColor(renderer, x, y, x + TUILE, y + TUILE, 0xFF000000) < 0)
		SDL_ExitWithError("Impossible de tracer les box");		
}

//------------Affichage des terrains------------------------------------

void AfficherLesTerrains(const Terrain *t, const Joueur *j)
{	
	if (t->suivant != NULL)
		AfficherLesTerrains(t->suivant, j);

	if (t == j->terrain_actif)
		IlluminerTerrain(t);
	
	AfficherUnTerrain(t);
	
	if (t != j->terrain_actif)
		AssombrirTerrain(t);
}

void AfficherUnTerrain(const Terrain *t)
{
	for (int i = 0; i < LARGEUR_TERRAIN + 2; i ++)
		for (int j = 0; j < HAUTEUR_TERRAIN; j++)
			AfficherTuile(t->pays->x + i * TUILE, t->pays->y + j * TUILE, t->matrice[i][j]);
	
	if (stringColor(renderer, TUILE + t->pays->x, TUILE / 2 + t->pays->y + TUILE * HAUTEUR_TERRAIN, t->pays->nom, BLANC) < 0)
		SDL_ExitWithError("Impossible d'afficher les noms de pays");
}

void AssombrirTerrain(const Terrain *t)
{
	if (boxColor(renderer, t->pays->x, t->pays->y, t->pays->x + (LARGEUR_TERRAIN + 2) * TUILE, t->pays->y + (1 + HAUTEUR_TERRAIN) * TUILE, 0xAA000000) < 0)
		SDL_ExitWithError("Impossible de tracer les box");
}

void IlluminerTerrain(const Terrain *t)
{
	if (boxColor(renderer, t->pays->x - TUILE, t->pays->y - TUILE, t->pays->x + (LARGEUR_TERRAIN + 3) * TUILE, t->pays->y + (HAUTEUR_TERRAIN + 1) * TUILE, 0x50C3E617) < 0)
		SDL_ExitWithError("Impossible de tracer les box");
}

//------------Affichage du joueur---------------------------------------

void AfficherJoueur(const Joueur *j)
{
	int x = j->terrain_actif->pays->x + j->x * TUILE;
	int y = j->terrain_actif->pays->y + j->y * TUILE;
	AfficherTuile(x, y, 99);
}



//------------Affichage des textes--------------------------------------

void ChargerTextes(int l)
{
	char *lien;
	if (l == 1)
		lien = "res/EN.txt";
	if (l == 2)
		lien = "res/ES.txt";
	if (l == 3)
		lien = "res/FR.txt";
	if (l == 4)
		lien = "res/JA.txt";
	
	TextesTraduits(lien);
	
	TextesNiveaux();
	
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

void TextesNiveaux(void)
{
	char niveau[4];
	for (int i = 0; i < 5; i++)
	{
		sprintf(niveau, "%d/6", i + 2);
		textures_niveaux[i] = TextureTexte(niveau, 0);
	}
}

void EcrireTexteProvisoire(char *texte, int X, int Y, int W, int H)
{	
	SDL_Texture *message = TextureTexte(texte, 0);	
	SDL_Rect MessageRect = {.x = X, .y = Y, .w = W, .h = H};
	SDL_RenderCopy(renderer, message, NULL, &MessageRect);
	SDL_DestroyTexture(message);
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
	SDL_RenderCopy(renderer, texture_textes[2], NULL, &message_rect);
	
	for (int i = 0; i < 5; i++)
	{
		SDL_Rect message_rect = {.x = 5, .y = HAUTEUR_FENETRE - 110 + i * 20, .w = longueur_texte[7 + i] * 8, .h = 20};
		SDL_RenderCopy(renderer, texture_textes[7 + i], NULL, &message_rect);
	}
}

void TextureLangages(void)
{
	char *lien = "res/LANG.txt";

	if (VerificationLangage(lien) == 1)
	{		
		FILE *lecture = fopen(lien, "r");
		char tampon[15];
		
		for (int i = 0; i < NOMBRE_LANGAGES; i ++)
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
		SDL_Rect message_rect = {.x = LARGEUR_FENETRE / 2 - longueur_langage[i * 2] * 8, .y = HAUTEUR_FENETRE / 2 + 50 + i * 30, .w = longueur_langage[i * 2] * 16, .h = 40};
		if (-l - 1 != i)
			SDL_RenderCopy(renderer, textures_langages[i * 2], NULL, &message_rect);
		else
			SDL_RenderCopy(renderer, textures_langages[i * 2 + 1], NULL, &message_rect);
	}
}

void AfficherIllustration (void)
{
	SDL_Rect rect_illu = {.w = 200, .h = 280};
	rect_illu.x = LARGEUR_FENETRE / 2 - 100; 
	rect_illu.y = HAUTEUR_FENETRE / 2 - 300;
	if (SDL_RenderCopy(renderer, texture_illustration, NULL, &rect_illu) != 0)
			SDL_ExitWithError("Impossible d'afficher la texture");
			
	hlineColor(renderer, LARGEUR_FENETRE / 2 - 200, LARGEUR_FENETRE / 2 + 200, HAUTEUR_FENETRE / 2 + 20, BLANC);
}





