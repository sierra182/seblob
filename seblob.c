#include <SDL.h>
#include <stdio.h>
#include <stdbool.h>

#define ROWS 100
#define COLS 150
#define CELL_SIZE 5

typedef struct {

    bool isAlive;
    bool isAliveNextState; 
    SDL_Rect destRect;

} Cell;

void actualizeNextState(Cell matrix[ROWS][COLS]) {

    for (int i = 0; i < ROWS; i++) {

        for (int j = 0; j < COLS; j++) {
            
            matrix[i][j].isAlive = matrix[i][j].isAliveNextState;
        }
    }
}

void displayMatrix(SDL_Renderer** renderer, Cell matrix[ROWS][COLS]) {
    
    SDL_SetRenderDrawColor(*renderer, 255, 255, 255, 255);

    for (int i = 0; i < ROWS; i++) {

        for (int j = 0; j < COLS; j++) {

            if(matrix[i][j].isAlive) {

                SDL_RenderFillRect(*renderer, &matrix[i][j].destRect);
            }             
        }
    }
}

void initMatrix(Cell matrix[ROWS][COLS]) {

    for (int i = 0; i < ROWS; i++) {

        for (int j = 0; j < COLS; j++) {

            SDL_Rect destRect;
            destRect.w = CELL_SIZE *.9;
            destRect.h = CELL_SIZE *.9;
            destRect.x = CELL_SIZE * j;
            destRect.y = CELL_SIZE * i;

            matrix[i][j].isAlive = false;
            matrix[i][j].isAliveNextState = false;
            matrix[i][j].destRect = destRect;
        }
    }
}

int initWindow(SDL_Window** window, SDL_Renderer** renderer, int width, int height) {

    if (SDL_Init(SDL_INIT_VIDEO) < 0) { 

        printf("Erreur d'initialisation de SDL: %s\n", SDL_GetError());
        return 1;
    }

    *window = SDL_CreateWindow("Life",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              width,
                              height,
                              SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
                           
    if (*window == NULL) {
      
        printf("Erreur de création de la fenêtre: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    if (*renderer == NULL) {      

        printf("Erreur de création du renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(*window);
        SDL_Quit();
        return 1;
    }    
    return 0;
}

int SDL_main(int argc, char* argv[]) {

 	SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
   
    int windowWidth = COLS * CELL_SIZE;
    int windowHeight = ROWS * CELL_SIZE;
    
    if (initWindow(&window, &renderer, windowWidth, windowHeight)) {
       	return 1;
    }
    
    Cell matrix[ROWS][COLS];
    initMatrix(matrix); 
       
    bool autoGen = false;
    SDL_Event event;	
    bool quit = false;
    while (!quit) {

        while(SDL_PollEvent(&event)) {

            if (event.type == SDL_QUIT) {
                quit = 1;
            }
        }
        
        SDL_Delay(16);
        SDL_SetRenderDrawColor(renderer, 64, 224, 208, 0);
        SDL_RenderClear(renderer);
                
        actualizeNextState(matrix);
        displayMatrix(&renderer, matrix);

        SDL_RenderPresent(renderer);  

        if (!autoGen)
        if(getchar() == 'a') {
            autoGen = true;
        }     
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
	return 0;
}
