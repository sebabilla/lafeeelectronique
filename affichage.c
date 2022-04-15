#include <SDL.h>
#include <stdio.h>
#include <time.h>
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
		
	font = TTF_OpenFont("res/mplus-2p-heavy.ttf", 100);
	if (!font)
		SDL_ExitWithError("Chargement de la police");
		
	int drapeau = IMG_INIT_PNG;
	int initialise=IMG_Init(drapeau);
	if((initialise&drapeau) != drapeau) 
		SDL_ExitWithError("Initiation SDL_image");
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

void AfficherChargement(void)
{
	char chargement[30];
	sprintf(chargement, "Loading ... %d / 6 ...", partie.chargement);
	if (stringColor(renderer, 50, HAUTEUR_FENETRE - 20, chargement, BLANC) < 0)
		SDL_ExitWithError("Impossible de charger le jeu");
}

void ChargerLesImages(void)
{
	ChargerImage("res/petite_fee.png", 0);
	ChargerImage("res/petit_gnome.png", 1);
	ChargerImage("res/fee.png", 2);
	ChargerImage("res/gnome.png", 3);
	ChargerImage("res/petit_gnome_pousse.png", 4);
	ChargerImage("res/lan.png", 5);
	ChargerImage("res/fee_titre.png", 6);
	ChargerImage("res/fee_finale.png", 7);
	ChargerImage("res/fee_gnome.png", 8);
	ChargerImage("res/yeux.png", 9);
	ChargerImage("res/petit_gnome2.png", 10);
	ChargerImage("res/love.png", 11);
	ChargerImage("res/resistencia.png", 12);
}

void ChargerLesIcones(void)
{
	ChargerIcone("res/card.png", 0);
	ChargerIcone("res/batterie.png", 1);
	ChargerIcone("res/usb.png", 2);
	ChargerIcone("res/film.png", 3);
	ChargerIcone("res/souris.png", 4);
	ChargerIcone("res/smartphone.png", 5);
	ChargerIcone("res/ecran.png", 6);
}

void ChargerLesTuiles(void)
{
	ChargerTuile("res/terrain_centre.png", 0);
	ChargerTuile("res/terrain_bord.png", 1);
	ChargerTuile("res/tuile_jaune.png", 2);
	ChargerTuile("res/tuile_rouge.png", 3);
	ChargerTuile("res/tuile_grise.png", 4);
	ChargerTuile("res/assombrir.png", 5);
	ChargerTuile("res/fond_gris.png", 6);
	ChargerTuile("res/fond_bleu.png", 7);
	ChargerTuile("res/pouvoir_recycler.png", 8);
	ChargerTuile("res/eclaircir.png", 9);
	ChargerTuile("res/fond_bleu_fonce.png", 10);
	ChargerTuile("res/fond_bleu_tres_fonce.png", 11);
	ChargerTuile("res/fond_bleu_tres_tres_fonce.png", 12);
}

void ChargerImage(char *l, int i)
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

void ChargerIcone(char *l, int i)
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

void ChargerTuile(char *l, int i)
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

//------------Affichage des unités de base------------------------------

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
	SDL_Rect rect = {.x = x, .y = y, .w = TUILE, .h = TUILE};
	if (SDL_RenderCopy(renderer, textures_tuiles[9], NULL, &rect) != 0)
		SDL_ExitWithError("Impossible d'afficher la texture");
	
	if (SDL_RenderCopy(renderer, textures_icones[statut - 1], NULL, &rect) != 0)
		SDL_ExitWithError("Impossible d'afficher la texture");	
}

void AfficherTir(int x, int y)
{
	SDL_Rect rect = {.x = x, .y = y, .w = 10, .h = 10};
	if (SDL_RenderCopy(renderer, textures_tuiles[8], NULL, &rect) != 0)
		SDL_ExitWithError("Impossible d'afficher la texture");
}

//------------Affichage des terrains------------------------------------

