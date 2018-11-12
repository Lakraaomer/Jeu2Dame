#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../define.h"
#include "../../mods/Vector.h"
#include "../../mods/Pion.h"
#include "../../mods/Player.h"
#include "sdlBoard.h"
#include "gui.h"

/**
 * Creer une fenetre et renvoie le renderer
 * On recupere aussi l'adresse du pointeur de la fenetre
 */
SDL_Renderer * createWindow(int height, int width)
{
    /* Initialisation, création de la fenêtre et du renderer. */
    if(0 != SDL_Init(SDL_INIT_VIDEO))
    {
        fprintf(stderr, "Erreur SDL_Init : %s\n", SDL_GetError());
        return NULL;
    }
    window = SDL_CreateWindow("SDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              height, width, SDL_WINDOW_SHOWN);
    if(NULL == window)
    {
        fprintf(stderr, "Erreur SDL_CreateWindow : %s\n", SDL_GetError());
        return NULL;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(NULL == renderer)
    {
        fprintf(stderr, "Erreur SDL_CreateRenderer : %s\n", SDL_GetError());
        return NULL;
    }

    return renderer;
}

/**
 * Load a picture and return the Texture
 */
SDL_Texture *loadImage(const char path[], SDL_Renderer *renderer)
{
    SDL_Surface *tmp = NULL; 
    SDL_Texture *texture = NULL;
    tmp = SDL_LoadBMP(path);
    if(NULL == tmp)
    {
        fprintf(stderr, "Erreur SDL_LoadBMP : %s\n", SDL_GetError());
        return NULL;
    }

    // Surface
    texture = SDL_CreateTextureFromSurface(renderer, tmp);
    // Libere l'espace aloué par la surface
    SDL_FreeSurface(tmp);

    if(NULL == texture)
    {
        fprintf(stderr, "Erreur SDL_CreateTextureFromSurface : %s\n", SDL_GetError());
        return NULL;
    }

    // On recupere les dimensions de l'image
    // A terme on rajoutera la taille en parametre et on redimensionnera en fonction
    //SDL_QueryTexture(texture, NULL, NULL, &dst.w, &dst.h);
    
    return texture;
}

/**
 * Fonction qui ecrit un texte a l'ecran
 * string est la valeur à afficher
 * position du texte
 * font est la police du texte (à importer au prealable)
 * color du texte
 */
SDL_Surface * write(char * string, SDL_Surface * text,  SDL_Point position, TTF_Font * font, SDL_Color color)
{
    /* Écriture du texte dans la SDL_Surface texte en mode Blended (optimal) */
    if((text = TTF_RenderText_Blended(font, string, color)) == NULL){
        printf("Erreur ecriture\n");
        return NULL;
    }

    // On créer le text (Surface)
    SDL_BlitSurface(text, NULL, pSurf, &position); /* Blit du texte */
    printf("Ecrit %s\n", string);


    // On convertie le text (surface) en texture
    SDL_Texture *texture;
    texture = SDL_CreateTextureFromSurface(renderer, text);

    if(NULL == texture)
    {
        fprintf(stderr, "Erreur SDL_CreateTextureFromSurface : %s\n", SDL_GetError());
    }

    // On copie la texture sur le renderer
    SDL_RenderCopy(renderer, texture, NULL, &position);
    return text;
}

/**
 * Affiche le font d'ecran
 */
int showSdlBackground()
{
    // Affichage du background
    changeColor(orange);
    if(0 != SDL_RenderClear(renderer))
    {
        fprintf(stderr, "Erreur SDL_SetRenderDrawColor : %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int showSdlPlayer()
{
    SDL_Point position = { caseWidth, caseWidth };
    // affichage text
    if(currentPlayer->team == player1.team){
        write("Joueur 1", text, position, police, black);
    }else{
        write("Joueur 2", text, position, police, black);
    }
}

/**
 * Gere les actions du tour quand on click sur le plateau
 */
void clickOnBoard(struct Vector clickPosition)
{
    // Position du text
    if(pionStart == NULL){
        // On selectionne un pion
        printf("On selectionne le pion de depart\n");
        int resultSearchBoard = searchBoard(clickPosition, &pionStart);

        if(resultSearchBoard == 0){ // Case déjà occupée 
            printf("Aucune piece n'est selectionnee\n");
            
            // affichage text
            infoMessage = "Aucune piece n'est selectionnee";
            
            pionStart = NULL;
        }else if(resultSearchBoard == -1){ // Case hors limite
            printf(" Case hors limite\n");
            pionStart = NULL;
        }else if(pionStart->team != currentPlayer->team){ // Le pion n'appartient pas au joueur
            printf(" Ce pion n'appartient pas au joueur\n");
            
            // affichage text
            infoMessage = "Ce pion n'appartient pas au joueur";

            pionStart = NULL;
        }else{
            printf("Pion selectionne\n");
            comboMode = 0; // Réinitialisation du mode Combo
            pionStart->selected = 1;
            // affichage text
            infoMessage = "Selectionnez la destination";
        }


    }else if(pionStart != NULL && equalVector(pionStart->position, clickPosition) && comboMode == 0){
        // L'user re-click sur le pion selectionne
        // On desactive alors le pion
        printf("Desactivation du pion selectionne\n");
        pionStart->selected = 0;
        pionStart = NULL;
        comboMode = 0;
    }else{
        printf("On selectionne la destination\n");
        int resultAction = action(pionStart, clickPosition, currentPlayer);

        // L'action n'a pas aboutie
        if(resultAction == -1){
            printf(" Echec action\n");

        // L'action est une prise
        }else if(resultAction == 2){
            comboMode = 1;
            printf(" Continue action\n");

            // Si le nombre de prise disponible autour du pion est 0
            // Alors ont sort de la boucle marque la fin du tour
            if(comboMode == 1 && testAllPrise(*pionStart) == 0){
                printf(" Plus de prise disponible pour ce tours\n");
                pionStart->selected = 0;
                pionStart = NULL;
                comboMode = 0;
                // Changement de joueur
                if(currentPlayer->team == player1.team){
                    currentPlayer = &player2;
                }else{
                    currentPlayer = &player1;
                }
                return;
            }

        }else{
            printf(" Action reussi\n");

            // Transformation du pion en dame
            if(testTranfo(*pionStart) == 1){
                printf("Tranformation !!!!\n");
                tranfoDame(pionStart);
            }

            // Changement de joueur
            if(currentPlayer->team == player1.team){
                currentPlayer = &player2;
            }else{
                currentPlayer = &player1;
            }

            pionStart->selected = 0; // Deselection du pion
            pionStart = NULL;
        }

    }
}

/**
 * 1. Fonction qui detecte les evenements
 * 2. Agit en fonction 
 * 3. Et retourne
 *      - Si evenement : Retourne le code de l'evenement SQL
 *      - Si pas d'evenement : Retourne 0
 *      - Erreur : -1
 */
int input(SDL_Event event)
{
   switch(event.type){

        case SDL_MOUSEBUTTONUP:
            // Detection left-click
            if(event.button.button == SDL_BUTTON_LEFT)
            {
                // mouse position => event.button.x & event.button.y
                //printf("Click !\n");
                //printf("x = : %d\ty : %d\n", event.button.x, event.button.y);

                SDL_Point mousePosition = {event.button.x, event.button.y };
                
                // Test si le click est dans le plateau
                if(SDL_PointInRect(&mousePosition, &SDLboard) == SDL_TRUE && gameStarted == 1){
                    struct Vector clickPosition = convertPositionSdlToVector(mousePosition);
                    showVector(clickPosition);

                    // Affichage background
                    showSdlBackground();


                    clickOnBoard(clickPosition);

                    // Affichage du player en cours
                    showSdlPlayer();

                    // Affichage du message d'indication
                    write(infoMessage, text, positionText, police, black);

                    // Affichage plateau
                    showSdlBoard();

                    // Renderer Update
                    SDL_RenderPresent(renderer);
                }
                
            }

            return SDL_MOUSEBUTTONUP;

        break;

        case SDL_QUIT:

            printf("Close window\n");
            return SDL_QUIT;

        break;

   }
   // Nothing
   return 0;
}

/**
 * Boucle de jeu
 */
int game()
{
    gameStarted = 1; // Flag jeu lancé
    setTestBoard();
    showSdlBackground();

	player2 = createPlayer(2);
	player1 = createPlayer(1);
    currentPlayer = &player1;
    showSdlPlayer();
    // Affichage du message d'indication
    write("Selectionnez une piece", text, positionText, police, black);
    
    // affichage text
    infoMessage = "Selectionnez un pion";
    if(EXIT_FAILURE == showSdlBoard()){
        printf("Erreur affichage\n");
        return EXIT_FAILURE;
    }
    // Renderer Update
    SDL_RenderPresent(renderer);

    SDL_Event event;

    // Boucle de jeu
	while(player1.score != NB_PION && player2.score != NB_PION)
    {
        // Lecture d'un evenement
        while(SDL_PollEvent(&event))
        {
            if(input(event) == SDL_QUIT)
                return SDL_QUIT;
        }
        SDL_Delay(30);
    }
    // Changement de joueur
    if(currentPlayer->team == player1.team){
        currentPlayer = &player2;
    }else{
        currentPlayer = &player1;
    }
	printf("Player %d a gagne !\n", currentPlayer->team);
	freeBoard();

    gameStarted = 0;
    return 0;
}


int gui()
{
    positionBoard.x = caseWidth; 
    positionBoard.y = caseWidth * 4;
    SDLboard.x = 0; SDLboard.y = 0; 
    SDLboard.h = caseWidth + 9 * caseWidth; SDLboard.w = caseWidth + 9 * caseWidth;
    texturePionPlayer1 = NULL; // Texture des pions du joueur 1
    texturePionPlayer2 = NULL; // Texture des pions du joueur 2
    orange.r = 255; orange.g = 127; orange.b = 40; orange.a = 255;
    black.r = 0; black.g = 0; black.b = 0; black.a = 255;
    blue.r = 0; blue.g = 0; blue.b = 255; blue.a = 255;

    SDLboard.x = positionBoard.x; SDLboard.y = positionBoard.y; // On place le plateau au bonne coordonnées

    
    int statut = EXIT_FAILURE;

    // Creation de la fenetre
    window = NULL;
    renderer = NULL;
    renderer = createWindow(caseWidth * 12, caseWidth * 12 + caseWidth * 3);
    if(renderer == NULL)
    {
        goto Quit;
    }

    
    if(loadPolices() == EXIT_FAILURE) goto Quit;
    positionText.x = caseWidth;
    positionText.y = caseWidth * 2;
    infoMessage = "";

    loadTextures();

    // Lancement partie
    game();
    

    statut = EXIT_SUCCESS;

Quit:

    if(NULL != renderer) SDL_DestroyRenderer(renderer);
    if(NULL != window) SDL_DestroyWindow(window);
    if(NULL != texturePionPlayer1) SDL_DestroyTexture(texturePionPlayer1);
    if(NULL != texturePionPlayer2) SDL_DestroyTexture(texturePionPlayer2);

    SDL_Quit();
    return statut;
}


