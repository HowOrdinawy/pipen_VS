
#include "gameStuff.hpp"

namespace game
{
    void Scene::renderThread()
    {
        uint32_t thisFrameTime;


        while (rendering)
        {
            thisFrameTime = SDL_GetTicks();

            SDL_RenderClear(renderer);
            //Do rendering stuff
            SDL_SetRenderDrawColor(renderer, 255, 128, 128, 255);

            for (auto it = systemsMap.begin(); it != systemsMap.end(); it++)
            {
                if (it->second->isRenderingSystem())
                {
                    it->second->upadateFrame(renderer);
                }
            }

            SDL_RenderPresent(renderer);

            if (SDL_GetTicks() - thisFrameTime < frametime)
                SDL_Delay(frametime - (SDL_GetTicks() - thisFrameTime));
        }
    }

    void Scene::physicsThread()
    {
        uint32_t thisTickTime;

        while(runningPhysics){
            thisTickTime = SDL_GetTicks();

            for (auto it = systemsMap.begin(); it != systemsMap.end(); it++)
            {
                if (it->second->isPhysicsSystem())
                {
                    it->second->upadatePhysics();
                }
            }

            if (SDL_GetTicks() - thisTickTime < tickTime)
                SDL_Delay(tickTime - (SDL_GetTicks() - thisTickTime));
        }
    }

    void Scene::enterScene()
    {
        //std::thread UIThread(&Scene::uIThread, this);

        rendering = true;
        std::thread RenderThread(&Scene::renderThread, this);

        bool hasPhysics = false;
        for (auto it = systemsMap.begin(); it != systemsMap.end(); it++)
        {
            if (it->second->isPhysicsSystem())
            {
                runningPhysics = true;
                break;
            }
        }
        
        std::thread PhysicsThread(&Scene::physicsThread, this);
        

        runningUI = true;
        SDL_Event event;

        bool mousePressed = false;
        int mouseStartX;
        int mouseStartY;
        bool pressedButton;
        int camStartX = 0;
        int camStartY = 0;

        while (runningUI)
        {
            if (SDL_WaitEvent(&event))
            {
                // printf("Detected event\n");
                switch (event.type)
                {
                case SDL_QUIT:
                    rendering = false;
                    runningUI = false;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    //printf("Mouse press at (%d, %d)\n", event.motion.x, event.motion.y);

                    if (systemsMap.count("ButtonSystem"))
                    {
                        ButtonSystem *butSys = (ButtonSystem *)systemsMap["ButtonSystem"];
                        if (butSys->mouseOnButton(event.motion.x, event.motion.y))
                        {
                            butSys->pressButtonAt(event.motion.x, event.motion.y, this);
                            pressedButton = true;
                            break;
                        }
                    }

                    mousePressed = true;
                    camStartX = camera.getOffSetX();
                    camStartY = camera.getOffSetY();
                    mouseStartX = event.motion.x;
                    mouseStartY = event.motion.y;

                    break;

                case SDL_MOUSEMOTION:
                    //printf("mortino event\n");
                    if (mousePressed)
                    {
                        camera.setOffsetX(camStartX + (-mouseStartX + event.motion.x));
                        camera.setOffsetY(camStartY + (-mouseStartY + event.motion.y));
                    }
                    break;

                case SDL_MOUSEBUTTONUP:
                    mousePressed = false;
                    pressedButton = false;
                    break;

                case SDL_MOUSEWHEEL:
                    //camera.changeScale(event.wheel.y);
                    int x;
                    int y;
                    SDL_GetMouseState(&x, &y);
                    camera.zoomAt(x, y, event.wheel.y);
                    break;
                }
            }
        }

        if (PhysicsThread.joinable())
        {
            runningPhysics = false;

            PhysicsThread.join();
        }
        RenderThread.join();

        printf("Scene exited\n");
    }
} // namespace game