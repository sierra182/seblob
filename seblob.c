#include <SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define ROWS 100
#define COLS 150
#define CELL_SIZE 5

typedef struct Cell {

    struct Cell* leftChild;
    struct Cell* middleChild;
    struct Cell* rightChild;

    int mana;
    int cellDepth;
    int veinDepth;

    bool isAlive;
    bool isAliveNextState; 
    SDL_Rect destRect;

} Cell;

void createNewNode(Cell* cell) {
    
    Cell* leftChild = calloc(1, sizeof(Cell));
    Cell* middleChild = calloc(1, sizeof(Cell));
    Cell* rightChild = calloc(1, sizeof(Cell));

    if (leftChild == NULL || middleChild == NULL || rightChild == NULL) {

        disposeAll();
        exit(1);
    }
     
    cell->leftChild = leftChild; 
    cell->middleChild = middleChild;
    cell->rightChild = rightChild; 

    leftChild->cellDepth = 0;
    middleChild->cellDepth = cell->cellDepth + 1;
    rightChild->cellDepth = 0;

    leftChild->veinDepth = cell->veinDepth + 1;
    middleChild->veinDepth = cell->veinDepth;
    rightChild->veinDepth = cell->veinDepth + 1;

    builder(cell->leftChild);
    builder(cell->middleChild);
    builder(cell->rightChild);
}

void createNewCell(Cell* cell) {
     
    Cell* middleChild = calloc(1, sizeof(Cell));

    if (middleChild == NULL) {

        disposeAll();
        exit(1);
    }
    
    cell->middleChild = middleChild; 
    middleChild->cellDepth = cell->cellDepth + 1;
    middleChild->veinDepth = cell->veinDepth;

    SDL_Rect middleChildDestRect = cell->destRect;
    middleChildDestRect.y += CELL_SIZE;    
    middleChild->destRect = middleChildDestRect; 
    
    builder(cell->middleChild);  
}

int isInteger(float num) {

    int intPart = (int) num;

    if (num - intPart == 0) {
        return 1;
    }
    return 0;
}

void builder(Cell* cell) {
    
   int gap = 3.0; 
   int cellMaxDepth = 100;

   if (cell->veinDepth < 3) {

        if (cell->cellDepth != 0 && isInteger(cell->cellDepth / gap)) {

            createNewNode(cell);
        }
        else if (cell->cellDepth < cellMaxDepth) {
            createNewCell(cell);
        }    
    }       
}

void cellLifeCycle() {

    Cell* mother = calloc(1, sizeof(Cell));
    builder(mother);   
}

void getOppositeSide(const int localRaw, const int localCol, const int raw, const int col, const int* oppRaw, const int* oppCol) {

    if (localRaw == 0 && localCol == 1) {

        oppRaw = raw + 2;
        oppCol = col; 
    }
    else if (localRaw == 2 && localCol == 1) {

        oppRaw = raw - 2;
        oppCol = col;
    }
    else if (localCol == 0 && localRaw == 1){
        
        oppCol = col + 2;
        oppRaw = raw;
    }
    else if (localCol == 2 && localRaw == 1){
        
        oppCol = col - 2;
        oppRaw = raw;
    }
}

void submitBlobRules(Cell matrix[ROWS][COLS]) {

    for (int i = 0; i < ROWS; i++) {

        for (int j = 0; j < COLS; j++) {

            int aliveCount = 0;
            int nighCellOppRaw = 0;
            int nighCellOppCol = 0;

            for (int k = 0, l = i - 1; k < 3; k++, l++) {    

                for (int m = 0, n = j - 1; m < 3; m++, n++) {
                        
                    if (!(k == 1 && m == 1 || l < 0 || n < 0 || l > ROWS - 1 || n > COLS - 1)) {
                            
                        if (matrix[l][n].isAlive) {
                            
                            aliveCount++; 
                            getOppositeSide(k, m, l, n, &nighCellOppRaw, &nighCellOppCol);                                              
                        }                                                   
                    }       
                }                                  
            }

            if (aliveCount == 1) {

                matrix[nighCellOppRaw][nighCellOppCol].isAliveNextState = true;
            }
        }
    }
}

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

    *window = SDL_CreateWindow("SeBlob",
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
   
    const int windowWidth = COLS * CELL_SIZE;
    const int windowHeight = ROWS * CELL_SIZE;
    
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