void AfficherLesTerrainsAvecJoueur(const Terrain *t)
{		
	if (t->suivant != NULL)
		AfficherLesTerrainsAvecJoueur(t->suivant);

	if (t == joueur.terrain_actif)
		InfosTerrainActif(t);
	
	AfficherUnTerrain(t);
	
	if (t != joueur.terrain_actif)
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
	SDL_Rect rectb1 = {.x = t->pays->x - TUILE, .y = t->pays->y, .w = 40, .h = 100};
	if (SDL_RenderCopy(renderer, textures_tuiles[1], NULL, &rectb1) != 0)
		SDL_ExitWithError("Impossible d'afficher la texture");
		
	SDL_Rect rectb2 = {.x = t->pays->x + 380, .y = t->pays->y, .w = 40, .h = 100};
	if (SDL_RenderCopy(renderer, textures_tuiles[1], NULL, &rectb2) != 0)
		SDL_ExitWithError("Impossible d'afficher la texture");
	
	SDL_Rect rect = {.x = t->pays->x, .y = t->pays->y, .w = 400, .h = 100};
	if (SDL_RenderCopy(renderer, textures_tuiles[0], NULL, &rect) != 0)
		SDL_ExitWithError("Impossible d'afficher la texture");
	
	for (int i = 0; i < LARGEUR_TERRAIN; i++)
		for (int j = 0; j < HAUTEUR_TERRAIN; j++)
			if (t->matrice[i][j] > 0)
				AfficherTuile(t->pays->x + i * TUILE, t->pays->y + j * TUILE, t->matrice[i][j]);
	
	int caractere = 7; 
	if (partie.langage == 3)
		caractere = 5;
	
	SDL_Rect message_rect = {.x = t->pays->x - TUILE, .y = t->pays->y + TUILE * HAUTEUR_TERRAIN, .w = longueur_texte[t->pays->numero] * caractere, .h = TUILE};
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
	SDL_Rect rect = {.x = t->pays->x, .y = t->pays->y, .w = 400, .h = 100};
	if (SDL_RenderCopy(renderer, textures_tuiles[5], NULL, &rect) != 0)
		SDL_ExitWithError("Impossible d'afficher la texture");
}

