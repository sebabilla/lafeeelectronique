#ifndef __MUSIQUE_H__
#define __MUSIQUE_H__

#define NB_MUSIQUES 6
#define NB_BRUITAGES 8

void InitialisationSon(void);
void DestructionSon(void);

Mix_Music *ChargementMusique(const char *lien);
void JouerMusique(int m);
void PauseMusique(void);

Mix_Chunk *ChargementBruitages(const char *lien);
void JouerBruitage(int b);

#endif
