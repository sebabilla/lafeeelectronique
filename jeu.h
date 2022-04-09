#ifndef __JEU_H__
#define __JEU_H__

#define PAYS_DEPART "France"

//------------Initialisation et rechargement----------------------------
Partie *InitialisationPartie(void);
void LibererPartie(Partie *p);
void ChoixLangage(Partie *p, Clavier c);
int ActionsLangage(Partie *partie);
void ChoixMenu(Partie *p, Clavier c);
int ActionsMenu(Partie *partie);

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
void ActionJoueur(Joueur *j, Partie *p);
void ActionPause(Joueur *j, Partie *p);
void BougerJoueur(Joueur *j, Clavier c);
void ChangerTerrainActifSuivant(Joueur *j);
void ChangerTerrainActifPrecedent(Joueur *j);
int AnneeFinie(Joueur *j);
void NouvelleAnnee(Joueur *j, Partie *p, Terrain *t);
void DebloquerMode(Joueur *j, Partie *p);

//------------Gestion Dechets-------------------------------------------
int RandomDechet(int actuel);
int GenererDechets(Terrain *t);
void GenererTousLesDechets(Terrain *t);
void AjouterDechets(Terrain *t);
void AjouterTousLesDechets(Terrain *t);

//------------Gestion Interaction Joueurs/Dechets-----------------------
int RencontreDechet(Joueur *j, Direction d);
void DetruireDechet(Joueur *j, Direction d);
void PousserDechet(Joueur *j, Direction d);

#endif
