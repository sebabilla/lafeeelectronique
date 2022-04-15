#ifndef __JEU_H__
#define __JEU_H__

#define PAYS_DEPART "France"

//------------Initialisation et rechargement----------------------------
Partie InitialisationPartie(void);
void LibererPartie(void);
void ChoixLangage(Clavier c);
int ActionsLangage(void);
void ChoixMenu(Clavier c, int index_menu, int nombre_de_choix);
int ActionsMenu(int index_menu, int nombre_de_choix);

//------------Gestion terrain-------------------------------------------
Terrain *NouveauTerrain(void);
void InitialiserTerrain(Terrain *t, Pays *pays);
void AjouterTerrainFin(Terrain *t, Pays *pays);
void LibererTerrain(Terrain *t);
void DetruireTerrains(Terrain *t);
void ResetTerrain(Terrain *t);
void ResetTousLesTerrains(Terrain *t);
int PotentielRecyclage(Terrain *t);
void EchangerAppareils(Terrain *t);
int ViderUntiers(int compteur_dechets);
Terrain *TrouverPays(Terrain *t, int numero_pays);
int AjouterATerrainSpecifique(Terrain *t, int compteur_dechets);

//------------Gestion Joueur--------------------------------------------
Joueur InitialiserJoueur(Terrain *t);
void LibererJoueur(void);
void ActionJoueur(void);
void BougerJoueur(Clavier c);
void ChangerTerrainActifSuivant(void);
void ChangerTerrainActifPrecedent(void);
int AnneeFinie(void);
void NouvelleAnnee(Terrain *t);
void DebloquerMode(void);

//------------Gestion Dechets-------------------------------------------
int RandomDechet(int actuel);
int GenererDechets(Terrain *t);
void GenererTousLesDechets(Terrain *t);
void AjouterDechets(Terrain *t);
void AjouterTousLesDechets(Terrain *t);

//------------Gestion Interaction Joueurs/Dechets-----------------------
int RencontreDechet(Direction d);
void DetruireDechet(Direction d);
void PousserDechet(Direction d);

#endif
