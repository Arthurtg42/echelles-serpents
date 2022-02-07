#include <time.h>
#include <stdio.h>
#include <stdlib.h>

/*
** --- Types et variables globales ---
*/
typedef struct CaseJeu{
	unsigned int numero;
	struct CaseJeu* suivante;
	struct CaseJeu* precedente;
	struct CaseJeu* piege;
}CaseJeu;

/*
** --- Fonctions ---
*/

/*
** randomize
**
** Initialise le générateur aléatoire
**
*/
void randomize(void) {
    srand((unsigned) time(NULL));
}

/*
** lancer_un_de
**
** Retourne un entier choisi aléatoirement entre 1 et 6
*/
unsigned int lancer_un_de(void) {
    return (rand() % 6) + 1;
}

/*
** CreerCaseJeu
**
** Alloue la mémoire nécessaire pour créer une case de jeu
** Initialise : le numero avec val, les pointeurs avec NULL
** Renvoi l'adresse de cette nouvelle case
*/
CaseJeu* CreerCaseJeu(unsigned int val) {
	CaseJeu* nv_case = (CaseJeu*) malloc(sizeof(CaseJeu));
	nv_case->numero = val;
	nv_case->suivante = NULL;
	nv_case->precedente = NULL;
	nv_case->piege = NULL;
	return nv_case;
}

/*
** InsertTete
**
** Crée une case de numero val qui pointe au même endroit que grille
** Renvoi l'adresse de cette case
*/
CaseJeu* InsertTete(CaseJeu* grille, unsigned int val){
	CaseJeu* nv_case = CreerCaseJeu(val);
	if(nv_case == NULL)
		return NULL;
	nv_case->suivante = grille;
	return nv_case;
}

/*
** InsertTri
**
** Insère une case de numéro val "au bon endroit" dans la liste chaînée grille (ordonnée)
** et met à jour chaque pointeur vers la case suivante
** Renvoi l'adresse, mise à jour, de grille (début de la liste chaînée)
*/
CaseJeu* InsertTri(CaseJeu* grille, unsigned int val){
	if(grille == NULL)
		return InsertTete(grille, val);
	// Déclaration et initialisation des pointeurs pour parcourir la grille
	CaseJeu* precedent = grille;
	CaseJeu* courant = grille->suivante;
	while(courant != NULL && courant->numero < val){
	// C.I : courant != NULL ET courant->numero < val
		courant = courant->suivante;
		precedent = precedent->suivante;
	} // Etat de sortie : courant == NULL OU courant->numero >= val
	// Si la valeur doit être en début de grille
	if(precedent == grille && precedent->numero >= val)
		grille = InsertTete(grille, val);
	// Sinon l'insérer entre precedent et courant
	else {
		CaseJeu* nv_case = CreerCaseJeu(val);
		nv_case->suivante = courant;
		precedent->suivante = nv_case;
	}
	return grille;
}

/*
** CaseNum
**
** Renvoi l'adresse de la case de numero val dans la liste chaînée grille
** NULL si non-trouvée
*/
CaseJeu* CaseNum(CaseJeu* grille, unsigned int val){
	if(grille == NULL || grille->numero == val)
		return grille;
	else
		grille = CaseNum(grille->suivante, val);
	return grille;
}

/*
** CaseMax
**
** Renvoi le plus grand numéro de la liste chaînée grille
** 0 si grille est NULL
** grille : un pointeur sur une liste chaînée ordonnée
*/
unsigned int CaseMax(CaseJeu* grille){
	if(grille == NULL)
		return 0;
	unsigned int max = grille->numero;
	while(grille->suivante != NULL){
		grille = grille->suivante;
		if(grille->numero > max)
			max = grille->numero;
	}
	return max;
}

/*
** NbrCase
**
** Renvoi le nombre de cellules de la liste chainée pointée par grille
** 0 si grille est NULL
*/
unsigned int NbrCase(CaseJeu* grille){
	if(grille == NULL)
		return 0;
	unsigned int nb = 1;
	while(grille->suivante != NULL){
		grille = grille->suivante;
		nb++;
	}
	return nb;
}

