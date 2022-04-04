#ifndef __JEU_H__
#define __JEU_H__

#define PAYS_DEPART "France"

//------------Initialisation et rechargement----------------------------
void InitialisationPartie(Partie *p);
void PartieReset(Partie *p);
void Choixlangage(Partie *p, Clavier c);
int ActionsLangage(Partie *partie);

//------------Gestion terrain-------------------------------------------
Terrain *NouveauTerrain(void);
void InitialiserTerrain(Terrain *t, Pays *pays);
void AjouterTerrainFin(Terrain *t, Pays *pays);
void LibererTerrain(Terrain *t);
void DetruireTerrains(Terrain *t);

//------------Gestion Joueur--------------------------------------------
Joueur *NouveauJoueur(void);
void InitialiserJoueur(Joueur *j, Terrain *t);
int PotentielRecyclage(Terrain *t);
void LibererJoueur(Joueur *j);
Bouton ActionJoueur(Joueur *j);
void BougerJoueur(Joueur *j, Clavier c);
void ChangerTerrainActif(Joueur *j);

//------------Gestion Dechets-------------------------------------------
int GenererDechets(Terrain *t);
void GenererTousLesDechets(Terrain *t);
void AjouterDechets(Terrain *t);
void AjouterTousLesDechets(Terrain *t);

//------------Gestion Interaction Joueurs/Dechets-----------------------
int RencontreDechet(Joueur *j, Direction d);
void DetruireDechet(Joueur *j, Direction d);
void PousserDechet(Joueur *j, Direction d);

#endif
