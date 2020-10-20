#include "pipenStuff.hpp"

int main(int argc, char* argv[]){

 
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        printf("error initializing SDL: %s\n", SDL_GetError());
        return -1;
    }

   /* if (!IMG_Init(IMG_INIT_JPG))
    {
        std::cout << "Failed to init JPG images " << SDL_GetError() << std::endl;
    }*/

    SDL_Window *window = SDL_CreateWindow("Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 720, SDL_WINDOW_OPENGL);
    if (window == nullptr)
    
    {
        printf("Could not create a window: %s", SDL_GetError());
        return -1;
    }

    int windowWidth;
    int windowHeight;

    SDL_GetWindowSize(window, &windowWidth, &windowHeight);

    //printf("Window is %d by %d\n", windowWidth, windowHeight);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        printf("Could not create a renderer: %s", SDL_GetError());
        return -1;
    }

    std::unordered_map<std::string, game::Scene> sceneMap;

    sceneMap.try_emplace("MainMenu", "MainMenu", renderer, windowWidth, windowHeight);
    
    pipen::loadMainMenu(&sceneMap["MainMenu"]);

    sceneMap["MainMenu"].enterScene();


    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    printf("Program exited succesfully\n");
    return 0;
}