/*
** Deplace
**
** Se déplace de nb_case cases à partir de pos et renvoi l'adresse "d'arrivée"
** Si le déplacement dépasse la dernière case on "revient en arrière" du nbr de cases restantes
** Si le déplacement "sort" de la grille en "reculant" renvoi NULL
*/
CaseJeu* Deplace(CaseJeu* pos, unsigned int nb_case){
	if(pos == NULL)
		return NULL;
	// sens = 1 signifie que l'on "avance"
	// sens = -1 signifie que l'on "recule"
	int sens = 1;
	while(pos != NULL && nb_case > 0){
	//C.I : pos != NULL (on est bien dans la grille) ET nb_case > 0 il reste des déplacements
		// si on est à la dernière case de la grille on "passe en mode recule"
		if(pos->suivante == NULL){
			printf("\rLancé de dés trop élevé : ");
			sens = -1;
		}
		// actualisation de pos en fonction de sens
		if(sens == 1)
			pos = pos->suivante;
		else if(sens == -1)
			pos = pos->precedente;
		nb_case--;
	}// Etats de sortie
	 // pos == NULL : c'est à dire que l'on est sorti de la grille mais en "reculant"
	 // OU
	 // nb_case = 0 : le déplacement a été réalisé et donc pos pointe sur la case d'arrivée
	return pos;
}

/*
** InsereCase
**
** Insere une case de numero val vers un potentiel piege dans la liste chainée grille.
** Créer cette case piege si piege != 0 et si elle n'existe pas.
** La liste chaînée garde son ordre croissant.
** Le pointeur piege de chaque case est initilisé si nécessaire
*/
void InsereCase(CaseJeu** grille, unsigned int val, unsigned int piege){
	// Insertion de la case(numero) si elle n'existe pas
	if(CaseNum(*grille, val) == NULL)
		*grille = InsertTri(*grille, val);
	// Si la case a une échelle ou un serpent
	if(piege != 0){
		// Insertion de la case(piege) visée si elle n'existe pas
		if(CaseNum(*grille, piege)==NULL)
			*grille = InsertTri(*grille, piege);
		// Actualisation du pointeur piege de la case(numero) vers la case(piege)
		CaseNum(*grille, val)->piege = CaseNum(*grille, piege);
	}
}

/*
** InitPointeurPrecedente
**
** Parcours la liste chaînée et initialise le pointeur precedente de chaque case
** grille doit être une liste chaînée d'au moins 2 cellules
*/
void InitPointeurPrecedente(CaseJeu** grille){
	// Déclaration et initialisation des pointeurs pour parcourir la grille
	CaseJeu* precedent = *grille;
	CaseJeu* courant = (*grille)->suivante;
	// Boucle pour initialiser chaque pointeur precedente vers la case précédente
	while(courant != NULL){
		courant->precedente = precedent;
		precedent = precedent->suivante;
		courant = courant->suivante;
	}
}

/*
** lire_fichier_grille
**
** Lit le fichier texte pour le nom donné en argument
** Crée la structure de données représentant la grille
** Retourne le pointeur sur cette structure de données
*/
CaseJeu* lire_fichier_grille(char* nom_fichier) {
	// Déclaration et initialisation des variables
	FILE* f = NULL;
	unsigned int numero, piege;
	CaseJeu* grille = NULL;
	// Test d'ouverture du fichier
	f = fopen(nom_fichier, "r");
	if(f == NULL){
		fprintf(stderr, "L'ouverture en 'r' du fichier %s a échoué.\n", nom_fichier);
		return NULL;
	}
	while(fscanf(f,"%u %u", &numero, &piege) != EOF){
	// C.I : au moins 1 entiers a été lu dans le fichier f
		InsereCase(&grille, numero, piege);
	} // Etat de sortie : fin du fichier
	fclose(f);
	// Test s'il y a au moins 2 cellules dans la grille,
	// car sinon pas besoin d'initialiser les pointeurs vers les cases précedentes
	if(grille != NULL && grille->suivante != NULL){
		InitPointeurPrecedente(&grille);
	}
	return grille;
}

