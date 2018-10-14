/**
 * Programme principale
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <SDL/SDL.h>

#include "Vector.h"
#include "Pion.h"
#include "Player.h"

/*

LE PLATEAU
-----------------------------------------------------------------------------
On modélise la map dans un tableau 2D. Chaque case du tableau est un pointeur de Pion
Si le pointeur est NULL il n'y a pas de pion sur cette case.

LES PIONS
-----------------------------------------------------------------------------
Les pions sont des structures représentant un pion en générale. On différenti les pions en
de leurs tableau de déplacement, leurs team (numéro du joueur) et leur coordonnées sur le plateau. 
Le tableau de déplacement est le nombre de déplacement possible 
pour un pion dans toutes les directions.


LES DEPLACEMENTS
-----------------------------------------------------------------------------
Les déplacements d'un pion sont représenté par un tableau de vecteur déplacement. 
Pour tester la validité d'un déplacement on vérifie si le vecteur déplacement est compris à l'interieur de l'un des
vecteurs deplacements de la moveList attachée au pion.


LES PRISES
-----------------------------------------------------------------------------
Une prise s'efectue quand le pion se déplace au dessus d'un seul pion adverse. 
Les conditons sont : 
	- La prise doit être de seulement 1 piont
	- La prise doit se faire par une trajectoire lineaire
	- Un pion ne peut pas passer au dessus d'un autre pion (même dans le cas d'un déplacement)
	- Pendant une prise, le pion augmente sa porté de 1
	- La case destination doit être libre


LES ACTIONS
-----------------------------------------------------------------------------
Un action est l'enchainement d'un déplacement et ou une prise la fonction action va mettre en relation les tests
de déplacement ainsi que les tests de prises pour effectuer les déplacements et les prises
Une action de déroule en plusieurs étapes : 

	- On commence par parcourir la trajectoire demandé pour déterminer si les prises sont correctes.
	- Ensuite on augmente la porté du pion pour cela on augmente tout les vecteurs deplacement du pion de 
		1 unite vecteur. 
	- Ensuite on effectue un déplacement classique (avec la porté augmenter)

	- A la fin du tour, il ne faut pas oublier de remettre les vecteurs déplacement à leurs valeurs initiales.


TOURS DE JEU
-----------------------------------------------------------------------------
Le jeu se déroule en tour par tour, les deux joueurs jouent chaqu'un leur tour.
Quand un joueur effectue une prise, il peut rejouer pour faire des combos 

*/


/**
 * Effectue une action 
 * move, move + prise...
 * La fonction retourne -1 en cas d'echec ex: saut au dessus de plusieurs prises
 * Sinon retourne 1 en cas de succès
 */
int action(struct Pion pion, struct Vector point, struct Player * player){
	struct Vector prise;
	int flagPrise = 0;
	
	showPion(pion);
	showVector(point);

	// Test de la case destination
	if(board[point.x][point.y] == NULL){
		switch(testPrise(pion, point, &prise)){
			case 1 : 
				printf(" Prise trouvee\n");
				flagPrise = 1;

				// On parcourt tous les déplacements possible du pion
				for(int i = 0; i < pion.nbMove; i++){
					
					// On increment le vecteur de 1 unite
					pion.moveList[i] = addVector(pion.moveList[i], unitVector(pion.moveList[i])); 
					
				}

			break;

			case -1 : 

				printf(" Erreur prise\n");
				return -1; // Action annulée

			break;

		}

		// Test de la validite du deplacement par rapport au possibilite du pion
		if(testMove(pion, point)){
			printf(" Deplacement possible\n");
			move(pion, point);
		}else{
			printf(" Le pion ne permet pas ce type de deplacement\n");
		}

		if(flagPrise){

			// On parcourt tous les déplacements possible du pion
			for(int i = 0; i < pion.nbMove; i++){
				// On decrement le vecteur de 1 unite
				pion.moveList[i] = subVector(pion.moveList[i], unitVector(pion.moveList[i])); 
			}

			board[prise.x][prise.y] = NULL;
			player->score++;
			printf(" Prise effectue\n");

		}

		return 1;

	}else{
		printf(" Impossible, la case de destination est deja occupee\n");
		return -1;
	}
}



// PROGRAMME PRINCIPALE
// -------------------------------------------------------------------------------------
int main()
{
	//setBoard();
	setTestBoard();


	struct Player player1, player2;
	struct Player * currentPlayer;
	player1 = createPlayer(1);
	player2 = createPlayer(2);

	currentPlayer = &player2;

	struct Vector start, end;

	// Boucle de jeu
	while(player1.score != NB_PION && player2.score != NB_PION){

		if(currentPlayer->team == player1.team){
		currentPlayer = &player2;
		}else{
			currentPlayer = &player1;
		}

		showBoard();
		printf("Joueur %d\n", currentPlayer->team);
		printf(" Start : \n");
		printf(" => x : ");
		scanf("%d", &start.x);
		printf(" => y : ");
		scanf("%d", &start.y);

		printf(" End : \n");
		printf(" => x : ");
		scanf("%d", &end.x);
		printf(" => y : ");
		scanf("%d", &end.y);

		if(board[start.x][start.y] != NULL && action(*board[start.x][start.y], end, currentPlayer) == 1){
			printf(" Action reussi\n");
		}else{
			printf(" Echec action\n");
		}		
	}
	printf("Player %d a gagne !\n", currentPlayer->team);
	freeBoard();
	return 0;
/*
	struct Vector point; point = createPoint(2, 2); // Point de destination

	board[1][1]->team = 2; // On ajoute une prise
	board[point.x][point.y] = NULL; // On vide la case destination
	showBoard();

	if(action(*board[0][0], point)){
		printf(" Action reussi\n");
	}else{
		printf(" Echec action\n");
	}

	showBoard();

	freeBoard();
    return 0;*/
}