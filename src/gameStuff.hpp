#ifndef INCLUDE_GAMESTUFF_HPP
#define INCLUDE_GAMESTUFF_HPP

//#define SDL_MAIN_HANDLED

//#include "../include/SDL_main.h"
//#include "../include/SDL.h"
//#include "../include/SDL_image.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <iostream>
#include <unordered_map>
#include <vector>
#include <thread>
#include <functional>

#include <filesystem>
namespace fs = std::filesystem;

namespace game
{

    class System
    {
    public:
        virtual bool isRenderingSystem();
        virtual bool isPhysicsSystem();
        virtual void upadateFrame(SDL_Renderer *renderer) {}
        virtual void upadatePhysics(){}
        virtual ~System(){}
    };
    struct Component{
        private:
        uint16_t entID;
        
        public:
        Component(uint16_t id){
            entID = id;
        }

        uint16_t getEntID(){
            return entID;
        }
    };
    
    struct Entity
    {
    private:
        uint16_t ID;
        std::unordered_map<std::string, Component *> components;

    public:
        Entity(){
            ID = 0;
        }
        Entity(uint16_t id){
            ID = id;
        }
        uint16_t getID(){
            return ID;
        }
        void addComponent(std::string name, Component *comp);

        Component *getComponent(std::string name);
    };

    struct Camera
    {
    private:
        int offsetX = 0;
        int offsetY = 0;
        float scale = 1.0f;
        int frameWidth;
        int frameHeight;

    public:
        Camera(){}
        Camera(int fW, int fH) {
            frameWidth = fW;
            frameHeight = fH;
        }

        int getOffSetX() { return offsetX; }
        int getOffSetY() { return offsetY; }
        float getScale() { return scale; }
        int getFrameWidth() { return frameWidth; }
        int getFrameHeight() { return frameHeight; }
        void setOffsetX(int newX) 
        {
            offsetX = newX;
        }
        
        void setOffsetY(int newY)
        {
            offsetY = newY;
        }

        void changeScale(int deltaScale) {
            scale += deltaScale / 10.0;
        }

        void zoomAt(int mX, int mY, int dir) {
            printf("zooming x: %d, y: %d\n", mX, mY);
            printf("Scale: %f\n", scale);
            if(scale <= 0.1 && dir == -1) return;

            int worldSpaceX = mX / scale;
            int worldSpaceY = mY / scale;

            float oldScale = scale;

            scale += (dir / 10.0);

            float propScale = oldScale / scale;
            
            int dx = (int)((mX - offsetX) * (propScale - 1));
            int dy = (int)((mY - offsetY) * (propScale - 1));
            offsetX += dx;
            offsetY += dy;

            // float origScale = scale;
            // scale += scaleD / 10.0;

            // int dX = offsetX - mX;
            // int dY = offsetY - mY;

            // offsetX = (int)(offsetX + dX * scale);
            // offsetY = (int)(offsetY + dY * scale);

            // int worldMX = mX / scale + offsetX;
            // int worldMY = mY / scale + offsetY;

            
        }
    };

    struct SpriteRenderer : public Component
    {
    private:
        SDL_Texture *texture;
        SDL_Rect rectangle;
        bool displayed = true;

    public:
        SpriteRenderer(SDL_Texture *text, SDL_Rect rect, uint16_t id) : Component(id){
            texture = text;
            rectangle = rect;
            
        }
        ~SpriteRenderer(){
            //SDL_DestroyTexture(texture);
        }
        bool isDisplayed(){
            return displayed;
        }
        void setDisplayed(bool display){
            displayed = display;
        }