/*
** jouer_jeu
**
** Moteur de jeu s'éxécutant en fonction de la grille donnée en argument
** Le jeu évolue en fonction des entrées du joueur
*/
void jouer_jeu(CaseJeu* grille) {
	// Déclaration et initialisation des variables
	CaseJeu* pos = grille;
	unsigned int de1, de2, nbrCase, caseMAX;
	char saisie = '0';
	nbrCase = NbrCase(grille);
	caseMAX = CaseMax(grille);
	if(nbrCase < 3){
		printf("Vérifier votre fichier .txt de la grille de jeu.\n");
		printf("Comme nous lançons 2 dés à 6 faces, il nous faut au moins 3 cases pour jouer !\n");
		return;
	}
	// Rappel des règles
	printf("\n-----------------------Jeu serpents et échelles------------------------\n\n");
	printf("Règle : - le joueur lance deux dés à 6 faces.\n");
	printf("        - il avance d'un nombre de cases\n");
	printf("          correspondant à la somme des deux dés.\n");
	printf("        - si la case d'arrivée est une échelle\n");
	printf("          le joueur avance jusqu'à la case correspondante\n");
	printf("          à l'autre extrémité de l'échelle.\n");
	printf("        - si la case d'arrivée est un serpent\n");
	printf("          le joueur recule jusqu'à la case correspondante\n");
	printf("          à la queue du serpent.\n");
	printf("        - si la somme des dés et du numéro de la case dépasse la case maximale\n");
	printf("          le joueur avance jusqu'à la case maximale puis\n");
	printf("          recule du nombre de cases restantes.\n");
	printf("        - le jeu se termine quand le joueur arrive pile sur la case maximale.\n");
	printf("\n---------------------------------------------------------------------\n\n");
	// Debut du jeu
	printf("Il y a %u cases.\n", nbrCase);
	printf("Le numéro max est le %u.\n", caseMAX);
	printf("La première case est la case %u.\nVous etes en case %u.\n", pos->numero, pos->numero);
	printf("\nVoulez-vous lancer une partie de serpents et échelles ? (pour arreter saisir 0) : ");
	scanf(" %c", &saisie);
	while(saisie != '0'){
	//C.I : saisie != '0' (le joueur ne veut pas arretez)
		de1 = lancer_un_de();
		de2 = lancer_un_de();
		printf("Vous lancez les dés : %u et %u.\n", de1, de2);
		// Test si le déplacement ne sort pas de la grille "en reculant"
		if(Deplace(pos,de1+de2) != NULL){
			pos = Deplace(pos, de1+de2);
		}
		// sinon, Deplace renvoi NULL il faut se repositionner au début
		else{
			printf("\rLancé de dés trop élevé : ");
			pos = grille;
		}
		printf("Vous arrivez en case %u.\n", pos->numero);
		// Test s'il y a un piege
		if(pos->piege != NULL){
			// Affichage du type de piège
			if(pos->numero < pos->piege->numero){
				printf("Une échelle : vous avancez en case %u.\n", pos->piege->numero);
			}
			else if(pos->numero > pos->piege->numero){
				printf("Un serpent : vous reculez en case %u.\n", pos->piege->numero);
			}
			pos = pos->piege;
		}
		// Test si arrivé, avec saisie à '0' forcée pour sortir, sinon demande nouvelle saisie
		if(pos->numero == caseMAX){
			printf("Vous avez terminé le jeu.\n");
			saisie = '0';
		}
		else{
			printf("\nContinuer (pour arreter saisir 0) ? ");
			scanf(" %c", &saisie);
		}
	}
}

/*
** supprimer_grille
**
** Libère la mémoire utilisée pour représenter la grille du jeu
*/
void supprimer_grille(CaseJeu* grille) {
	CaseJeu* suppr;
	while(grille->suivante != NULL){
		suppr = grille;
		grille = grille->suivante;
		free(suppr);
	}
	free(grille);
}

/*
** Affiche
**
** Affiche la liste chainée grille
**
*/
void Affiche(CaseJeu* grille){
	if(grille !=NULL && grille->suivante != NULL){
		if(grille->precedente != NULL)
			printf("%u <- ", grille->precedente->numero);
		printf("%u -> %u",  grille->numero, grille->suivante->numero);
		if(grille->piege != NULL)
			printf(" | piege vers : %u", grille->piege->numero);
		printf("\n");
		Affiche(grille->suivante);
	}
}

/*
** main
**
** Fonction principale
**
*/
int main(/*int argc, const char * argv[]*/) {
	CaseJeu* grille;
	randomize();
	/*  Modifier le chemin en fonction de votre environnement */
	grille = lire_fichier_grille("./serpents-echelles.txt");
	printf("La grille de jeu : (case precedente) <- case -> (case suivante) | piege potentiel\n");
	Affiche(grille);
	jouer_jeu(grille);
	supprimer_grille(grille);
	return 0;
}
