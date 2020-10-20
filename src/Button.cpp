#include "gameStuff.hpp"
namespace game{


    void Button::pressButton(Scene* scene)
        {
            printf("calling fucnton\n");
            function(scene);
            printf("Function called\n");
            //printf("Button pressed\n");
        }
}