        void drawSprite(SDL_Renderer *renderer){
            if(!displayed) return;
            SDL_RenderCopy(renderer, texture, NULL, &rectangle);
        }
        void drawSpriteOffset(SDL_Renderer *renderer, Camera * camera){
            if(!displayed) return;
            SDL_Rect offsetRectangle;
            offsetRectangle.x = rectangle.x * camera->getScale() + camera->getOffSetX();
            offsetRectangle.y = rectangle.y * camera->getScale() + camera->getOffSetY();

            // offsetRectangle.x = (rectangle.x + camera->getOffSetX());
            // offsetRectangle.y = (rectangle.y + camera->getOffSetY());

            offsetRectangle.w = rectangle.w * camera->getScale();
            offsetRectangle.h = rectangle.h * camera->getScale();
            SDL_RenderCopy(renderer, texture, NULL, &offsetRectangle);
        }
    };

    struct SpriteRenderSystem : public System
    {
    private:
        // std::vector<game::SpriteRenderer> backgroundSprites;
        // std::vector<game::SpriteRenderer> gameSprites;
        // std::vector<game::SpriteRenderer> uISprites;

        std::vector<std::vector<game::SpriteRenderer> *> layers;
        std::vector<std::vector<game::SpriteRenderer> *> layersUI;

        Camera * sceneCamera;
        Scene * scenePtr = scene;

    public:
        SpriteRenderSystem(Camera *cam, Scene scene) {
            sceneCamera = cam;
            scenePtr = scene;
        }

        ~SpriteRenderSystem()
        {
            for(auto & vector: layers){
                delete vector;
            }
        }

        bool isRenderingSystem() { return true; }
        bool isPhysicsSystem() { return false; }

        void addSprite(SpriteRenderer sprite, int layer){
            while(layers.size() <= layer){
                layers.push_back(new std::vector<game::SpriteRenderer>);
            }
            layers[layer]->push_back(sprite);
        }

        void addUISprite(SpriteRenderer sprite, int layer)
        {
            while (layersUI.size() <= layer)
            {
                layersUI.push_back(new std::vector<game::SpriteRenderer>);
            }
            layersUI[layer]->push_back(sprite);
        }

        void upadateFrame(SDL_Renderer *renderer){
            

            
            for(auto & layer: layers)
            {
                for(auto it = layer->begin(); it != layer->end(); it++)
                {
                    it->drawSpriteOffset(renderer, sceneCamera);
                }
            }

            for (auto &layer : layersUI)
            {
                for (auto it = layer->begin(); it != layer->end(); it++)
                {
                    it->drawSprite(renderer);
                }
            }
        }

    };

    struct Scene
    {
    private:
        std::string sceneName;
        std::unordered_map<std::string, System *> systemsMap;
        std::unordered_map<uint16_t, Entity> entities;
        std::unordered_map<std::string, SDL_Texture *> textureMap;
        SDL_Renderer *renderer;
        uint16_t frametime = 10;
        uint16_t tickTime = 1000;
        bool rendering = false;
        bool runningUI = false;
        bool runningPhysics = false;
        Camera camera;
        uint16_t entity_counter = 0;

    public:
        Scene() {}

        Scene(std::string name, SDL_Renderer *rend, int frameWidth, int frameHeight)
        {
            sceneName = name;
            renderer = rend;
            camera = Camera(frameWidth, frameHeight);
        }
        ~Scene() {
            printf("Deleting scene: %s\n", sceneName.c_str());
            for(std::pair<std::string, SDL_Texture *> entry: textureMap){
                SDL_DestroyTexture(entry.second);
            }
            for (auto it = systemsMap.begin(); it != systemsMap.end(); it++)
            {
                delete it->second;
            }
        }

        Camera *getCameraPointer() { return &camera; }

        void addEntity(Entity e){
            entities[e.getID()] = e;
        }

        Entity * getEntityPtr(uint16_t id){
            return &entities[id];
        }

        uint16_t getNextEntityID(){
            return entity_counter++;
        }

        void setRunningUI(bool setting)
        {
            runningUI = setting;
        }
        void setRendering(bool setting){
            rendering = setting;
        }

        void renderThread();
        void physicsThread();
        void enterScene();
        void exitScene(){
        }

        void addSystem(std::string name, System *sys)
        {
            systemsMap[name] = sys;
        }

