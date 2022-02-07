#include <time.h>
#include <stdio.h>
#include <stdlib.h>

/*
** --- Types et variables globales ---
*/
typedef struct CaseJeu{
	unsigned int numero;
	struct CaseJeu* suivante;
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
*/
CaseJeu* CreerCaseJeu(unsigned int val) {
	CaseJeu* nv_case = (CaseJeu*) malloc(sizeof(CaseJeu));
	nv_case->numero = val;
	nv_case->suivante = NULL;
	nv_case->piege = NULL;
	return nv_case;
}

/*
** InsertTete
**
** Crée une case de numéro val qui pointe au même endroit que grille
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
** Insère une case de numéro val au bon endroit dans la liste chaînée grille
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
	} // E.S : courant == NULL OU courant->numero >= val
	// Si la valeur doit être en début de grille
	if(precedent == grille && precedent->numero >= val){
		grille = InsertTete(grille, val);
	} // Sinon l'insérer entre precedent et courant
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
** NULL si non trouvée
*/
CaseJeu* CaseNum(CaseJeu* grille, unsigned int val){
	if(grille == NULL || grille->numero == val){
		return grille;
	}
	else{
		grille = CaseNum(grille->suivante, val);
	}
	return grille;
}

/*
** Deplace
**
** Se déplace de nb_case case à partir de pos en mettant à jour le contenu de pos
*/
void Deplace(CaseJeu** pos, unsigned int nb_case){
	if(pos != NULL && nb_case > 0){
		*pos = (*pos)->suivante;
		Deplace(pos, nb_case - 1);
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
	// C.I : deux entiers positifs ont été lus dans le fichier f
		// Insertion de la case(numero) si elle n'existe pas
		if(CaseNum(grille, numero) == NULL)
			grille = InsertTri(grille, numero);
		// Si la case a une echelle ou un serpent
		if(piege != 0){
			// Insertion de la case(piege) visée si elle n'existe pas
			if(CaseNum(grille, piege)==NULL)
				grille = InsertTri(grille, piege);
			// Actualisation du pointeur piege de la case(numero) vers la case(piege)
			CaseNum(grille, numero)->piege = CaseNum(grille, piege);
		}
	} // E.S : fin du fichier
	fclose(f);
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
	unsigned int de1, de2;
	char saisie = '0';
	// Rappel des règles
	printf("-----------------------Jeu serpents et échelles------------------------\n\n");
	printf("Règle : - le joueur lance deux dés à 6 faces.\n");
	printf("        - il avance d'un nombre de cases\n");
	printf("          correspondant à la somme des deux dés.\n");
	printf("        - si la case d'arrivée est une échelle\n");
	printf("          le joueur avance jusqu'à la case correspondante\n");
	printf("          à l'autre extrémité de l'échelle.\n");
	printf("        - si la case d'arrivée est un serpent\n");
	printf("          le joueur recule jusqu'à la case correspondante\n");
	printf("          à la queue du serpent.\n");
	printf("        - si la somme des dés et du numéro de la case dépasse 100\n");
	printf("          le joueur avance jusqu'à la case 100 puis\n");
	printf("          recule du nombre de cases restantes.\n");
	printf("        - le jeu se termine quand le joueur arrive pile sur la case 100.\n");
	printf("\n---------------------------------------------------------------------\n\n");
	// Debut du jeu
	printf("Vous etes en case %u.\n", pos->numero);
	printf("Voulez-vous lancer une partie de serpents et échelles ? (pour arreter saisir 0) : ");
	scanf(" %c", &saisie);
	while(saisie != '0'){
	//C.I : saisie != '0' (le joueur ne veut pas arretez)
		de1 = lancer_un_de();
		de2 = lancer_un_de();
		printf("Vous lancez les dés : %u et %u.\n", de1, de2);
		if(((pos->numero)+de1+de2) <= 100){
			Deplace(&pos, de1+de2);
			printf("Vous arrivez en case %u.\n", pos->numero);
		}
		// sinon, si on "dépasse" la case 100
		else if((((pos->numero)+de1+de2) > 100)){
			// il faut aller en case 100 puis reculez du nombre restant=(numero+de1+de2)-100)
			// autrement dit il faut aller en case [100-((numero+de1+de2)-100)]
			// équivalent à la case [200-(numero+de1+de2)]
			pos = CaseNum(grille, 200-(pos->numero+de1+de2));
			printf("Lancés de dés trop élevé : vous allez en case %u.\n", pos->numero);
		}
		// Test s'il y a un piege
		if(pos->piege != NULL){
			if(pos->numero < pos->piege->numero){
				printf("Une échelle : vous avancez en case %u.\n", pos->piege->numero);
			}
			else if(pos->numero > pos->piege->numero){
				printf("Un serpent : vous reculez en case %u.\n", pos->piege->numero);
			}
			pos = pos->piege;
		}
		// Test si arrivé ou demande de nouvelle saisie
		if(pos->numero == 100){
			printf("Vous avez terminé le jeu.\n");
			saisie = '0';
		}
		else{
			printf("Continuer (pour arreter saisir 0) ? ");
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
    
    jouer_jeu(grille);
    supprimer_grille(grille);
    return 0;
}
