#include <SDL_mixer.h>

#include "types.h"
#include "musique.h"

int initted;
Mix_Music *musiques[NB_MUSIQUES];
Mix_Chunk *bruitages[NB_BRUITAGES];

void InitialisationSon(void)
{
	int flags = MIX_INIT_OGG;
	initted = Mix_Init(flags);
		
	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) < 0)
	{
		printf("Mix_OpenAudio: %s\n", Mix_GetError());
		exit(EXIT_FAILURE);;
	}

	Mix_AllocateChannels(30);
	
}

void DestructionSon(void)
{
	for (int i = 0; i < NB_BRUITAGES; i++)
		Mix_FreeChunk(bruitages[i]);
	Mix_HaltMusic();
	for (int i = 0; i < NB_MUSIQUES; i++)
		Mix_FreeMusic(musiques[i]);
	Mix_CloseAudio();
}

void ChargerLesMusiques(void)
{
	musiques[0] = ChargementMusique("res/musique_intro.ogg");
	musiques[1] = ChargementMusique("res/musique_menu.ogg");
	musiques[2] = ChargementMusique("res/musique_en_jeu1.ogg");
	musiques[3] = ChargementMusique("res/musique_en_jeu2.ogg");
	musiques[4] = ChargementMusique("res/musique_fin.ogg");
}

void ChargerLesBruitages(void)
{
	bruitages[0] = ChargementBruitages("res/argent0.ogg");
	bruitages[1] = ChargementBruitages("res/argent1.ogg");
	bruitages[2] = ChargementBruitages("res/argent2.ogg");
	bruitages[3] = ChargementBruitages("res/argent3.ogg");
	bruitages[4] = ChargementBruitages("res/argent4.ogg");
	bruitages[5] = ChargementBruitages("res/caisse.ogg");
	bruitages[6] = ChargementBruitages("res/bulle.ogg");
	bruitages[7] = ChargementBruitages("res/frottement1.ogg");
	bruitages[8] = ChargementBruitages("res/frottement2.ogg");
	bruitages[9] = ChargementBruitages("res/cloche.ogg");
}

Mix_Music *ChargementMusique(const char *lien)
{
	Mix_Music *s = Mix_LoadMUS(lien);
	if (s == NULL)
	{
		printf("Mix_LoadMUS: %s\n", Mix_GetError());
		exit(EXIT_FAILURE);
	}
	return s;
}

void JouerMusique(int m)
{
	if(Mix_FadeInMusic(musiques[m], -1, 100) < 0) {
		printf("Mix_PlayMusic: %s\n", Mix_GetError());
	}
}

void PauseMusique(void)
{
	if (Mix_PausedMusic())
		Mix_ResumeMusic();
	else
		Mix_PauseMusic();
}

Mix_Chunk *ChargementBruitages(const char *lien)
{
	Mix_Chunk *b = Mix_LoadWAV(lien);
	if (b == NULL)
	{
		printf("Mix_LoadWAV: %s\n", Mix_GetError());
		exit(EXIT_FAILURE);
	}
	return b;
}

void JouerBruitage(int b)
{
	if(Mix_PlayChannel(-1, bruitages[b], 0) < 0 ) {
		printf("Mix_PlayChannel: %s\n",Mix_GetError()); 
	}
}

void JouerDechets(const Terrain *t, const int i)
{
	for (int x = 1; x < i+1; x++)
		for (int j = 0; j < HAUTEUR_TERRAIN; j++)
			if (JouerUnDechet(t, x, j) == 1)
			{
				JouerBruitage(j);
				return;
			}
}

int JouerUnDechet(const Terrain *t, const int i, const int j)
{
	if (t->matrice_dechets[i][j] == 1)
		return 1;
	
	if (t->suivant != NULL)
		return JouerUnDechet(t->suivant, i, j);
	
	return 0;
}