        SDL_Texture * getImagePtrFromName(std::string name){
            return textureMap[name];
        }

        void loadImgFromDir(std::string dir){
            dir = "img/MainMenu/";
            printf("Started loading images\n");

            for (auto& entry : fs::directory_iterator(dir)) {
                std::cout << "Loading: " << entry.path().stem().string() << std::endl;

                SDL_Surface* newS = IMG_Load(entry.path().string().c_str());

                if (!newS) printf("surface null\n");

                SDL_Texture* newT = SDL_CreateTextureFromSurface(renderer, newS);
                if (!newT) printf("texture null\n");

                SDL_FreeSurface(newS);

                textureMap[entry.path().stem().string()] = newT;


            }

            /*for(auto & entry : fs::directory_iterator(dir)){
                SDL_Texture *texture = IMG_LoadTexture(renderer, entry.path().string().c_str());
                textureMap[entry.path().stem().string()] = texture;
                std::cout<<entry.path().stem().string()<<" Loaded"<<std::endl;
            }
            printf("%d sprites loaded\n", textureMap.size());*/

        }
    };

    struct Button : public Component
    {
    private:
        SDL_Rect rectangle;
        std::function<void(game::Scene *)> function;
       
    public:
        Button(SDL_Rect rect, std::function<void(game::Scene *)> func, uint16_t id) : Component(id)
        {
            rectangle = rect;
            function = func;
            
        }

        bool onButton(int x, int y)
        {
            printf("x: %d, y: %d,   \trx: %d, ry: %d, rw: %d, rh: %d\n", x, y, rectangle.x, rectangle.y, rectangle.w, rectangle.h);
            return rectangle.x <= x && rectangle.x + rectangle.w >= x && rectangle.y <= y && rectangle.y + rectangle.h >= y;
        }

        void pressButton(Scene* scene);

        void setActive(bool active, Scene * scene){
            scene->getEntityPtr(entID)->getComponent("SpriteRenderer")->setDisplayed(active);
        }
    };

    struct ButtonLayer {
    private:
        std::vector<Button> buttons;
        bool active = true;

    public:
        ButtonLayer() {}

        void addButton(Button button){
            buttons.push_back(button);
        }

        bool isActive(){
            return active;
        }

        bool mouseOnButton(int x, int y){
            for (auto & button: buttons){ 
                if(button.onButton(x, y)){ 
                    return true;
                }
            }

            return false;
        }

        void pressButtonAt(int x, int y, Scene *scene) {
            for (auto & button: buttons) {
                if (button.onButton(x, y)) {
                    button.pressButton(scene);
                    return;
                }
            }
        }

        void setLayerActive(bool active, Scene scene){
           for(auto & button: buttons){
               button.setActive(active, scene);
           }
        }
    };

    struct ButtonSystem : public System
    {
    private:
        //std::vector<Button> buttons;
        std::vector<ButtonLayer> layers;
        Scene * scenePtr;
    public:
        ButtonSystem(Scene * scene)
        {
            scenePtr = scene;
            printf("button sys made\n");
        }
        virtual ~ButtonSystem(){}

        bool isRenderingSystem() { return false; }
        bool isPhysicsSystem() { return false; }

        void addButtonToLayer(Button button, int layer)
        {
            //buttons.push_back(button);
            while(layers.size() <= layer){ 
                layers.emplace(layers.end());
            }
            layers[layer].addButton(button);
        }

        bool mouseOnButton(int x, int y)
        {

            // if (buttons.size() == 0)
            //     return false;
            for(int i = layers.size() - 1; i >= 0; i--)
            {
                if(layers[i].mouseOnButton(x, y)){ 
                    return true;
                }
            }

            return false;
        }
        void pressButtonAt(int x, int y, Scene *sys);

        void setLayerActive(uint16_t layer, bool active){
            layers[layer].setActiveLayer(active, scenePtr);
        }
    };

} // namespace game

#endif
