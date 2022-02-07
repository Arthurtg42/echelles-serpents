# echelles-serpents
Règles :
- le joueur lance deux dés à 6 faces.\n");
- il avance d'un nombre de cases correspondant à la somme des deux dés.
- si la case d'arrivée est une échelle le joueur avance jusqu'à la case correspondante
  à l'autre extrémité de l'échelle.\n");
- si la case d'arrivée est un serpent le joueur recule jusqu'à la case correspondante
  à la queue du serpent.\n");
- si la somme des dés et du numéro de la case dépasse 100 le joueur avance jusqu'à la case 100 puis
  recule du nombre de cases restantes.\n");
- le jeu se termine quand le joueur arrive pile sur la case 100.

Fichiers :
- serpents.c : Code C du déroulement d'une partie
- serpents-echelles.txt : fichier permettant de générer une grille de jeu

Autres :
Vous pouvez générer votre propre grille dans un fichier texte en sachant que :
- Une ligne représente une case du jeu. Chaque ligne est composée de deux
nombres entiers : le numéro d’une case et le numéro d’une case destination
s’il y a une échelle ou un serpent (0 sinon) ;
- Les cases dans le fichier n'ont pas besoin d'être ordonnées.
