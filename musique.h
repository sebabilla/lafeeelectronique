#ifndef __MUSIQUE_H__
#define __MUSIQUE_H__

#define NB_MUSIQUES 5
#define NB_BRUITAGES 10

void InitialisationSon(void);
void DestructionSon(void);

void ChargerLesMusiques(void);
void ChargerLesBruitages(void);

Mix_Music *ChargementMusique(const char *lien);
void JouerMusique(int m);
void PauseMusique(void);

Mix_Chunk *ChargementBruitages(const char *lien);
void JouerBruitage(int b);

void JouerDechets(const Terrain *t, const int i);
int JouerUnDechet(const Terrain *t, const int i, const int j);

#endif
