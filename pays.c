#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "pays.h"


void InitialiserPays(Pays *pays)
{
	for (int i = 0; i < NOMBRE_PAYS; i++)
	{
		pays[i] = LirePays(i);
		if (i % 2 == 0)
		{
			pays[i].x = 4 * TUILE;
			pays[i].y = 4 * TUILE + i / 2 * (2 + HAUTEUR_TERRAIN) * TUILE;
		}
		else
		{
			pays[i].x = 5 * TUILE + (2 + LARGEUR_TERRAIN) * TUILE;
			pays[i].y = 4 * TUILE + (i - 1) / 2 * (2 + HAUTEUR_TERRAIN) * TUILE;
		}
	}
}

Pays LirePays(int indice)
{
	FILE *fichier;
	if ((fichier = fopen("res/InfoPays.txt", "r")) == 0)
	{
		printf("Erreur: Pas de fichier de données sur les pays");
		exit(EXIT_FAILURE);
	}
	
	Pays p;
	Pays trash;
		
	for (int i = 0; i < NOMBRE_PAYS; i++)
	{
		if (i == indice)
			fscanf(fichier, "%s %d %d %d", p.nom, &p.numero, &p.dechets_electroniques, &p.ratio_lineaire);
		else
			fscanf(fichier, "%s %d %d %d", trash.nom, &trash.numero, &trash.dechets_electroniques, &trash.ratio_lineaire);
	}		
	fclose(fichier);
	
	return p;
	
}
