#include <SDL.h>
#include <stdio.h>

#include "types.h"
#include "controle.h"

Clavier EntreeJoueur(void)
{
	Clavier c = {.direction = RIEN,
				.bouton = SANS};
	
	SDL_Event event;		
		
	if (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			c.bouton = FERMERFENETRE;
			return c;
		}
	
	const Uint8 *state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_UP])
		c.direction = HAUT;
	else if (state[SDL_SCANCODE_DOWN])
		c.direction = BAS;
	else if (state[SDL_SCANCODE_LEFT])
		c.direction = GAUCHE;
	else if (state[SDL_SCANCODE_RIGHT])
		c.direction = DROITE;
	if (state[SDL_SCANCODE_ESCAPE])
		c.bouton = ECHAP;
	else if (state[SDL_SCANCODE_RETURN])
		c.bouton = ENTREE;
	else if (state[SDL_SCANCODE_SPACE])
		c.bouton = ESPACE;
	}
	
	return c;		
}