void InfosTerrainActif(const Terrain *t)
{
	int caractere = 7; 
	if (partie.langage == 3)
		caractere = 5;
	
	char ratio[10];
	int long_texte_pays = t->pays->x - TUILE + (longueur_texte[t->pays->numero] + 1) * caractere;
	int long_pourcentage = caractere * 6;
	
	sprintf(ratio, " (%d %%)", (joueur.terrain_actif->pays->ratio_lineaire + joueur.bonus));
	EcrireTexteProvisoire(ratio, 
							long_texte_pays, 
							t->pays->y + TUILE * HAUTEUR_TERRAIN, 
							long_pourcentage, 
							TUILE);
	
	for (int i = 0; i < t->recyclable; i++)

		AfficherTir((i + 1) * 5 + long_texte_pays + long_pourcentage,
					t->pays->y + TUILE * (HAUTEUR_TERRAIN + 0.25));
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

void AfficherJoueur(void)
{
	int x = joueur.terrain_actif->pays->x + joueur.x * TUILE;
	int y = joueur.terrain_actif->pays->y + joueur.y * TUILE;
	
	SDL_Rect rect = {.x = x, .y = y, .w = TUILE, .h = TUILE};
	if (SDL_RenderCopy(renderer, textures_tuiles[5], NULL, &rect) != 0)
		SDL_ExitWithError("Impossible d'afficher la texture");
	if (SDL_RenderCopy(renderer, textures_images[1], NULL, &rect) != 0)
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

void AfficherAnnee(int ajouter)
{
	SDL_Rect rect = {.x = 0, .y = 0, .w = LARGEUR_FENETRE, .h = 60};
	if (SDL_RenderCopy(renderer, textures_tuiles[6], NULL, &rect) != 0)
		SDL_ExitWithError("Impossible d'afficher la texture");
	
	char annee[5];
	sprintf(annee, "%d", joueur.annee + ajouter);
	EcrireTexteProvisoire(annee, TUILE, 0, 80, 50);
		
}

void AfficherOptions(void)
{
	int caractere = 20;
	if (partie.langage == 3)
		caractere = 12;
	
	SDL_Rect rect = {0, 0, LARGEUR_FENETRE, HAUTEUR_FENETRE};
	if (SDL_RenderCopy(renderer, textures_tuiles[5], NULL, &rect) != 0)
		SDL_ExitWithError("Impossible d'afficher la texture");
	
	SDL_Rect message_rect = {.x = 150, 
							.y = 100, 
							.w = longueur_texte[31] * caractere, 
							.h = 50};
	SDL_RenderCopy(renderer, texture_textes[31], NULL, &message_rect);
	
	AfficherMenu(1, 5, 59, 250, 300, 200);
	
}

void AfficherMenu(int index_menu, int nombre_de_choix, int ligne_texte, int x_milieu, int largeur, int y)
{
	SDL_Rect message_rect = {.h = 30};
	int caractere = 10;
	if (partie.langage == 3)
		caractere = 7;
	
	SDL_Rect rect = {.x = x_milieu - largeur / 2, .h = 50, .w = largeur};
	
	int couleur[nombre_de_choix];
	for (int i = 0; i < nombre_de_choix; i++)
		couleur[i] = 12;
	couleur[partie.menu[index_menu]] = 10;
	
	for (int i = 0; i < nombre_de_choix; i++)
	{
		rect.y = y - 10 + i * 60;
		if (SDL_RenderCopy(renderer, textures_tuiles[couleur[i]], NULL, &rect) != 0)
			SDL_ExitWithError("Impossible d'afficher la texture");
			
			
		message_rect.x = x_milieu - longueur_texte[ligne_texte + i] * caractere /2;
		message_rect.y = y + i * 60; 
		message_rect.w = longueur_texte[ligne_texte + i] * caractere;		
		SDL_RenderCopy(renderer, texture_textes[ligne_texte + i], NULL, &message_rect);	
	
		if (index_menu == 0)
		{
			if (partie.debloque[i] == 0)
		{
			if (SDL_RenderCopy(renderer, textures_tuiles[5], NULL, &rect) != 0)
				SDL_ExitWithError("Impossible d'afficher la texture");
			if (partie.menu[0] == i)
				AfficherPousser(rect.x - 2 * TUILE, rect.y);
		}
		else			
			if (partie.menu[0] == i)
				AfficherPetitGnome(rect.x - 2 * TUILE, rect.y);
		}
		
		if (index_menu == 1)
		{		
			if (joueur.pouvoir[i] == 0)
			{
				if (SDL_RenderCopy(renderer, textures_tuiles[5], NULL, &rect) != 0)
					SDL_ExitWithError("Impossible d'afficher la texture");
				if (partie.menu[1] == i)
					AfficherPousser(rect.x - 2 * TUILE, rect.y);
			}
			else
				if (partie.menu[1] == i)
					AfficherPetitGnome(rect.x - 2 * TUILE, rect.y);
		}
	}
		
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
	AfficherLan();
	
	if (boxColor(renderer, (LARGEUR_FENETRE - longueur_texte[32] * 31) / 2, 110, (LARGEUR_FENETRE + longueur_texte[32] * 31) / 2, 180, NOIR) < 0)
		SDL_ExitWithError("Impossible de dessiner le fond de l'intro'");
	
	SDL_Rect message_rect = {.x = (LARGEUR_FENETRE - longueur_texte[32] * 30) / 2, .y = 100, .w = longueur_texte[32] * 30, .h = 80};
	if (SDL_RenderCopy(renderer, texture_textes[32], NULL, &message_rect) < 0)
		SDL_ExitWithError("Impossible d'écrire l'intro");
	
	SDL_Rect rect_illu = {LARGEUR_FENETRE / 2 - 200, 0, 400, 110};
	if (SDL_RenderCopy(renderer, textures_images[6], NULL, &rect_illu) != 0)
		SDL_ExitWithError("Impossible d'afficher la texture");
}

void AfficherTexteIntro(int ligne, int x, int y)
{
	SDL_Rect message_rect = {.x = x, .y = y, .w = longueur_texte[ligne] * 10, .h = 40};
	if (SDL_RenderCopy(renderer, texture_textes[ligne], NULL, &message_rect) < 0)
		SDL_ExitWithError("Impossible d'écrire l'intro");
}

void AfficherDebloque(int ligne, int x, int y)
{
	int caractere = 8; 
	if (partie.langage == 3)
		caractere = 6;
	
	SDL_Rect rect = {.x = x - 10, .y = y - 5, .w = longueur_texte[ligne] * caractere + 20, .h = 35};
	if (SDL_RenderCopy(renderer, textures_tuiles[7], NULL, &rect) != 0)
		SDL_ExitWithError("Impossible d'afficher la texture");
	
	SDL_Rect message_rect = {.x = x, .y = y, .w = longueur_texte[ligne] * caractere, .h = 25};
	if (SDL_RenderCopy(renderer, texture_textes[ligne], NULL, &message_rect) < 0)
		SDL_ExitWithError("Impossible d'écrire message debloque");
}

void TexteTuto(int ligne)
{
	int caractere = 8; 
	if (partie.langage == 3)
		caractere = 6;
	
	SDL_Rect message_rect = {.x = 230, .y = 15, .w = longueur_texte[ligne] * caractere, .h = 25};
	if (SDL_RenderCopy(renderer, texture_textes[ligne], NULL, &message_rect) < 0)
		SDL_ExitWithError("Impossible d'écrire message debloque");
}

void AfficherTuto(void)
{
	if (joueur.annee == 2022)
		switch(joueur.terrain_actif->pays->numero)
		{
			case 0:
				TexteTuto(49);
				break;
			case 1:
				TexteTuto(50);
				break;
			case 2:
				TexteTuto(51);
				break;
			case 3:
				TexteTuto(52);
				break;
			case 4:
				TexteTuto(53);
				break;
			default:
				break;
		}
	else if (joueur.annee == 2023)
		switch(joueur.terrain_actif->pays->numero)
		{
			case 0:
				TexteTuto(54);
				break;
			case 1:
				TexteTuto(55);
				break;
			case 2:
				TexteTuto(56);
				break;
			case 3:
				TexteTuto(57);
				break;
			case 4:
				TexteTuto(58);
				break;
			default:
				break;
		}
		
}

void AfficherCredits(void)
{
	SDL_Rect message_rect = {.x = (LARGEUR_FENETRE - longueur_texte[32] * 30) / 2, .y = 50, .w = longueur_texte[32] * 30, .h = 80};
	if (SDL_RenderCopy(renderer, texture_textes[32], NULL, &message_rect) < 0)
		SDL_ExitWithError("Impossible d'écrire l'intro");
	
	message_rect.x = (LARGEUR_FENETRE - longueur_texte[64] * 9) / 2;
	message_rect.y = 180;
	message_rect.w = longueur_texte[64] * 9;
	message_rect.h = 30;
	if (SDL_RenderCopy(renderer, texture_textes[64], NULL, &message_rect) < 0)
		SDL_ExitWithError("Impossible d'écrire message debloque");
	
	message_rect.x = (LARGEUR_FENETRE - longueur_texte[65] * 8) / 2;
		message_rect.y = 220;
		message_rect.w = longueur_texte[65] * 8;
		message_rect.h = 25;
		if (SDL_RenderCopy(renderer, texture_textes[65], NULL, &message_rect) < 0)
			SDL_ExitWithError("Impossible d'écrire message debloque");
	
	for (int i = 0; i < 5; i++)
	{
		message_rect.x = (LARGEUR_FENETRE - longueur_texte[66 + i] * 8) / 2;
		message_rect.y = 270 + i * 30;
		message_rect.w = longueur_texte[66 + i] * 8;
		message_rect.h = 25;
		if (SDL_RenderCopy(renderer, texture_textes[66 + i], NULL, &message_rect) < 0)
			SDL_ExitWithError("Impossible d'écrire message debloque");
	}
	
	for (int i = 0; i < 4; i++)
	{
		message_rect.x = (LARGEUR_FENETRE - longueur_texte[71 + i] * 8) / 2;
		message_rect.y = 440 + i * 30;
		message_rect.w = longueur_texte[71 + i] * 8;
		message_rect.h = 25;
		if (SDL_RenderCopy(renderer, texture_textes[71 + i], NULL, &message_rect) < 0)
			SDL_ExitWithError("Impossible d'écrire message debloque");
	}
	
	for (int i = 0; i < 5; i++)
	{
		message_rect.x = (LARGEUR_FENETRE - longueur_texte[75 + i] * 8) / 2;
		message_rect.y = 580 + i * 30;
		message_rect.w = longueur_texte[75 + i] * 8;
		message_rect.h = 25;
		if (SDL_RenderCopy(renderer, texture_textes[75 + i], NULL, &message_rect) < 0)
			SDL_ExitWithError("Impossible d'écrire message debloque");
	}
}

void AfficherPerformances(Terrain *terrain)
{
	AfficherLesTerrainsSansJoueur(terrain);
	
	SDL_Rect rect = {0, 0, LARGEUR_FENETRE, HAUTEUR_FENETRE};
	if (SDL_RenderCopy(renderer, textures_tuiles[5], NULL, &rect) != 0)
		SDL_ExitWithError("Impossible d'afficher la texture");
	if (SDL_RenderCopy(renderer, textures_tuiles[5], NULL, &rect) != 0)
		SDL_ExitWithError("Impossible d'afficher la texture");
	
	SDL_Rect message_rect = {(LARGEUR_FENETRE - longueur_texte[80] * 9) / 2,
						200, longueur_texte[80] * 9, 30};
	if (SDL_RenderCopy(renderer, texture_textes[80], NULL, &message_rect) < 0)
		SDL_ExitWithError("Impossible d'écrire message debloque");
	
	char temps[20];
	sprintf(temps, "%02dh %02dm %02ds", joueur.temps / 3600, joueur.temps % 3600 / 60, joueur.temps % 3600 % 60);
	EcrireTexteProvisoire(temps, LARGEUR_FENETRE / 2 + 8, 300, 11 * 8, 25);
	char pas[20];
	sprintf(pas, "%06d", joueur.nombre_pas);
	EcrireTexteProvisoire(pas, LARGEUR_FENETRE / 2 + 8, 330, 6 * 8, 25);
	char deplacements[20];
	sprintf(deplacements, "%05d", joueur.nombre_deplacements);
	EcrireTexteProvisoire(deplacements, LARGEUR_FENETRE / 2 + 8, 360, 5 * 8, 25);
	char recyclages[20];
	sprintf(recyclages, "%04d", joueur.nombre_recyclages);
	EcrireTexteProvisoire(recyclages, LARGEUR_FENETRE / 2 + 8, 390, 4 * 8, 25);
	char resets[20];
	sprintf(resets, "%03d", joueur.nombre_resets);
	EcrireTexteProvisoire(resets, LARGEUR_FENETRE / 2 + 8, 420, 3 * 8, 25);
	
	message_rect.h = 25;
	for (int i = 0; i < 5; i++)
	{		
		message_rect.x = LARGEUR_FENETRE / 2 - longueur_texte[81 + i] * 8;
		message_rect.y = 300 + i * 30;
		message_rect.w = longueur_texte[81 + i] * 8;
		if (SDL_RenderCopy(renderer, texture_textes[81 + i], NULL, &message_rect) < 0)
			SDL_ExitWithError("Impossible d'écrire message debloque");
	}		
}

void AfficherCarteAction(int nombre)
{
	SDL_Rect rect_illu = {.x = LARGEUR_FENETRE / 2, .y = 100, .w = 300, .h = 450};
	SDL_Rect message_rect = {.x = LARGEUR_FENETRE / 2 + 10, .h = 20};
	
	int caractere = 7; 
	if (partie.langage == 3)
		caractere = 6;
	
	if (nombre == 3)
	{			
		if (SDL_RenderCopy(renderer, textures_images[11], NULL, &rect_illu) != 0)
			SDL_ExitWithError("Impossible d'afficher la texture");
		
		for (int i = 0; i < 12; i++)
		{
		message_rect.y = 300 + 20 * i;
		message_rect.w = longueur_texte[92 + i] * caractere;
		if (SDL_RenderCopy(renderer, texture_textes[92 + i], NULL, &message_rect) < 0)
			SDL_ExitWithError("Impossible d'écrire carte action");
		}
	}
	if (nombre == 4)
	{	
		if (SDL_RenderCopy(renderer, textures_images[12], NULL, &rect_illu) != 0)
			SDL_ExitWithError("Impossible d'afficher la texture");
		
		for (int i = 0; i < 10; i++)
		{
		message_rect.y = 300 + 20 * i;
		message_rect.w = longueur_texte[104 + i] * caractere;
		if (SDL_RenderCopy(renderer, texture_textes[104 + i], NULL, &message_rect) < 0)
			SDL_ExitWithError("Impossible d'écrire carte action");
		}
	}
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
	rect_illu.x = position_x; 
	rect_illu.y = 0 + position_y;
	
	if (SDL_GetTicks() / 400 % 2 == 0)
	{
		if (SDL_RenderCopy(renderer, textures_images[1], NULL, &rect_illu) != 0)
			SDL_ExitWithError("Impossible d'afficher la texture");
	}
	else
		if (SDL_RenderCopy(renderer, textures_images[10], NULL, &rect_illu) != 0)
			SDL_ExitWithError("Impossible d'afficher la texture");
}

void AfficherPousser(int position_x, int position_y)
{
	SDL_Rect rect_illu = {.w = 2*TUILE, .h = 2.5*TUILE};
	rect_illu.x = position_x; 
	rect_illu.y = position_y;
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

void AfficherCiel(Terrain *t)
{
	time_t timestamp = time( NULL );
    struct tm * pTime = gmtime( & timestamp );
    int heure = pTime->tm_hour + t->pays->variation_gmt;
    if (heure < 0)
		heure += 24;
	else if (heure > 24)
		heure -= 24;
	
	int tuile = 12; 	
	if ((heure > 4 && heure <= 8) || (heure > 17 && heure <= 20))
		tuile = 11;
	else if (heure > 8 && heure <= 17)
		tuile = 10;
		
	SDL_Rect rect_illu = {0, 0, LARGEUR_FENETRE, HAUTEUR_FENETRE};
	if (SDL_RenderCopy(renderer, textures_tuiles[tuile], NULL, &rect_illu) != 0)
		SDL_ExitWithError("Impossible d'afficher la texture");
	
}




