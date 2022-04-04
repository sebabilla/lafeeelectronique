#include <SDL_mixer.h>

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
/*		
	musiques[0] = ChargementMusique("res/resultat.ogg");
	bruitages[0] = ChargementBruitages("res/b0.ogg");
*/
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
