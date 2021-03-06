#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "../../define.h"
#include "../../mods/Vector.h"
#include "../../mods/Pion.h"
#include "../../mods/Player.h"
#include "../../mods/network.h"
#include "../../main.h"
#include "../../mods/audio.h"
#include "Menu.h"
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
    printf("str : %s\n", path);
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
    // A terme on rajoutera la taille en thread_parametre et on redimensionnera en fonction
    //SDL_QueryTexture(texture, NULL, NULL, &dst.w, &dst.h);
    
    return texture;
}

/**
 * Charge les textures du jeu
 * Renvoie EXIT_FAILURE en cas d'erreur et EXIT_SUCCESS en cas de succes
 */
int loadTextures(char * name)
{
    char * str;
    str = (char *)malloc(sizeof(char) * 100);

    // Ajout des textures des pions du joueur 1
    str[0] = '\0';
    texturePion1 = NULL;
    str = strcat(str, "images/");
    str = strcat(str, name);
    str = strcat(str, "/pion1.bmp");
    texturePion1 = loadImage(str, renderer);
    
    if(NULL == texturePion1)
    {
        printf("Impossible de charger la texture du joueur 1");
        return EXIT_FAILURE;
    }
    
    // Ajout des textures des dames du joueur 1
    str[0] = '\0';
    textureDame1 = NULL;
    str = strcat(str, "images/");
    str = strcat(str, name);
    str = strcat(str, "/dame1.bmp");
    textureDame1 = loadImage(str, renderer);
    
    if(NULL == textureDame1)
    {
        printf("Impossible de charger la texture du joueur 1");
        return EXIT_FAILURE;
    }

    // Ajout des textures des pions du joueur 2
    str[0] = '\0';
    texturePion2 = NULL;
    str = strcat(str, "images/");
    str = strcat(str, name);
    str = strcat(str, "/pion2.bmp");
    texturePion2 = loadImage(str, renderer);
    
    if(NULL == texturePion2)
    {
        printf("Impossible de charger la texture du joueur 2");
        return EXIT_FAILURE;
    }
    
    // Ajout des textures des dames du joueur 2
    str[0] = '\0';
    textureDame2 = NULL;
    str = strcat(str, "images/");
    str = strcat(str, name);
    str = strcat(str, "/dame2.bmp");
    textureDame2 = loadImage(str, renderer);
    
    if(NULL == textureDame2)
    {
        printf("Impossible de charger la texture du joueur 2");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/**
 * Charge les polices du jeu
 * Renvoie EXIT_FAILURE en cas d'erreur et EXIT_SUCCESS en cas de succes
 */
int loadPolices()
{
    TTF_Init();
    police = NULL;
    text = NULL; //*fond = NULL;
    pSurf = SDL_GetWindowSurface(window);
    /* Chargement de la police */
    if((police = TTF_OpenFont("./police/game_over.ttf", 62)) == NULL){
        printf("TTF_OpenFont: %s\n", TTF_GetError());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

/**
 * Change la couleur du cuseur
 */
int changeColor(SDL_Color color)
{

    if(0 != SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a))
    {
        fprintf(stderr, "Erreur SDL_SetRenderDrawColor : %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

/**
 * Fonction qui ecrit un texte a l'ecran
 * string est la valeur à afficher
 * position du texte
 * font est la police du texte (à importer au prealable)
 * color du texte
 */
SDL_Surface * sdlWrite(char * string, SDL_Surface * text,  SDL_Point position, TTF_Font * font, SDL_Color color)
{
    /* Écriture du texte dans la SDL_Surface texte en mode Blended (optimal) */
    if((text = TTF_RenderText_Blended(font, string, color)) == NULL){
        printf("Erreur ecriture\n");
        return NULL;
    }

    SDL_Rect rect_position;
    rect_position.x = position.x;
    rect_position.y = position.y;
    // On créer le text (Surface)
    SDL_BlitSurface(text, NULL, pSurf, &rect_position); /* Blit du texte */


    // On convertie le text (surface) en texture
    SDL_Texture *texture;
    texture = SDL_CreateTextureFromSurface(renderer, text);

    if(NULL == texture)
    {
        fprintf(stderr, "Erreur SDL_CreateTextureFromSurface : %s\n", SDL_GetError());
    }

    // On copie la texture sur le renderer
    SDL_RenderCopy(renderer, texture, NULL, &rect_position);
    return text;
}

/**
 * Affiche le font d'ecran
 */
int showSdlBackground(SDL_Color color)
{
    // Affichage du background
    changeColor(color);
    if(0 != SDL_RenderClear(renderer))
    {
        fprintf(stderr, "Erreur SDL_SetRenderDrawColor : %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

/**
 * Affiche le joueur en cours
 */
void showSdlPlayer()
{
    SDL_Point position = { caseWidth, caseWidth };
    // affichage text
    if(currentPlayer->team == player1.team){
        sdlWrite("Joueur 1", text, position, police, black);
    }else{
        sdlWrite("Joueur 2", text, position, police, black);
    }
}

/**
 * Gere les actions du tour quand on click sur le plateau
 * La fonction renvoie :
 *      - -1 : erreur
 *      - 0 : fin combo
 *      - 1 : selection d'un pion
 *      - 2 : combo en cours
 *      - 3 : fin deplacement
 *      - 4 : Deselection pion
 */
int clickOnBoard(struct Vector clickPosition)
{
    // Selection start
    if(pionStart == NULL)
    {
        // On selectionne un pion
        int resultSearchBoard = searchBoard(clickPosition, &pionStart);

        if(resultSearchBoard == 0){ // Case déjà occupée 
            printf("Aucune piece n'est selectionnee\n");
            
            // affichage text
            infoMessage = "Aucune piece n'est selectionnee";
            
            pionStart = NULL;
            return -1;
        }else if(resultSearchBoard == -1){ // Case hors limite
            printf("Case hors limite\n");
            pionStart = NULL;
            return -1;
        }else if(pionStart->team != currentPlayer->team){ // Le pion n'appartient pas au joueur
            printf("Ce pion n'appartient pas au joueur\n");
            
            // affichage text
            infoMessage = "Ce pion n'appartient pas au joueur";

            pionStart = NULL;
            return -1;
        }else{
            printf("Pion selectionne\n");
            comboMode = 0; // Réinitialisation du mode Combo
            pionStart->selected = 1;
            // affichage text
            infoMessage = "Selectionnez la destination";
            return 1;
        }


    }
    // Deselection
    else if(pionStart != NULL && equalVector(pionStart->position, clickPosition) && comboMode == 0)
    {
        // L'user re-click sur le pion selectionne
        // On desactive alors le pion
        pionStart->selected = 0;
        pionStart = NULL;
        comboMode = 0;
        return 4;
    }
    // Selection d'une destination
    else
    {
        struct Vector posPrise; // Position de la prise

        int resultAction = action(pionStart, clickPosition, currentPlayer, &posPrise);
        // L'action n'a pas aboutie
        if(resultAction == -1)
        {
            infoMessage = "Erreur, pas de prise";
            return -1;
        }
        else if(resultAction == -2)
        {
            infoMessage = "Erreur, hors limite";
            return -1;
        }
        else if(resultAction == -3)
        {
            infoMessage = "Erreur, deja occupee";
            return -1;
        }
        else if(resultAction == -4)
        {
            infoMessage = "Erreur, prise";
            return -1;
        }
        else if(resultAction == -5)
        {
            infoMessage = "Erreur, deplacement impossible";
            return -1;
        }
        // L'action est une prise
        else if(resultAction == 1)
        {
            comboMode = 1;
            printf("Continue action\n");
            // Si le nombre de prise disponible autour du pion est 0
            // Alors ont sort de la boucle, ça marque la fin du tour
            if(comboMode == 1 && testAllPrise(*pionStart) == 0)
            {
                printf("Plus de prise disponible pour ce tours\n");
                pionStart->selected = 0;
                pionStart = NULL;
                comboMode = 0;
                // Changement de joueur
                if(currentPlayer->team == player1.team){
                    currentPlayer = &player2;
                }else{
                    currentPlayer = &player1;
                }
                infoMessage = "Selectionnez un pion";

                // On stocke la position de la prise si on est en mode réseau 
                if(network == 1){
                    printf("Add Prise !\n");
                    thread_param.posPrises[thread_param.nbPrise] = posPrise;
                    thread_param.nbPrise++;
                }
                return 0; // fin combo
            }
            // On stocke la position de la prise si on est en mode réseau 
            if(network == 1){
                printf("Add Prise !\n");
                thread_param.posPrises[thread_param.nbPrise] = posPrise;
                thread_param.nbPrise++;
            }
            return 2; // Combo en cours

        }
        else
        {
            printf("Action reussi\n");

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
            infoMessage = "Selectionnez un pion";
            return 3;
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
                    showVector("Selection", clickPosition);

                    int resultClickOnBoard = clickOnBoard(clickPosition);

                    if((resultClickOnBoard == 0 || resultClickOnBoard == 3) && network == 1) // Fin de tour et mode reseau actif
                    {
                        // Affichage background
                        showSdlBackground(orange);

                        // Affichage du player en cours
                        showSdlPlayer();
                        
                        // Affichage du message d'indication
                        sdlWrite(infoMessage, text, positionText, police, black);

                        // Affichage plateau
                        showSdlBoard();

                        // Renderer Update
                        SDL_RenderPresent(renderer);


                        printf("Enregistrement de la selectionEnd\n");
                        thread_param.posEnd = clickPosition;
                        // On lance le thread serveur
                        printf("Server...\n");
                        void * arg = (void*)&thread_param;

                        // On ouvre une nouvelle connexion dans un thread
                        if(pthread_create(&thread, NULL, network_connect, arg) == -1) {
                            perror("pthread_create");
                        }
                        printf("FIN DE TOUR\n");
                        printf("==================================================================\n");
                        tour = 0;
                    }
                    else if(network == 1 && resultClickOnBoard == 1) // Selection de tour et mode reseau actif
                    {
                        // On stocke la position de la selection dans les parametres de thread 
                        thread_param.posStart = pionStart->position;
                    }

                    // Affichage background
                    showSdlBackground(orange);

                    // Affichage du player en cours
                    showSdlPlayer();

                    // Affichage du message d'indication
                    sdlWrite(infoMessage, text, positionText, police, black);

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

    setBoard();
    showSdlBackground(orange);

	player1 = createPlayer(1, "Joueur 1");
	player2 = createPlayer(2, "Joueur 2");

    currentPlayer = &player1;

    showSdlPlayer();
    // Affichage du message d'indication
    sdlWrite("Selectionnez un pion", text, positionText, police, black);
    
    if(EXIT_FAILURE == showSdlBoard())
    {
        printf("Erreur affichage\n");
        return EXIT_FAILURE;
    }
    
    // Renderer Update
    SDL_RenderPresent(renderer);

    SDL_Event event;
    char response[100]; // Buffer pour la fontion network_client
    // Boucle de jeu
	while(player1.score != NB_PION && player2.score != NB_PION)
    {
        // Lecture d'un evenement
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)
                return SDL_QUIT;
            if(tour == 1)
                input(event);
        }

        if(tour == 0 && network == 1 && network_client(response, thread_param.port_des) == EXIT_SUCCESS)
        {
            // On s'assure que le thread est biens fermé avant de continuer
            if(pthread_join(thread, NULL)) {
                perror("pthread_join");
            }
            tour = 1;
            thread_param.nbPrise = 0; // On reinitialise le nombre de prises

            struct Data data; // Données recupérées par la socket
            data = decode_data(response);
            showVector("Start", data.posStart);
            showVector("End", data.posEnd);

            for(int i = 0; i < data.nbPrise; i++){
                showVector("Prise", data.posPrises[i]);
            }

            board[data.posEnd.x][data.posEnd.y] = board[data.posStart.x][data.posStart.y];
            board[data.posStart.x][data.posStart.y] = NULL;
            
            // Liste des prises
            if(data.nbPrise > 1)
            {
                for(int i = 0; i < data.nbPrise; i++)
                {
                    board[data.posPrises[i].x][data.posPrises[i].y] = NULL;
                }
            }
            else if(data.nbPrise == 1)
            {
                board[data.posPrises[0].x][data.posPrises[0].y] = NULL;
            }

            currentPlayer->score += data.nbPrise;

            // Changement de joueur
            if(currentPlayer->team == player1.team){
                currentPlayer = &player2;
            }else{
                currentPlayer = &player1;
            }

            // Affichage background
            showSdlBackground(orange);

            // Affichage du player en cours
            showSdlPlayer();

            // Affichage du message d'indication
            sdlWrite(infoMessage, text, positionText, police, black);

            // Affichage plateau
            showSdlBoard();

            // Renderer Update
            SDL_RenderPresent(renderer);
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
    thread_param.nbPrise = 0;
    return 0;
}


int gui()
{
    positionBoard.x = caseWidth; 
    positionBoard.y = caseWidth * 4;
    SDLboard.x = 0; SDLboard.y = 0; 
    SDLboard.h = caseWidth + 9 * caseWidth; SDLboard.w = caseWidth + 9 * caseWidth;
    orange.r = 255; orange.g = 127; orange.b = 40; orange.a = 255;
    black.r = 0; black.g = 0; black.b = 0; black.a = 255;
    blue.r = 0; blue.g = 0; blue.b = 255; blue.a = 255;
    white.r = 255; white.g = 255; white.b = 255; white.a = 255;
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
    SDL_SetWindowTitle(window, "jeu2Dame");

    
    if(loadPolices() == EXIT_FAILURE) goto Quit;
    positionText.x = caseWidth;
    positionText.y = caseWidth * 2;
    infoMessage = "";

    loadTextures("default");

    /* Initialize only SDL Audio on default device */
    if(SDL_Init(SDL_INIT_AUDIO) < 0)
    {
        return 1; 
    }

    struct Menu startMenu = loadStartMenu();
    int resultMenu;
    while((resultMenu = showMenu(startMenu)) != 2){

        // Mode réseau
        if(resultMenu == 1)
        {
            printf("Entrez le port source \n");
            scanf("%d", &thread_param.port_src);
            printf("Entrez le port destination \n");
            scanf("%d", &thread_param.port_des);

            char response[32];
            if(network_client(response, thread_param.port_des) == EXIT_FAILURE)
            {
                network_server("OK !", thread_param.port_src);
                tour = 1;
            }else{
                tour = 0;
            }
            network = 1;
        }else{
            tour = 1;
        }

        // Lancement partie
        game();

        // Titre menu
        startMenu.title = calloc(sizeof(currentPlayer->name)+sizeof(" gagne"), sizeof(startMenu.title));
        strcpy(startMenu.title,currentPlayer->name);
        strcat(startMenu.title," gagne");
        startMenu.textPosition.x = caseWidth * 3;
    }
    

    statut = EXIT_SUCCESS;

Quit:

    if(NULL != renderer) SDL_DestroyRenderer(renderer);
    if(NULL != window) SDL_DestroyWindow(window);
    if(NULL != textureDame1) SDL_DestroyTexture(textureDame1);
    if(NULL != textureDame2) SDL_DestroyTexture(textureDame2);
    if(NULL != texturePion1) SDL_DestroyTexture(texturePion1);
    if(NULL != texturePion2) SDL_DestroyTexture(texturePion2);
    freeMenu(startMenu);
    
    /* End Simple-SDL2-Audio */
    endAudio();

    if (pthread_join(thread, NULL)) {
	    perror("pthread_join");
    }
    SDL_Quit();


    return statut;
}


