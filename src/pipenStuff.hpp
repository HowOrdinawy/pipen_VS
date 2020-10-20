#include "gameStuff.hpp"

namespace pipen{

    struct BoardSystem: public game::System{
    private:
        std::vector<std::vector<int>> board;

    public:
        bool isPhysicsSystem() { return true; }
        void upadatePhysics(){
            //Do board updating
            printf("tick\n");
        }
    };

    void exitButton(game::Scene *mMScene){
        printf("Exit button\n");
        mMScene->setRunningUI(false);
        mMScene->setRendering(false);
        mMScene->exitScene();
    }

    void loadMainMenu(game::Scene *mMScene){
        
        game::SpriteRenderSystem* srs = new game::SpriteRenderSystem(mMScene->getCameraPointer());
        mMScene->addSystem("SpriteRendererSystem", srs);

        BoardSystem * board = new BoardSystem();
        mMScene->addSystem("BoardSystem", board);

        mMScene->loadImgFromDir("img/MainMenu");

        game::ButtonSystem* bs = new game::ButtonSystem();
        SDL_Rect rect;
        rect.x = 10;
        rect.y = 10;
        rect.h = 100;
        rect.w = 100;
        // void (*exitFunction)(game::Scene *);
        // exitFunction = &exitButton;

        std::function<void(game::Scene *)> exitBF = exitButton;

        game::Button exitB(rect, exitBF);
        bs->addButtonToLayer(exitB, 0);
        mMScene->addSystem("ButtonSystem", bs);

        game::SpriteRenderer sprite(mMScene->getImagePtrFromName("5"), rect);
        srs->addUISprite(sprite, 1);

        srs->addSprite(sprite, 1);

        rect.x = 200;
        rect.y = 200;
        rect.h = 150;
        rect.w = 100;

        printf("sdfg");

        game::SpriteRenderer sprite2(mMScene->getImagePtrFromName("5"), rect);

        srs->addSprite(sprite2, 1);
    }

 
}