#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Creer une fenetre et renvoie le renderer
 * On recupere aussi l'adresse du pointeur de la fenetre
 */
SDL_Renderer * createWindow(int height, int width, SDL_Window **window)
{
    *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Color orange = {255, 127, 40, 255};

    /* Initialisation, création de la fenêtre et du renderer. */
    if(0 != SDL_Init(SDL_INIT_VIDEO))
    {
        fprintf(stderr, "Erreur SDL_Init : %s\n", SDL_GetError());
        return NULL;
    }
    *window = SDL_CreateWindow("SDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              height, width, SDL_WINDOW_SHOWN);
    if(NULL == *window)
    {
        fprintf(stderr, "Erreur SDL_CreateWindow : %s\n", SDL_GetError());
        return NULL;
    }
    renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    if(NULL == renderer)
    {
        fprintf(stderr, "Erreur SDL_CreateRenderer : %s\n", SDL_GetError());
        return NULL;
    }

    /* C’est à partir de maintenant que ça se passe. */
    if(0 != SDL_SetRenderDrawColor(renderer, orange.r, orange.g, orange.b, orange.a))
    {
        fprintf(stderr, "Erreur SDL_SetRenderDrawColor : %s\n", SDL_GetError());
        return NULL;
    }

    if(0 != SDL_RenderClear(renderer))
    {
        fprintf(stderr, "Erreur SDL_SetRenderDrawColor : %s\n", SDL_GetError());
        return NULL;
    }



    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    return renderer;
}

/**
 * Load a picture and return the Texture
 */
SDL_Texture *loadImage(const char path[], SDL_Renderer *renderer, SDL_Point point)
{
    // On place le curseur au coordonnees
    SDL_Rect dst = {point.x, point.y, 0, 0};

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
    SDL_QueryTexture(texture, NULL, NULL, &dst.w, &dst.h);

    // On affiche la texture sur le renderer
    SDL_RenderCopy(renderer, texture, NULL, &dst);
    SDL_DestroyTexture(texture);
    
    return texture;
}

/**
 * 1. Fonction qui detecte les evenements
 * 2. Agit en fonction 
 * 3. Et retourne
 *      - Si evenement : Retourne le code de l'evenement SQL
 *      - Si pas d'evenement : Retourne 0
 *      - Erreur : -1
 */
int input(SDL_Event event){
   switch(event.type){

        case SDL_MOUSEBUTTONUP:
            if(event.button.button == SDL_BUTTON_LEFT)
            {
                printf("Click !\n");
                printf("x = : %d\ty : %d\n", event.button.x, event.button.y);
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

int main(int argc, char *argv[])
{
    int statut = EXIT_FAILURE;


    // Creation de la fenetre
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    renderer = createWindow(640, 480, &window);
    if(renderer == NULL)
    {
        goto Quit;
    }


    // Ajout image
    SDL_Texture *texture = NULL;
    SDL_Point point;
    point.x = 100; point.y = 100;
    texture = loadImage("images/iop.bmp", renderer, point);
    if(NULL == texture)
    {
        goto Quit;
    }


    // Renderer Update
    SDL_RenderPresent(renderer);
    SDL_Event event;
    SDL_bool quit = SDL_FALSE;

    while(!quit)
    {
        while(SDL_PollEvent(&event))
        {
            if(input(event) == SDL_QUIT)
                quit = SDL_TRUE;
        }
        SDL_Delay(30);
    }

    statut = EXIT_SUCCESS;

Quit:
    if(NULL != renderer)
        SDL_DestroyRenderer(renderer);
    if(NULL != window)
        SDL_DestroyWindow(window);
    SDL_Quit();
    return statut;
}